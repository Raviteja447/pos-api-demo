#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) return 0;
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

void parse_title(const char *json) {
    const char *title_ptr = strstr(json, "\"title\":");
    if (title_ptr) {
        title_ptr += 9;
        while (*title_ptr == ' ' || *title_ptr == '\"') title_ptr++;
        char title[256] = {0};
        int i = 0;
        while (*title_ptr && *title_ptr != '\"' && i < 255) {
            title[i++] = *title_ptr++;
        }
        title[i] = '\0';
        printf("Title: %s\n", title);
    } else {
        printf("Title not found in response.\n");
    }
}

void get_request() {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk = { malloc(1), 0 };

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://jsonplaceholder.typicode.com/posts/1");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        res = curl_easy_perform(curl);
        if(res == CURLE_OK) {
            parse_title(chunk.memory);
        } else {
            printf("GET request failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
        free(chunk.memory);
    }
}

void post_request() {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk = { malloc(1), 0 };
    long http_code = 0;
    const char *json_data = "{\"title\":\"POS Test\",\"body\":\"This is a test post\",\"userId\":1}";

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://jsonplaceholder.typicode.com/posts");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(NULL, "Content-Type: application/json"));
        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if(res == CURLE_OK) {
            printf("HTTP Response Code: %ld\n", http_code);
            printf("Response Body: %s\n", chunk.memory);
        } else {
            printf("POST request failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
        free(chunk.memory);
    }
}

int main() {
    printf("GET Request:\n");
    get_request();
    printf("\nPOST Request:\n");
    post_request();
    return 0;
}