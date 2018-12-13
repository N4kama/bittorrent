#ifndef DECODE_H
# define DECODE_H

#include "../struct.h"
#include <sys/stat.h>
#include <jansson.h>
#include <string.h>
#include "../../include/bencode/bencode.h"

int decode_torrent(char *file_path);

#endif /* !INCLUDE_H */