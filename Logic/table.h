#ifndef TABLE_H
#define TABLE_H

#include "chip.h"

typedef struct {
    int** grid;      
    int rows;        
    int cols;        
    int num_inputs;  
    int num_outputs;
} TruthTable;

TruthTable* table_generate(Chip* chip);
void table_free(TruthTable* table);
void table_print(TruthTable* table, const vector_t* input_labels, const vector_t* output_labels);

void table_export_csv(Chip* chip, const char* filename);

#endif