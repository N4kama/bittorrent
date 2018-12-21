#include "encode.h"

static char *my_basename(char *path)
{
    char *res = path;
    if (*path == '/')
        return res;
    while (*path)
    {
        if (*path == '/')
        {
            path++;
            if (*path != '\0')
                res = path;
        }
        else
            path++;
    }
    if (*(path - 1) == '/')
    {
        *(path - 1) = '\0';
    }
    return res;
}

static void get_files_from_dir(char *path, char ***files_path, int *arr_size)
{
    char new_path[4096];

    struct dirent *file = NULL;
    DIR *dir = opendir(path);

    if (!dir)
    {
        *arr_size += 1;
        *files_path = realloc(*files_path, *arr_size * sizeof(char *));
        (*files_path)[*arr_size - 1] = strdup(path);
    }
    else
    {
        while ((file = readdir(dir)))
        {
            if (strcmp(file->d_name, ".") && strcmp(file->d_name, ".."))
            {
                strcpy(new_path, path);
                strcat(new_path, "/");
                strcat(new_path, file->d_name);
                get_files_from_dir(new_path, files_path, arr_size);
            }
        }
    }
    closedir(dir);
}

static void init_basic_info(struct be_node *root)
{
    //Init the first 3 information of the torrent file
    root->element.dict[0]->key = calloc(1, sizeof(struct be_string));
    root->element.dict[0]->key->length = 7;
    root->element.dict[0]->key->content = strdup("announce");
    root->element.dict[0]->val = be_alloc(BE_STR);
    root->element.dict[0]->val->element.str =
        calloc(1, sizeof(struct be_string));
    root->element.dict[0]->val->element.str->length = 10;
    root->element.dict[0]->val->element.str->content = strdup("Naruto>DBZ");
    root->element.dict[1]->key = calloc(1, sizeof(struct be_string));
    root->element.dict[1]->key->length = 10;
    root->element.dict[1]->key->content = strdup("created by");
    root->element.dict[1]->val = be_alloc(BE_STR);
    root->element.dict[1]->val->element.str = calloc(1,
                                                     sizeof(struct be_string));
    root->element.dict[1]->val->element.str->length = 18;
    root->element.dict[1]->val->element.str->content =
        strdup("Des BGs dla street");
    root->element.dict[2]->key = calloc(1, sizeof(struct be_string));
    root->element.dict[2]->key->length = 13;
    root->element.dict[2]->key->content = strdup("creation date");
    root->element.dict[2]->val = be_alloc(BE_INT);
    root->element.dict[2]->val->element.num = 1998;
}

static int hash_single_file(char *path, struct be_node *pieces)
{
    int total_bytes_read = 0;
    int nb_hash = -1;
    unsigned char hash[SHA_DIGEST_LENGTH];
    unsigned char buf[262144];
    int res_size = 0;
    unsigned char *res = NULL;

    //Calculating the pieces of the file, result in res ptr
    FILE *f = fopen(path, "r");
    if (!f)
        return -1;
    int r = 0;
    while ((r = fread(buf, sizeof(char), 262144, f)) > 0)
    {
        total_bytes_read += r;
        res_size += SHA_DIGEST_LENGTH;
        res = realloc(res, res_size * sizeof(char));
        SHA1(buf, r, hash);
        nb_hash++;
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
            res[SHA_DIGEST_LENGTH * nb_hash + i] = hash[i];
    }
    fclose(f);
    if (!r)
    {
        res_size += SHA_DIGEST_LENGTH;
        res = realloc(res, res_size * sizeof(char));
        SHA1(buf, r, hash);
        nb_hash++;
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
            res[SHA_DIGEST_LENGTH * nb_hash + i] = hash[i];
    }

    pieces->element.str = calloc(1, sizeof(struct be_string));
    pieces->element.str->length = (nb_hash + 1) * SHA_DIGEST_LENGTH;
    void *ress = res;
    char *resss = ress;
    pieces->element.str->content = resss;
    return total_bytes_read;
}

