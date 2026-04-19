#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <curl/curl.h>
#include "string.h"
#include "chatgpt.h"
char* chatgpt_query(const char* API_KEY, const char* prompt)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "model", "gpt-5");

    cJSON *messages = cJSON_CreateArray();

    cJSON *msg = cJSON_CreateObject();
    cJSON_AddStringToObject(msg, "role", "user");
    cJSON_AddStringToObject(msg, "content", prompt);

    cJSON_AddItemToArray(messages, msg);

    cJSON_AddItemToObject(root, "messages", messages);

    char* json_data = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if(!json_data) return NULL;

    CURL* curl;
    CURLcode res;
    //initialize curl
    curl_global_init(CURL_GLOBAL_ALL); 
    curl = curl_easy_init();
    if(!curl)
    {
        free(json_data);
        return NULL;
        
    }
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/models"); //send to this endpoint
    res = curl_easy_perform(curl); //perform the action and returns the result
    curl_global_cleanup();
    free(json_data);
    return NULL;
}