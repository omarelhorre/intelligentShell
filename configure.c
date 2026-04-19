#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "chatgpt.h"
#include <string.h>
char *read_file(const char* fileName)
{
    FILE* file = fopen(fileName,"r");
    if(!file) return NULL;

    fseek(file,0,SEEK_END);
    long length = ftell(file);
    rewind(file);

    char* buffer = malloc(length + 1);
    if(!buffer)
    {
        fclose(file);
        return NULL;
    }

    fread(buffer,1,length,file);
    buffer[length] = '\0';
    fclose(file);
    return buffer;

}
int main(int argc, char* argv[])
{
    const char* API_KEY = getenv("OPENAI_API_KEY");
    if(!API_KEY)
    {
        fprintf(stderr, "Your environement variable is not set.\n");
        fprintf(stderr,"Please set it before running this program\n");
        return -1;
    }
    if(argc < 4)
    {
        fprintf(stderr, "Excpected %s <input_config> <output_config> <modification_description\n>""exited with an error\n", argv[0]);
        return -1;
    }

    char* file_content = read_file(argv[1]);
    if(!file_content)
    {
        fprintf(stderr,"Error: couldn't read file %s\n",argv[1]);
        return -1;
    }

    size_t prompt_size = strlen(argv[1]) + strlen(file_content) + strlen(argv[3]) + 2048;
    char* prompt = malloc(prompt_size);
    if(!prompt) 
    { 
        fprintf(stderr, "error building prompt\n");
        return -1;
    }
    snprintf(prompt,prompt_size, "You are helping to modify a configuration file.\n\n"
        "The current filename is: %s\n\n "
        "The filename is provided in case the name and/or extension"
        " of the filename is helpful for undertanding the content."
        "Here is the current content of the file:\n\n"
        "-------\n%s\n ------\n\n"
        "The user wants to make this modification : \n %s \n\n"
        "Please think hard about how to make this modification "
        "in the most sensible, logical and readable way. "
        "ONLY make this requested modification"
        ", DO NOT make any other modifications to the file."
        "Then output two section: \n"
        "1.Explanation: a brief explanation of what changes were made,"
        "including what was changed, inserted and/or deleted and why."
        "DO NOT comment or explain about any lines or content that "
        "was left the same as before.\n"
        "2.New file content: only the complete new vesrion of the file,"
        "without extra commentary.\n"
        "Format:\n"
        "Explanation:\n <text>\n\n"
        "New file content:\n <full file>\n",argv[1],file_content,argv[3],argv[2]);
        free(file_content);
        char* response = chatgpt_query(API_KEY, prompt);
        free(prompt);
        if(!response)
        {
            fprintf(stderr, "Status -1, could not get answer from OPENAI\n");
            return -1;
        }

        //injection gotta fix it later
        char* new_file_start = strstr(response,"New file content:");
        if(!new_file_start)
        {
            fprintf(stderr, "Status -1, usupported format\n %s", response);
            free(response);
            return -1;
        }

        size_t explation_len = new_file_start - response;
        char* explanation = malloc(explation_len + 1);
        if(!explanation)
        {
            fprintf(stderr,"Status -1 Error allocating space\n");
            free(response);
            return -1;
        }

        strncpy(explanation,response,explation_len);
        explanation[explation_len] = '\0';
        char *new_file_content = new_file_start + strlen("New file content:");
        while(*new_file_content == '\n' || *new_file_content == ' ')
        {
            new_file_content++;
        }


        printf("%s",explanation);
    return 0;
}