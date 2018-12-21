#include "includes.h"

static struct options parse_options(struct options opt, int *argv_index, char **argv, char c)
{
    switch (c)
    {
    case 'p':
        opt.type = opt.type == '?' ? 'p' : 'E';
        *argv_index += 1;
        opt.argv = argv[*argv_index];
        break;
    case 'm':
        opt.type = opt.type == '?' ? 'm' : 'E';
        *argv_index += 1;
        opt.argv = argv[*argv_index];
        break;
    case 'c':
        opt.type = opt.type == '?' ? 'c' : 'E';
        *argv_index += 1;
        opt.argv = argv[*argv_index];
        break;
    case 'v':
        opt.v += 1;
        break;
    case 'd':
        opt.d += 1;
        *argv_index += 1;
        opt.argv = argv[*argv_index];
        break;
    default:
        errx(1, "Usage: ./my-bittorrent [options] [files]");
    }
    return opt;
}
struct options fill_options(int argc, char **argv, struct option *options)
{
    static struct options opt =
        {
            0,
            0,
            '?',
            NULL,
            0,
            0,
        };
    int argv_index = 1;
    int option_index = 0;
    char c = ' ';
    while ((c = getopt_long(argc, argv, "p:m:c:vd", options, &option_index)) != -1)
    {
        if (opt.v > 1 || opt.d > 1 || c == '?' || opt.type == 'E')
        {
            errx(1, "Usage: ./my-bittorrent [options] [files]");
        }
        opt.nb_argv++;
        opt = parse_options(opt, &argv_index, argv, c);
        argv_index++;
    }
    if (!opt.nb_argv || opt.v > 1 || opt.d > 1 || c == '?' || opt.type == 'E' || argv_index < argc)
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
            {"dump-peers", 1, NULL, 'd'},
            {"verbose", 0, NULL, 'v'},
        };
    //filling options struct with corresponding values
    struct options opt = fill_options(argc, argv, options);
    return opt;
}

int exec_option(struct options opt)
{
    json_t *root = NULL;
    json_t *temp_info = NULL;
    int res = 0;
    switch (opt.type)
    {
    case 'p':
        root = decode_torrent(opt.argv);
        res = pretty_print(root);
        free_json(root);
        return res;
    case 'm':
        return encode_torrent(opt.argv);
    case 'c':
        return check_integrity(opt.argv);
        break;
    default:
        break;
    }
    if (opt.d)
    {
        root = decode_torrent(opt.argv);
        temp_info = json_object_get(root, "announce");
        if (!json_is_string(temp_info))
        {
            free_json(root);
            return 1;
        }
        const char *s = json_string_value(temp_info);
        res = dump_peers(s, opt.argv);
        free_json(root);
        return res;
    }
    return 1;
}
int main(int argc, char *argv[])
{
    struct options options = get_options(argc, argv);
    int res = exec_option(options);
    printf("%d\n", res);
    return 0;
}