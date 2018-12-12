#include "decode.h"

static char *str_decode(struct be_string *str)
{
    char* s = calloc(str->length + 1, 1);
    s = memcpy(s, str->content, str->length);
    return s;
}
static json_t *create_json(json_t *root, struct be_node *tree)
{
    struct be_node *cur = NULL;
    struct be_dict *dict = NULL;
    json_t *child = NULL;
    switch (tree->type)
    {
    case BE_STR:
        return json_string(str_decode(tree->element.str));
    case BE_INT:
        return json_integer(tree->element.num);
    case BE_LIST:
        cur = *(tree->element.list + 0);
        while (cur)
        {
            child = json_object();
            json_array_append(root, create_json(child, cur));
            cur += 1;
        }
        break;
    case BE_DICT:
        dict = *(tree->element.dict + 0);
        while (dict)
        {
            child = json_object();
            json_object_set_new(root, str_decode(dict->key),
                                create_json(child, dict->val));
            dict += 1;
        }
    default:
        break;
    }
    return root;
}

static void print_json(struct be_node *tree)
{
    json_t *root = json_object();
    create_json(root, tree);
    char *s = json_dumps(root, 0);
    printf("%s\n", s);
    json_decref(root);
}

int decode_torrent(char *file_path)
{
    //get the size of the file in order to create the right fitted buffer
    struct stat st;
    stat(file_path, &st);
    char *file = calloc(st.st_size, sizeof(char));

    printf("DEBUG : size of file buffer is : %d (should be equal to char number)\n", (int)st.st_size);

    //Open the file and fill the buffer
    FILE *f = fopen(file_path, "r");
    if (fread(file, sizeof(char), st.st_size, f) <= 0)
    {
        //Error while trying to read file
    }

    //Decode the buffer and parses it into a be_node tree
    struct be_node *tree = be_decode(file, st.st_size);

    //JSON output
    print_json(tree);

    be_free(tree);
    return 0;
}