static void init_single_file_torrent(struct be_node *root, char *path)
{
    root->element.dict[3]->key = calloc(1, sizeof(struct be_string));
    root->element.dict[3]->key->length = 6;
    root->element.dict[3]->key->content = strdup("pieces");
    root->element.dict[3]->val = be_alloc(BE_STR);

    //init pieces from a single file, get the total bytes read for the length
    //dictionnary
    int total_bytes_read = hash_single_file(path, root->element.dict[3]->val);

    //Getting basename from the base
    char *basename = my_basename(path);

    root->element.dict[0]->key = calloc(1, sizeof(struct be_string));
    root->element.dict[0]->key->length = 6;
    root->element.dict[0]->key->content = strdup("length");
    root->element.dict[0]->val = be_alloc(BE_INT);
    root->element.dict[0]->val->element.num = total_bytes_read;

    root->element.dict[1]->key = calloc(1, sizeof(struct be_string));
    root->element.dict[1]->key->length = 4;
    root->element.dict[1]->key->content = strdup("name");
    root->element.dict[1]->val = be_alloc(BE_STR);
    root->element.dict[1]->val->element.str = calloc(1,
                                                     sizeof(struct be_string));
    root->element.dict[1]->val->element.str->length = strlen(basename);
    root->element.dict[1]->val->element.str->content = strdup(basename);
}

static void init_basic_files_dict(struct be_node *dict, char *path,
                                  char *origin_path)
{
    dict->element.dict = calloc(3, sizeof(struct be_dict *));
    dict->element.dict[0] = calloc(1, sizeof(struct be_dict));
    dict->element.dict[1] = calloc(1, sizeof(struct be_dict));
    dict->element.dict[0]->key = calloc(1, sizeof(struct be_string));
    dict->element.dict[0]->val = be_alloc(BE_INT);
    dict->element.dict[0]->key->length = 6;
    dict->element.dict[0]->key->content = strdup("length");
    dict->element.dict[1]->key = calloc(1, sizeof(struct be_string));
    dict->element.dict[1]->val = be_alloc(BE_LIST);
    dict->element.dict[1]->key->length = 4;
    dict->element.dict[1]->key->content = strdup("path");

    //filling the 'path' array
    struct be_node *list = dict->element.dict[1]->val;
    int idx_entries = -1;

    int idx = 0;
    char p_copy[4096];
    char o_copy[4096];
    strcpy(p_copy, path);
    strcpy(o_copy, origin_path);
    while (p_copy[idx] && o_copy[idx] && p_copy[idx] == o_copy[idx])
        idx++;
    if (p_copy[idx] == '/')
        idx++;
    while (p_copy[idx])
    {
        int tmp = idx;
        while (p_copy[tmp] && p_copy[tmp] != '/')
            tmp++;

        idx_entries++;
        list->element.list = realloc(list->element.list,
                                (idx_entries + 2) * sizeof(struct be_node *));
        list->element.list[idx_entries + 1] = NULL;
        list->element.list[idx_entries] = be_alloc(BE_STR);
        list->element.list[idx_entries]->element.str =
            calloc(1, sizeof(struct be_string));
        list->element.list[idx_entries]->element.str->length = tmp - idx;
        list->element.list[idx_entries]->element.str->content =
            strndup(p_copy + idx, tmp - idx);

        if (p_copy[tmp])
            idx = tmp + 1;
        else
            idx = tmp;
    }
}

static void hash_multiple_files(char *path, struct be_node *dict,
                                struct be_node *pieces, int last_elem)
{
    //last elem = 0 if it's the last path

    int total_bytes_read = 0;
    static unsigned char buf[262144];
    static int idx = 0;
    unsigned char hash[SHA_DIGEST_LENGTH];
    static int res_size = 0;
    static char *res = 0;
    int nb_hash = -1;

    //Calculating file size, pieces values
    FILE *f = fopen(path, "r");
    if (!f)
        return;
    int r = 0;
    while ((r = fread(buf + idx, sizeof(char), 262144 - idx, f)) > 0)
    {
        total_bytes_read += r;
        idx += r;
        if (last_elem && idx != 262144)
            break;
        if (!last_elem)
            buf[idx] = '\0';
        idx = 0;
        res_size += SHA_DIGEST_LENGTH;
        res = realloc(res, res_size * sizeof(char));
        SHA1(buf, r, hash);
        nb_hash++;
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
            res[SHA_DIGEST_LENGTH * nb_hash + i] = hash[i];
    }
    fclose(f);
    if (!r && !last_elem)
    {
        buf[idx] = '\0';
        idx = 0;
        res_size += SHA_DIGEST_LENGTH;
        res = realloc(res, res_size * sizeof(char));
        SHA1(buf, r, hash);
        nb_hash++;
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
            res[SHA_DIGEST_LENGTH * nb_hash + i] = hash[i];
    }

    //total bytes read = length
    dict->element.dict[0]->val->element.num = total_bytes_read;

    pieces->element.str->length = (nb_hash + 1) * SHA_DIGEST_LENGTH;
    void *ress = res;
    char *resss = ress;
    pieces->element.str->content = resss;
}

