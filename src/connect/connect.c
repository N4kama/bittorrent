#include "connect.h"
#include "../encode/decode.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <openssl/sha.h>

static struct be_node *get_tree(const char *f_path)
{
    struct stat st;
    stat(f_path, &st);
    char *file = calloc(st.st_size, sizeof(char));
    FILE *f = fopen(f_path, "r");
    if (!f)
    {
        free(file);
        return NULL;
    }
    if (fread(file, sizeof(char), st.st_size, f) <= 0)
    {
        free(file);
        fclose(f);
        return NULL;
    }
    fclose(f);
    struct be_node *tree = be_decode(file, st.st_size);
    free(file);
    if (!tree)
    {
        return NULL;
    }
    return tree;
}

static char *find_val(struct be_node *tree)
{
    if (!tree)
    {
        return NULL;
    }
    char *val = NULL;
    size_t len = 0;
    int i = 0;
    struct be_dict *dict = tree->element.dict[i++];
    while (dict && !val)
    {
        if (!strcmp(dict->key->content, "info"))
            val = be_encode(dict->val, &len + 1);
        dict = tree->element.dict[i++];
    }
    if (!val)
    {
        return NULL;
    }
    return val;
}

int dump_peers(const char *path, const char *f_path)
{
    struct be_node *tree = get_tree(f_path);
    char *val = find_val(tree);
    if (tree)
    {
        be_free(tree);
    }
    if (!val)
    {
        return 0;
    }
    void *val_v = val;
    unsigned char *val_u = val_v;
    char *n_path = calloc(4096, 1);
    unsigned char *info_hash_u = calloc(4096, 1);
    info_hash_u = SHA1(val_u, strlen(val), info_hash_u); //"%f0%7e%0b%05%84%74%5b%7b%cb%35%e9%80%97%48%8d%34%e6%86%23%d0"; //sha1(bencode(metadata['info']))";
    void *info_hash_v = info_hash_u;
    char *info_hash = info_hash_v;
    info_hash = info_hash;
    FILE *buf = fopen("test", "w+");
    CURL *curl;
    curl = curl_easy_init();
    if (curl)
    {
        sprintf(n_path, "/announce?info_hash=%s&peer_id=-MB2021-&port=6881&uploaded=0&downloaded=100&left=0&compact=1", curl_easy_escape(curl, info_hash, strlen(info_hash)));
        curl_easy_setopt(curl, CURLOPT_URL, path);
        printf("%s\n", n_path);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
        curl_easy_setopt(curl, CURLOPT_REQUEST_TARGET, n_path);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);
        curl_easy_perform(curl);
        fclose(buf);
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    free(n_path);
    return 1;
}
