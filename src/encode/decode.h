#ifndef DECODE_H
# define DECODE_H

#include "../struct.h"
#include <sys/stat.h>
#include <jansson.h>
#include <string.h>
#include "../../include/bencode/bencode.h"


struct info_metainfo
{
    int length;
    char *name;
    int piece_length;
    char* pieces;
};

struct metainfo
{
    char *announce;
    char *author;
    char *creat_auth;
    char *creat_date;
    struct info_metainfo *info;
};

int decode_torrent(char *file_path);

#endif /* !INCLUDE_H */