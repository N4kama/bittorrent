#include "encode.h"

static struct be_node *file_to_be_node(char *path)
{
}

int encode_torrent(char *path)
{
    //Read through path and transforms it into be_node structure
    struct be_node *out = file_to_be_node(path);

    //transforms the be_node into a string BENCODED
    size_t buf_size = 0;
    char *buf = be_encode(out, &buf_size);

    //Writes the BENCODED string into a <path>.torrent file
    char torrent_name[4046] = path;
    strcat(torrent_name, ".torrent");
    FILE *f = fopen(torrent_name, "w");
    fwrite(buf, sizeof(char), buf_size, f);

    //free the be_node, close the file
    be_free(out);
    free(buf);
    fclose(f);

    return 0;
}