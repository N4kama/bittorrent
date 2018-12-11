#include "decode.h"

static void print_json(struct be_node *tree)
{
    json_t *root = json_object();

    char *s = json_dumps(root, 0); //use JSON_INDENT for pretty print

    

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