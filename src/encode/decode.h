#ifndef DECODE_H
# define DECODE_H

#include "../struct.h"
#include <sys/stat.h>
#include <jansson.h>
#include <string.h>
#include "../../include/bencode/bencode.h"

json_t *decode_torrent(char *file_path);
void free_json(json_t *root);
int pretty_print(json_t *j);

#endif /* !INCLUDE_H */