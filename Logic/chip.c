#include <string.h>
#include <stdio.h>

#include "chip.h"


Wire* wire_init() {
	Wire* wire = (Wire*) malloc(sizeof(Wire));
	if (!wire) return NULL;

    wire->state = 0;
    wire->old_state = 0;
    wire->size = 1;

	return wire;
}

void free_wire(Wire* wire) {
	if (wire) free(wire);
}


Chip* chip_init(const char* name) {

	Chip* chip = (Chip*) malloc(sizeof(Chip));
	if (!chip) goto cleanup;

	chip->inputs = vector_init();
	chip->outputs = vector_init();

	chip->input_labels = vector_init();
	chip->output_labels = vector_init();

	chip->sub_chips = vector_init();
	chip->internal_wires = vector_init();

	chip->input_buffers = vector_init();
	chip->output_buffers = vector_init();

	chip->name = name;

	chip->evaluate = composite_evaluate;

	return chip;

	cleanup:
		free(chip);
		return NULL;
}

Chip* chip_init_composite(const char* name, int num_inputs, int num_outputs) {	
	Chip* chip = chip_init(name);
    chip_set_evaluate(chip, composite_evaluate);

    for (int i = 0; i < num_inputs; i++) {
        Wire* w = wire_init();
        chip_register_wire(chip, w);       
        chip_register_input_buffer(chip, w);
        chip_set_input(chip, i, w);
        
        char buf[32];
        sprintf(buf, "I%d", i);
        chip_label_input(chip, i, strdup(buf));
    }

    for (int i = 0; i < num_outputs; i++) {
        Wire* w = wire_init();
        chip_register_wire(chip, w);         
        chip_register_output_buffer(chip, w);
        chip_set_output(chip, i, w);

        char buf[32];
        sprintf(buf, "O%d", i);
        chip_label_output(chip, i, strdup(buf));
    }

    return chip;
}

void chip_connect_input(Chip* parent, int parent_pin, Chip* sub_chip, int sub_pin) {
    Wire* buffer = (Wire*) vector_get(parent->input_buffers, parent_pin);
    chip_set_input(sub_chip, sub_pin, buffer);
}

void chip_connect_output(Chip* parent, int parent_pin, Chip* sub_chip, int sub_pin) {
    Wire* buffer = (Wire*) vector_get(parent->output_buffers, parent_pin);
    chip_set_output(sub_chip, sub_pin, buffer);
}

void chip_free_shallow(Chip* chip) {
    if (!chip) return;

    if (chip->inputs) vector_free_shallow(chip->inputs);
    if (chip->outputs) vector_free_shallow(chip->outputs);

    if (chip->input_labels) vector_free_shallow(chip->input_labels);
    if (chip->output_labels) vector_free_shallow(chip->output_labels);

    if (chip->input_buffers) vector_free_shallow(chip->input_buffers);
    if (chip->output_buffers) vector_free_shallow(chip->output_buffers);

    if (chip->sub_chips) vector_free_shallow(chip->sub_chips);
    if (chip->internal_wires) vector_free_shallow(chip->internal_wires);

    free(chip);
}

void chip_add_input(Chip* chip, Wire* wire) {
	if (!chip || !wire) return;
    vector_push_back(chip->inputs, wire);
}

void chip_add_output(Chip* chip, Wire* wire) {
	if (!chip || !wire) return;
    vector_push_back(chip->outputs, wire);
}

void chip_set_input(Chip* chip, const int index, Wire* wire) {
    if (!chip) return;
    while (chip->inputs->size <= index) {
        vector_push_back(chip->inputs, NULL);
    }

    vector_set(chip->inputs, index, wire);
}

void chip_set_output(Chip* chip, const int index, Wire* wire) {
    if (!chip) return;   
    while (chip->outputs->size <= index) {
        vector_push_back(chip->outputs, NULL);
    }

    vector_set(chip->outputs, index, wire);
}

