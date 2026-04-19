#include <stdio.h>
#include <stdlib.h>
#include "chatgpt.h"
int main()
{
    const char* API_KEY = getenv("OPENAI_API_KEY");
    if(!API_KEY)
    {
        fprintf(stderr, "Your environement variable is not set.\n");
        fprintf(stderr,"Please set it before running this program\n");
       // return -1;
    }
    const char* prompt = "what is the capital of france?";
    char* reply = chatgpt_query(API_KEY, prompt);

    if (reply)
    {
        printf("ChatGPT says: %s\n",reply);
        free(reply);
    }
    else
    {
        fprintf(stderr, "Failed to get a response, please try again later.\n");
    }
    return 0;
}