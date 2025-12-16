#include <stdio.h>
#include <stdlib.h>
#include <math.h> 

#include "table.h"
#include "chip.h"

TruthTable* table_generate(Chip* chip) {
    if (!chip) return NULL;

    int num_in = chip->inputs->size;
    int num_out = chip->outputs->size;

    int rows = 1 << num_in; 
    int cols = num_in + num_out;

    TruthTable* tt = malloc(sizeof(TruthTable));
    tt->rows = rows;
    tt->cols = cols;
    tt->num_inputs = num_in;
    tt->num_outputs = num_out;

    tt->grid = malloc(sizeof(int*) * rows);
    for (int i = 0; i < rows; i++) {
        tt->grid[i] = malloc(sizeof(int) * cols);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < num_in; j++) {
            int bit = (i >> j) & 1;
            
            Wire* w = chip_get_input(chip, j);
            w->state = bit;
            
            tt->grid[i][j] = bit;
        }

        chip->evaluate(chip);

        for (int j = 0; j < num_out; j++) {
            Wire* w = chip_get_output(chip, j);

            tt->grid[i][num_in + j] = w->state;
        }
    }

    return tt;
}

void table_free(TruthTable* tt) {
    if (!tt) return;
    for (int i = 0; i < tt->rows; i++) {
        free(tt->grid[i]);
    }
    free(tt->grid);
    free(tt);
}

void table_print(TruthTable* tt, const vector_t* input_labels, const vector_t* output_labels) {
    if (!tt 
        || input_labels < 0 || input_labels->size > tt->num_inputs 
        || output_labels < 0 || output_labels->size > tt->num_inputs) return;

    printf("\n--- TRUTH TABLE ---\n");
    
    // Print Headers
    for (int i = tt->num_inputs - 1; i >= 0; i--) 
        printf("%s ", (char*)vector_get(input_labels, i));
    
    printf("| ");
    
    for (int i = tt->num_outputs - 1; i >= 0; i--)
        printf("%s ", vector_get(output_labels, i));
        
    printf("\n");

    // Print Divider
    for (int i=0; i < tt->cols + 1; i++) printf("---");
    printf("\n");

    // Print Rows
    for (int r = 0; r < tt->rows; r++) {
        for (int i = tt->num_inputs - 1; i >= 0; i--) {
            printf("%d  ", tt->grid[r][i]);
        }

        printf("| ");

        for (int i = tt->num_outputs - 1; i >= 0; i--) {
            printf("%d  ", tt->grid[r][tt->num_inputs + i]);
        }

        printf("\n");
    }

    printf("-------------------\n");
}

void table_export_csv(Chip* chip, const char* filename) {
    if (!chip) return;

    FILE* f = fopen(filename, "w");
    if (!f) {
        printf("Error: Could not open file %s for writing.\n", filename);
        return;
    }

    printf("Generating %s... (This may take a moment)\n", filename);

    int num_in = chip->inputs->size;
    int num_out = chip->outputs->size;
    
    // We use long long to ensure we can hold 2^17 (which fits in int, but good practice)
    long long rows = 1LL << num_in; 

    // 1. Write Header
    // Inputs
    for (int i=0; i<num_in; i++) fprintf(f, "In_%d,", i);
    // Outputs
    for (int i=0; i<num_out; i++) fprintf(f, "Out_%d,", i);
    fprintf(f, "\n");

    // 2. Loop Rows
    for (long long i = 0; i < rows; i++) {
        
        // A. Set Inputs
        for (int j = 0; j < num_in; j++) {
            int bit = (i >> j) & 1;

            Wire* w = chip_get_input(chip, j);
            if (w) w->state = bit;

            fprintf(f, "%d,", bit);
        }

        // B. Evaluate
        chip->evaluate(chip);

        // C. Write Outputs
        for (int j = 0; j < num_out; j++) {
            Wire* w = chip_get_output(chip, j);
            if (w) fprintf(f, "%d,", w->state);
        }
        fprintf(f, "\n");
        
        // Optional: Progress bar for huge tables
        if (i % 10000 == 0) printf("\rProgress: %.1f%%", (double)i/rows * 100.0);
    }
    
    printf("\rProgress: 100.0%%\n");
    printf("Successfully wrote %lld rows to %s\n", rows, filename);
    fclose(f);
}