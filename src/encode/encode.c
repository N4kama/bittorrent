#include "encode.h"

static void init_basic_info(struct be_node *root)
{
    //Init the first 3 information of the torrent file
    root->element.dict[0]->key = calloc(1, sizeof(struct be_string));
    root->element.dict[0]->key->length = 7;
    root->element.dict[0]->key->content = "annonce";
    root->element.dict[0]->val = be_alloc(BE_STR);
    //A SURPPRIMER : PAS SUR QUE CA ALLOC LE PTR DE STRING DONC JLE FAIT
    root->element.dict[0]->val->element.str = calloc(1, sizeof(struct be_string));
    root->element.dict[0]->val->element.str->length = 10;
    root->element.dict[0]->val->element.str->content = "Naruto>DBZ";
    root->element.dict[1]->key = calloc(1, sizeof(struct be_string));
    root->element.dict[1]->key->length = 10;
    root->element.dict[1]->key->content = "created by";
    root->element.dict[1]->val = be_alloc(BE_STR);
    //A SURPPRIMER : PAS SUR QUE CA ALLOC LE PTR DE STRING DONC JLE FAIT
    root->element.dict[1]->val->element.str = calloc(1, sizeof(struct be_string));
    root->element.dict[1]->val->element.str->length = 18;
    root->element.dict[1]->val->element.str->content = "Des BGs dla street";
    root->element.dict[2]->key = calloc(1, sizeof(struct be_string));
    root->element.dict[2]->key->length = 13;
    root->element.dict[2]->key->content = "creation date";
    root->element.dict[2]->val = be_alloc(BE_INT);
    //A SURPPRIMER : PAS SUR QUE CA ALLOC LE PTR DE STRING DONC JLE FAIT
    root->element.dict[2]->val->element.num = 1998;
}

static void init_torrent_info(char *path, struct be_node *root)
{
    path = path;
    root = root;
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
    root->element.dict[3]->key->content = "info";
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
    char torrent_name[4046] = { 0 };
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