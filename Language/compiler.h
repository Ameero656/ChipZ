#ifndef COMPILER_H
#define COMPILER_H

#include "vector.h"

typedef struct Memory {
    vector_t* keys;
    vector_t* values;
} Memory;

vector_t* parse_chipZ_code(const char* file_name);

void compiler_print_raw_tokens(vector_t* tokens);

void run(vector_t* tokens);

float evaluate_expression(vector_t* tokens);

#endif