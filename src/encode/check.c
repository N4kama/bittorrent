#include "encode.h"

static int check_single_file(struct be_node *info)
{
    if (!info->element.dict[1] || strcmp("name", info->element.dict[1]->key->content))
        return 1;

    if (!info->element.dict[3] || strcmp("pieces", info->element.dict[3]->key->content))
        return 1;

    int res = 1;
    int length = info->element.dict[0]->val->element.num;
    char *filename = info->element.dict[1]->val->element.str->content;
    char *pieces = info->element.dict[3]->val->element.str->content;

    FILE *f = fopen(filename, "r");
    if (!f)
        return 1;

    int res_size = 0;
    int nb_hash = -1;
    unsigned char buf[262144];
    unsigned char hash[SHA_DIGEST_LENGTH];
    unsigned char res = NULL;

    int r = 0;
    while ((r = fread(buf, sizeof(char), 262144, f) > 0)
    {
        res_size += SHA_DIGEST_LENGTH;
        res = realloc(res, res_size * sizeof(char));
        SHA1(buf, r, hash);
        nb_hash++;
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
            res[SHA_DIGEST_LENGTH * nb_hash + i] = hash[i];
    }

    void *tmp = res;
    char *calculated_pieces = tmp;
    if (!strcmp(calculated_pieces, pieces))
        res = 0;

    free(res);
    fclose(f);
    return res;
}

static int check_be_node(struct be_node *info)
{
    if (!info->element.dict[0])
        return 1;
    if (!strcmp("length", info->element.dict[0]->key->content))
        return check_single_file(info);
    if (!strcmp("files", info->element.dict[0]->key->content))
        return 1; //check_folder(info);
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
    if (tree->type == BE_DICT && tree->element.dict && tree->element.dict[0],
        tree->element.dict[1] && tree->element.dict[2] && tree->element.dict[3])
        res = check_be_node(tree->element.dict[3]->val);
    be_free(tree);
    return res; //error then
}