#ifndef STRUCT_H
# define STRUCT_H

# include <stdio.h>
# include <unistd.h>
# include <getopt.h>

struct options
{
    int p;
    char *argv_p;
    int m;
    char *argv_m;
    int c;
    char *argv_c;
    int d;
    int v;
};

#endif