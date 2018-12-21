#include "encode.h"

static void concat_path(char *dest, char *base, struct be_node *list)
{
    strcpy(dest, base);

    int len = 0;
    while (list->element.list[len])
    {
        char *str = list->element.list[len]->element.str->content;
        strcat(dest, "/");
        strcat(dest, str);
        len++;
    }
}

static int check_single_file(struct be_node *info)
{
    if (!info->element.dict[1] || strcmp("name",
                                         info->element.dict[1]->key->content))
        return 1;

    if (!info->element.dict[3] || strcmp("pieces",
                                         info->element.dict[3]->key->content))
        return 1;

    int res = 1;
    //int length = info->element.dict[0]->val->element.num;
    char *filename = info->element.dict[1]->val->element.str->content;
    char *pieces = info->element.dict[3]->val->element.str->content;

    FILE *f = fopen(filename, "r");
    if (!f)
        return 1;

    int res_size = 0;
    int nb_hash = -1;
    unsigned char buf[262144];
    unsigned char hash[SHA_DIGEST_LENGTH];
    unsigned char *res_buf = NULL;

    int r = 0;
    while ((r = fread(buf, sizeof(char), 262144, f)) > 0)
    {
        res_size += SHA_DIGEST_LENGTH;
        res_buf = realloc(res_buf, res_size * sizeof(char));
        SHA1(buf, r, hash);
        nb_hash++;
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
            res_buf[SHA_DIGEST_LENGTH * nb_hash + i] = hash[i];
    }

    void *tmp = res_buf;
    char *calculated_pieces = tmp;
    if (!memcmp(calculated_pieces, pieces, res_size * sizeof(char)))
        res = 0;

    free(res_buf);
    fclose(f);
    return res;
}

static char *hash_multiple_files(char *path, int file_length, int last_elem,
                                 int *len)
{
    static char *res = NULL;
    static unsigned char buf[262144];
    static int idx = 0;
    unsigned char hash[SHA_DIGEST_LENGTH];
    static int res_size = 0;
    int nb_hash = -1;

    struct stat st;
    stat(path, &st);
    if (st.st_size != file_length)
    {
        *len = -1;
        return NULL;
    }

    FILE *f = fopen(path, "r");
    if (!f)
    {
        *len = -1;
        return NULL;
    }
    int r = 0;
    int overflow = 0;
    int bytes_to_read = file_length;
    if (idx + bytes_to_read > 262144)
    {
        overflow = 1;
        bytes_to_read = 262144 - idx;
    }
    while ((r = fread(buf + idx, sizeof(char), bytes_to_read, f)) > 0)
    {
        idx += r;
        if (last_elem && idx != 262144)
            break;
        if (!last_elem)
            buf[idx] = '\0';
        if (overflow)
        {
            file_length -= bytes_to_read;
            if (file_length <= 262144)
            {
                overflow = 0;
                bytes_to_read = file_length;
            }
            else
                bytes_to_read = 262144;
        }
        idx = 0;
        res_size += SHA_DIGEST_LENGTH;
        res = realloc(res, res_size * sizeof(char));
        SHA1(buf, r, hash);
        nb_hash++;
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
            res[SHA_DIGEST_LENGTH * nb_hash + i] = hash[i];
    }
    fclose(f);

    *len = res_size;

    void *tmp = res;
    char *ress = tmp;
    return ress;
}

static int check_folder(struct be_node *info)
{
    if (!info->element.dict[1] || strcmp("name",
                                         info->element.dict[1]->key->content))
        return 1;

    if (!info->element.dict[3] || strcmp("pieces",
                                         info->element.dict[3]->key->content))
        return 1;

    int res = 1;
    char *basename = info->element.dict[1]->val->element.str->content;
    char *pieces = info->element.dict[3]->val->element.str->content;
    char *calculated_pieces = NULL;
    int pieces_len = 0;

    int last = 1;
    int file_length = 0;
    int files_count = 0;
    char path[4096];
    struct be_node *file_node =
        info->element.dict[0]->val->element.list[files_count];
    while (file_node)
    {
        file_length = file_node->element.dict[0]->val->element.num;
        concat_path(path, basename, file_node->element.dict[1]->val);

        if (info->element.dict[0]->val->element.list[files_count + 1] == NULL)
            last = 0;

        calculated_pieces = hash_multiple_files(path, file_length, last,
                                                &pieces_len);
        if (pieces_len == -1)
            return 1;

        file_node = info->element.dict[0]->val->element.list[++files_count];
    }

    if (!memcmp(calculated_pieces, pieces, pieces_len * sizeof(char)))
        res = 0;
    free(calculated_pieces);
    return res;
}

static int check_be_node(struct be_node *info)
{
    if (!info->element.dict[0])
        return 1;
    if (!strcmp("length", info->element.dict[0]->key->content))
        return check_single_file(info);
    if (!strcmp("files", info->element.dict[0]->key->content))
        return check_folder(info);
    return 1;
}

int check_integrity(char *path)
{
    //Return 1 on Error, 0 otherwise

    //get the size of the file in order to create the right fitted buffer
    struct stat st;
    stat(path, &st);
    char *file = calloc(st.st_size, sizeof(char));

    //printf("DEBUG : size of file buffer is : %d (should be equal to char number)\n", (int)st.st_size);

    //Open the file and fill the buffer
    FILE *f = fopen(path, "r");
    if (!f)
    {
        free(file);
        return 1;
    }
    if (fread(file, sizeof(char), st.st_size, f) <= 0)
    {
        //Error while trying to read file
        free(file);
        fclose(f);
        return 1;
    }
    fclose(f);

    //Decode the buffer and parses it into a be_node tree
    struct be_node *tree = be_decode(file, st.st_size);
    free(file);
    if (!tree)
    {
        return 1;
    }
    int res = 1;
    if (tree->type == BE_DICT && tree->element.dict && tree->element.dict[0] &&
        tree->element.dict[1] && tree->element.dict[2] && tree->element.dict[3])
        res = check_be_node(tree->element.dict[3]->val);
    be_free(tree);
    return res; //error then
}