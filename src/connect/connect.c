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

int dump_peers(const char *path, const char *f_path)
{
    //get the size of the file in order to create the right fitted buffer
    struct stat st;
    stat(f_path, &st);
    char *file = calloc(st.st_size, sizeof(char));

    //printf("DEBUG : size of file buffer is : %d (should be equal to char number)\n", (int)st.st_size);

    //Open the file and fill the buffer
    FILE *f = fopen(f_path, "r");
    if (!f)
    {
        free(file);
        return 0;
    }
    if (fread(file, sizeof(char), st.st_size, f) <= 0)
    {
        //Error while trying to read file
        free(file);
        fclose(f);
        return 0;
    }
    fclose(f);

    //Decode the buffer and parses it into a be_node tree
    struct be_node *tree = be_decode(file, st.st_size);
    free(file);
    if (!tree)
    {
        return 0;
    }
    char *val;
    int i = 0;
    struct be_dict *dict = tree->element.dict[i++];
    while (dict && !val)
    {
        size_t re = 0;
        char *temp = dict->key->content;
        if (!strcmp(temp, "info"))
            val = be_encode(dict->val, &re);
        free(temp);
        dict = tree->element.dict[i++];
    }
    if (!val)
        return 0;
    int res = 0;
    char *n_path = calloc(4096, 1);
    unsigned char *buff = calloc(4096, 1);
    unsigned char *valux = calloc(4096, 1);
    valux = memcpy(valux, val, strlen(val));
    n_path = strcpy(n_path, path);
    char *info_hash = calloc(4096, 1);
    int r = 20;
    buff = SHA1(valux, r, buff); //"%f0%7e%0b%05%84%74%5b%7b%cb%35%e9%80%97%48%8d%34%e6%86%23%d0"; //sha1(bencode(metadata['info']))";
    info_hash = memcpy(info_hash, buff, r);


    CURL *curl;
    struct curl_slist *host = NULL;
    path = path;
    curl = curl_easy_init();
    if (curl)
    {
        printf("---%s\n", curl_easy_escape(curl, info_hash, strlen(info_hash)));
        n_path = strcat(n_path, "?info_hash=");
        n_path = strcat(n_path, curl_easy_escape(curl, info_hash, strlen(info_hash)));
        n_path = strcat(n_path,
                        "&peer_id=%2D%41%5A%35%37%31%30%2D%54%70%6B%58%74%74%5A%4C%66%70%53%48&port=6881&uploaded=0&downloaded=0&left=1502576640&event=started&compact=1");
        curl_easy_setopt(curl, CURLOPT_RESOLVE, host);
        curl_easy_setopt(curl, CURLOPT_URL, n_path);
        res = curl_easy_perform(curl);

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    free(n_path);
    free(tree);
    curl_slist_free_all(host);
    return res;
}
