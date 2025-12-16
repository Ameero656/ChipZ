#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"


vector_t* parse_chipZ_code(const char* file_name) {
    FILE* f = fopen(file_name, "r");
    if (!f) {
        printf("Error: Could not open file %s\n", file_name);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = (char*) malloc(length + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, length, f);
    buffer[length] = '\0'; // Null-terminate the string
    fclose(f);

    vector_t* tokens = vector_init();

    char* token = strtok(buffer, " \t\n\r");
    
    while (token != NULL) {
        vector_push_back(tokens, strdup(token));
        
        token = strtok(NULL, " \t\n\r");
    }

    free(buffer); 
    return tokens;
}

void compiler_print_raw_tokens(vector_t* tokens) {
    for (int i=0; i<tokens->size; ++i) 
        printf("%s\n", vector_get(tokens, i));
    
}