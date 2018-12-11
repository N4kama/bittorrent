#include "includes.h"

struct options fill_options(int argc, char **argv, struct option *options)
{
    static struct options opt =
    {
        0
    };
    int argv_index = 1;
    int option_index = 0;
    char c = ' ';
    while ((c = getopt_long(argc, argv, "p:m:c:vd", options, &option_index)) != -1)
    {
        if (c == '?')
        {
            //option not recognized, should throw an error ?
            break;
        }
        switch (c)
            {
            case 'p':
                opt.p = 1;
                argv_index++;
                opt.argv_p = argv[argv_index];
                break;
            default:
                break;
            }
    }
    return opt;
}

struct options get_options(int argc, char **argv)
{
    //init options arguments
    static struct option options[] =
        {
            {"pretty-print-torrent-file", 1, NULL, 'p'},
            {"mktorrent", 1, NULL, 'm'},
            {"check-integrity", 1, NULL, 'c'},
            {"dump-peers", 0, NULL, 'd'},
            {"verbose", 0, NULL, 'v'},
        };
    //filling options struct with corresponding values
    struct options opt = fill_options(argc, argv, options);
    return opt;
}

//warnx("my-bittorrent: Usage: ./my-bittorrent [options] [files]"")
int main(int argc, char *argv[])
{
    struct options options = get_options(argc, argv);
    if (options.p)
    {
        printf("ok\n");
        printf("%s\n", options.argv_p);
    }
    else
    {
        printf("KO\n");
    }
    return 0;
}