#include "connect.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

int dump_peers(const char *path)
{
    int res = 0;
    char *n_path = calloc(4096, 1);
    n_path = strcpy(n_path, path);
    char *info_hash = "sha1(bencode(metadata['info']))";
    n_path = strcat(n_path, "?info_hash=");
    n_path = strcat(n_path, info_hash);
    n_path = strcat(n_path,
                    "&peer_id=%2D%41%5A%35%37%35%30%2D%54%70%6B%58%74%74\
                    %5A%4C%66%70%53%48&port=6881&uploaded=0&downloaded=0\
                    &left=1502576640&event=started&compact=1");
    CURL *curl;
    struct curl_slist *host = NULL;
    path = path;
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_RESOLVE, host);
        curl_easy_setopt(curl, CURLOPT_URL, n_path);
        res = curl_easy_perform(curl);

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    free(n_path);
    curl_slist_free_all(host);
    return res;
}
