#ifndef INCLUDE_H
# define INCLUDE_H

# include <err.h>
# include <getopt.h>

# include "struct.h"
# include "encode/decode.h"

struct options
{
    int nb_options;
    int nb_argv;
    char type;
    char *argv;
    int d;
    int v;
};

#endif /* !INCLUDE_H */