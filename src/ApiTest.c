#include "../inc/global.h"
#include "../inc/ApiTest.h"
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

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

static void DisplayOnPOS(const char* msg) {
    ScrCls_Api();
    ScrDisp_Api(LINE2, 0, msg, CDISP);
    WaitAnyKey_Api(5);
}

static void parse_title_and_display(const char *json) {
    const char *title_ptr = strstr(json, "\"title\":");
    if (title_ptr) {
        title_ptr += 9;
        while (*title_ptr == ' ' || *title_ptr == '\"') title_ptr++;
        char title[64] = {0};
        int i = 0;
        while (*title_ptr && *title_ptr != '\"' && i < 63) {
            title[i++] = *title_ptr++;
        }
        title[i] = '\0';
        DisplayOnPOS(title);
    } else {
        DisplayOnPOS("Title not found");
    }
}

static void pos_get_request() {
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
            parse_title_and_display(chunk.memory);
        } else {
            DisplayOnPOS("GET failed");
        }
        curl_easy_cleanup(curl);
        free(chunk.memory);
    }
}

static void pos_post_request() {
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
        char buf[128];
        if(res == CURLE_OK) {
            snprintf(buf, sizeof(buf), "HTTP Code: %ld", http_code);
            DisplayOnPOS(buf);
            DisplayOnPOS(chunk.memory);
        } else {
            DisplayOnPOS("POST failed");
        }
        curl_easy_cleanup(curl);
        free(chunk.memory);
    }
}

void RunApiGetTest(void) {
    DisplayOnPOS("GET Request...");
    pos_get_request();
}

void RunApiPostTest(void) {
    DisplayOnPOS("POST Request...");
    pos_post_request();
}