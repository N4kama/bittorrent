#include "encode.h"

int check_be_node(struct be_node *info)
{
    return 0;
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
    if (tree->type == BE_DICT && tree->element.dict && tree->element.dict[0],
        tree->element.dict[1] && tree->element.dict[2] && tree->element.dict[3])
        return check_be_node(tree->element.dict[3]);
    return 1; //error then
}