void chip_label_input(Chip* chip, const int index, const char* label) {
    if (!chip) return;
    // Ensure vector is big enough
    while (chip->input_labels->size <= index) {
        vector_push_back(chip->input_labels, NULL);
    }
    vector_set(chip->input_labels, index, (void*)label);
}

void chip_label_output(Chip* chip, const int index, const char* label) {
    if (!chip) return;
    while (chip->output_labels->size <= index) {
        vector_push_back(chip->output_labels, NULL);
    }
    vector_set(chip->output_labels, index, (void*)label);
}

void chip_set_evaluate(Chip* chip, void (*evaluate)) {
	chip->evaluate = evaluate;
}

Wire* chip_get_input(Chip* chip, const int index) {
	return (Wire*) vector_get(chip->inputs, index);
}

Wire* chip_get_output(Chip* chip, const int index) {
	return (Wire*) vector_get(chip->outputs, index);
}

void chip_register_wire(Chip* chip, Wire* wire) {
	vector_push_back(chip->internal_wires, wire);
}

void chip_register_input_buffer(Chip* chip, Wire* wire) {
    if (!chip || !wire) return;
    vector_push_back(chip->input_buffers, wire);
}

void chip_register_output_buffer(Chip* chip, Wire* wire) {
    if (!chip || !wire) return;
    vector_push_back(chip->output_buffers, wire);
}

void chip_link(Chip* parent, Chip* source, Chip* target) {
    if (!parent || !source || !target) return;

    Wire* wire = wire_init();
    
    chip_register_wire(parent, wire);

    chip_add_output(source, wire);
    chip_add_input(target, wire);
}

void chip_link_pins(Chip* parent, Chip* source, int out_idx, Chip* target, int in_idx) {
    if (!parent || !source || !target) return;

    Wire* wire = wire_init();
    chip_register_wire(parent, wire); 

    chip_set_output(source, out_idx, wire);
    chip_set_input(target, in_idx, wire);
}

void chip_add_sub_chip(Chip* parent, Chip* child) {
    if (parent && child) 
        vector_push_back(parent->sub_chips, child);
}

void composite_evaluate(Chip* self) {
    for (int i = 0; i < self->inputs->size && i < self->input_buffers->size; i++) {
        Wire* ext = (Wire*) vector_get(self->inputs, i);
        Wire* buf = (Wire*) vector_get(self->input_buffers, i);
        buf->state = ext->state;
    }

    // 2. SETTLE LOOP: Run sub-chips until wires stop changing
    // We limit this to prevent infinite loops (e.g., if you accidentally built a clock/oscillator)
    const int MAX_ITERATIONS = 100; 
    bool stable = false;

    for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
        stable = true;

        // A. Snapshot: Record the state of all internal wires *before* this pass
        for (int i = 0; i < self->internal_wires->size; i++) {
            Wire* w = (Wire*) vector_get(self->internal_wires, i);
            w->old_state = w->state;
        }

        // B. Evaluate: Run all sub-chips once
        // (Order doesn't matter now, because we will loop if things are wrong)
        for (int i = 0; i < self->sub_chips->size; i++) {
            Chip* child = (Chip*) vector_get(self->sub_chips, i);
            if (child->evaluate) child->evaluate(child);
        }

        // C. Check Stability: Did any wire change?
        for (int i = 0; i < self->internal_wires->size; i++) {
            Wire* w = (Wire*) vector_get(self->internal_wires, i);
            if (w->state != w->old_state) {
                stable = false; // Something changed, we need another pass
                break; 
            }
        }

        // If nothing changed, the circuit has settled. Stop working.
        if (stable) break;
    }

    if (!stable) {
        printf("WARNING: Chip '%s' did not settle. Possible infinite loop/oscillation.\n", self->name);
    }

    // 3. OUTPUT PHASE: Push internal results to the outside world
    for (int i = 0; i < self->outputs->size && i < self->output_buffers->size; i++) {
        Wire* ext = (Wire*) vector_get(self->outputs, i);
        Wire* buf = (Wire*) vector_get(self->output_buffers, i);
        ext->state = buf->state;
    }
}