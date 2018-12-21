#include "decode.h"

static char *str_decode(struct be_string *be_str)
{
    char *s = be_str->content;
    size_t len = be_str->length;
    if (!s)
        return NULL;
    char *res = calloc(len * 5 + 1, sizeof(char));
    size_t cursor = 0;
    for (size_t i = 0; i < len; i++)
    {
        if (s[i] < 32 || s[i] > 126)
        {
            sprintf((res + cursor), "%s", "U+00");
            cursor += 4;
            unsigned char hexa = s[i];
            sprintf((res + cursor), "%02X", hexa);
            cursor += 2;
        }
        else
        {
            if (s[i] == '\\')
            {
                res[cursor] = '\\';
                cursor += 1;
            }
            res[cursor] = s[i];
            cursor += 1;
        }
    }
    return res;
}

static json_t *create_json(json_t *root, struct be_node *tree)
{
    int i = 0;
    struct be_node *cur = NULL;
    struct be_dict *dict = NULL;
    json_t *child = NULL;
    json_t *array = NULL;
    json_t *s = NULL;
    char *temp = NULL;
    switch (tree->type)
    {
    case BE_STR:
        json_decref(root);
        temp = str_decode(tree->element.str);
        s = json_string(temp);
        free(temp);
        return s;
    case BE_INT:
        json_decref(root);
        return json_integer(tree->element.num);
    case BE_LIST:
        json_decref(root);
        cur = tree->element.list[i++];
        array = json_array();
        while (cur)
        {
            child = json_object();
            json_array_append_new(array, create_json(child, cur));
            cur = tree->element.list[i++];
        }
        return array;
    case BE_DICT:
        dict = tree->element.dict[i++];
        while (dict)
        {
            child = json_object();
            temp = str_decode(dict->key);
            json_object_set_new(root, temp,
                                create_json(child, dict->val));
            free(temp);
            dict = tree->element.dict[i++];
        }
        return root;
    default:
        break;
    }
    return root;
}

void free_json(json_t *root)
{
    if (!root)
    {
        return;
    }
    const char *key;
    void *tmp;
    json_t *value;
    json_object_foreach_safe(root, tmp, key, value)
    {
        free_json(json_object_get(root, key));
    }
    json_decref(root);
}

int pretty_print(json_t *root)
{
    if (!root)
    {
        return 1;
    }
    char *s = json_dumps(root, JSON_INDENT(6));
    printf("%s\n", s);
    free(s);
    return 0;
}

json_t *decode_torrent(char *file_path)
{
    //get the size of the file in order to create the right fitted buffer
    if (!file_path)
        return NULL;
    struct stat st;
    stat(file_path, &st);
    char *file = calloc(st.st_size, sizeof(char));

    //printf("DEBUG : size of file buffer is : %d (should be equal to char number)\n", (int)st.st_size);

    //Open the file and fill the buffer
    FILE *f = fopen(file_path, "r");
    if(!f)
    {
        free(file);
        return NULL;
    }
    if (fread(file, sizeof(char), st.st_size, f) <= 0)
    {
        //Error while trying to read file
        free(file);
        fclose(f);
        return NULL;
    }
    fclose(f);

    //Decode the buffer and parses it into a be_node tree
    struct be_node *tree = be_decode(file, st.st_size);
    free(file);
    if (!tree)
    {
        return NULL;
    }

    //JSON output
    json_t *root = json_object();
    root = create_json(root, tree);
    be_free(tree);
    return root;
}