static void init_directory_torrent(struct be_node *root, char *path)
{
    //Init the pieces part
    root->element.dict[3]->key = calloc(1, sizeof(struct be_string));
    root->element.dict[3]->key->length = 6;
    root->element.dict[3]->key->content = strdup("pieces");
    root->element.dict[3]->val = be_alloc(BE_STR);
    root->element.dict[3]->val->element.str = calloc(1,
                                                     sizeof(struct be_string));

    //Getting the list of files paths to fill 'files' dictionnary
    char **files_path = NULL;
    int arr_size = 0;
    get_files_from_dir(path, &files_path, &arr_size);

    //Init the files path
    root->element.dict[0]->key = calloc(1, sizeof(struct be_string));
    root->element.dict[0]->key->length = 5;
    root->element.dict[0]->key->content = strdup("files");
    root->element.dict[0]->val = be_alloc(BE_LIST);
    root->element.dict[0]->val->element.list = calloc(arr_size + 1,
                                                      sizeof(struct be_node *));

    //Filling the 'files' and 'pieces'
    for (int i = 0; i < arr_size; i++)
    {
        root->element.dict[0]->val->element.list[i] = be_alloc(BE_DICT);
        init_basic_files_dict(root->element.dict[0]->val->element.list[i],
                              files_path[i], path);
        hash_multiple_files(files_path[i],
                            root->element.dict[0]->val->element.list[i],
                            root->element.dict[3]->val, arr_size - i - 1);
    }

    char *dirname = my_basename(path); //ex : foo/bar/foobar_dir
    root->element.dict[1]->key = calloc(1, sizeof(struct be_string));
    root->element.dict[1]->key->length = 4;
    root->element.dict[1]->key->content = strdup("name");
    root->element.dict[1]->val = be_alloc(BE_STR);
    root->element.dict[1]->val->element.str = calloc(1,
                                                     sizeof(struct be_string));
    root->element.dict[1]->val->element.str->length = strlen(dirname);
    root->element.dict[1]->val->element.str->content = strdup(dirname);

    //free files_path
    for (int i = 0; i < arr_size; i++)
        free(files_path[i]);
    free(files_path);
}

static void init_torrent_info(char *path, struct be_node *root)
{
    /*  Two types of dictionnary : length, name, pieces_length, pieces
                  Or               files , name, pieces_length, pieces */

    //allocation of the 'info' be_node with the 4 elements above
    root->element.dict = calloc(5, sizeof(struct be_dict *));
    for (int i = 0; i < 4; i++)
        root->element.dict[i] = calloc(1, sizeof(struct be_dict));

    //Get general info about path (Dir or File)
    struct stat st;
    stat(path, &st);

    if (!S_ISDIR(st.st_mode))
    {
        //If path is a single file (first type of dict)
        init_single_file_torrent(root, path);
    }
    else
    {
        //If path is a Directory, iterate through it (second type of dict)
        init_directory_torrent(root, path);
    }

    //Init piece length : ALWAYS 262144
    root->element.dict[2]->key = calloc(1, sizeof(struct be_string));
    root->element.dict[2]->key->length = 12;
    root->element.dict[2]->key->content = strdup("piece length");
    root->element.dict[2]->val = be_alloc(BE_INT);
    root->element.dict[2]->val->element.num = 262144;
}

static struct be_node *file_to_be_node(char *path)
{
    //allocation of the base be_node with the 4 basics
    struct be_node *root = be_alloc(BE_DICT);
    root->element.dict = calloc(5, sizeof(struct be_dict *)); //5 because NULL
    for (int i = 0; i < 4; i++)                               //terminated
        root->element.dict[i] = calloc(1, sizeof(struct be_dict));

    //Initialization of the first 3 parameters which are always the same
    init_basic_info(root);

    //Initialization of the fourth parameter which direclty depends on the file
    root->element.dict[3]->key = calloc(1, sizeof(struct be_string));
    root->element.dict[3]->key->length = 4;
    root->element.dict[3]->key->content = strdup("info");
    root->element.dict[3]->val = be_alloc(BE_DICT);
    init_torrent_info(path, root->element.dict[3]->val);

    return root;
}

int encode_torrent(char *path)
{
    //Read through path and transforms it into be_node structure
    struct be_node *out = file_to_be_node(path);

    //transforms the be_node into a string BENCODED
    size_t buf_size = 0;
    char *buf = be_encode(out, &buf_size);

    //Writes the BENCODED string into a <path>.torrent file
    char torrent_name[4046] = {0};
    strcpy(torrent_name, path);
    strcat(torrent_name, ".torrent");
    FILE *f = fopen(torrent_name, "w");
    fwrite(buf, sizeof(char), buf_size, f);

    //free the be_node, close the file
    be_free(out);
    free(buf);
    fclose(f);

    return 0;
}