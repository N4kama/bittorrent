#include "includes.h"

static void check_options(struct options opt, int argc, char **argv, struct option *options)
{
    int option_index = 0;
    while ((c = getopt_long(argc, argv, "p:m:c:vd", options, &option_index)) != -1)
    {
        if (opt.v > 1 || opt.d > 1 || c == '?' || opt.type == 'E')
        {
            errx(1, "Usage: ./my-bittorrent [options] [files]");
        }
        opt.nb_argv++;
        switch (c)
        {
        case 'p':
            opt.type = opt.type == '?' ? 'p' : 'E';
            opt.argv = argv[++argv_index];
            break;
        case 'm':
            opt.type = opt.type == '?' ? 'm' : 'E';
            opt.argv = argv[++argv_index];
            break;
        case 'c':
            opt.type = opt.type == '?' ? 'c' : 'E';
            opt.argv = argv[++argv_index];
            break;
        case 'v':
            opt.v += 1;
            break;
        case 'd':
            opt.d += 1;
            break;
        default:
            errx(1, "Usage: ./my-bittorrent [options] [files]");
        }
        argv_index++;
    }
}
struct options fill_options(int argc, char **argv, struct option *options)
{
    static struct options opt =
        {
            0, 0, '?', NULL, 0, 0,
        };
    int argv_index = 1;
    char c = ' ';
    if (opt.v > 1 || opt.d > 1 || c == '?' || opt.type == 'E' || argv_index < argc)
    {
        errx(1, "Usage: ./my-bittorrent [options] [files]");
    }
    opt.nb_options = argv_index - 1;
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

int main(int argc, char *argv[])
{
    struct options options = get_options(argc, argv);
    printf("%c", options.type);
    return 0;
}