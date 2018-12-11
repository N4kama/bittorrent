#include "includes.h"

static enum opt_type get_opt_enum(char c, struct options opt)
{
    return c;
}

struct options fill_options(int argc, char **argv, struct option *options)
{
    static struct options opt =
        {
            0};
    int argv_index = 1;
    int option_index = 0;
    char c = '?';
    opt.type = '?';
    while ((c = getopt_long(argc, argv, "p:m:c:vd", options, &option_index)) != -1)
    {
        //argv_index += complete_options(c, opt);
        if (opt.v > 1
            || opt.d > 1
            || c == '?'
            || !strchr("pmc?", opt.type))
        {
            //option not recognized, should throw an error ?
            break;
        }
        opt.nb_argv++;
        switch (c)
        {
        case 'p':
            opt.type = 'p';
            opt.argv = argv[++argv_index];
            break;
        case 'm':
            opt.type = 'm';
            opt.argv = argv[++argv_index];
            break;
        case 'c':
            opt.type = 'c';
            opt.argv = argv[++argv_index];
            break;
        case 'v':
            opt.v += 1;
            break;
        case 'd':
            opt.d += 1;
            break;
        default:
            opt.nb_argv--;
            break;
        }
        argv_index++;
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

int check_options(int argc, struct options options)
{
    //offset is the number of options, argc - offset = nb of torrent file as arg
    int offset = options.nb_options + options.nb_argv;
    if (argc == offset + 1)
    {
        return 0;
    }
    //conflict_options is the number of unique options (always leq than 1)
    int conflict_options = options.nb_options - options.d - options.v;
    if (conflict_options > 1)
    {
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[])
{
    struct options options = get_options(argc, argv);
    if (!check_options(argc, options))
    {
        errx(1, "my-bittorrent: Usage: ./my-bittorrent [options] [files]");
    }
    return 0;
}