#ifndef CHIP_H
#define CHIP_H

#include <stdbool.h>
#include "vector.h"

typedef struct {
    int state; 
    int old_state;
    int size;
    const char* name;  
} Wire;

Wire* wire_init();

void free_wire(Wire* wire);

typedef struct Chip Chip;

typedef struct Chip {
	vector_t* inputs;
	vector_t* outputs;

    vector_t* input_labels;  
    vector_t* output_labels;

	vector_t* sub_chips; // internal chips
    vector_t* internal_wires; // ALL internal wires (for memory cleanup)
    
    // Mappings for the "Bridge" logic
    vector_t* input_buffers;  // Internal wires that map to External Inputs
    vector_t* output_buffers; // Internal wires that map to External Outputs

	const char* name;

	void (*evaluate)(Chip* self);
} Chip;

Chip* chip_init();

Chip* chip_init_composite(const char* name, int num_inputs, int num_outputs);
void chip_connect_input(Chip* parent, int parent_pin, Chip* sub_chip, int sub_pin);
void chip_connect_output(Chip* parent, int parent_pin, Chip* sub_chip, int sub_pin);

void chip_free_shallow(Chip* chip);

void chip_add_input(Chip* chip, Wire* wire);
void chip_set_input(Chip* chip, int index, Wire* wire);

void chip_add_output(Chip* chip, Wire* wire);
void chip_set_output(Chip* chip, const int index, Wire* wire);

void chip_label_input(Chip* chip, int index, const char* label);
void chip_label_output(Chip* chip, int index, const char* label);

void chip_set_evaluate(Chip* chip, void (*evaluate));

void chip_register_wire(Chip* chip, Wire* wire);
void chip_register_input_buffer(Chip* chip, Wire* wire);
void chip_register_output_buffer(Chip* chip, Wire* wire);

void chip_link(Chip* parent, Chip* source, Chip* target);
void chip_link_pins(Chip* parent, Chip* source, const int out_idx, Chip* target, const int in_idx);
void chip_add_sub_chip(Chip* parent, Chip* child);

Wire* chip_get_input(Chip* chip, const int index);

Wire* chip_get_output(Chip* chip, const int index);

void composite_evaluate(Chip* self);

inline void chip_to_image() {}

#endif