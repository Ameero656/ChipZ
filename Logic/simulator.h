#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "vector.h"
#include "chip.h"

typedef struct {
    vector_t* all_chips;
    vector_t* all_wires;
} Simulator;

Simulator* sim_init();
void sim_free(Simulator* sim);

Wire* sim_create_wire(Simulator* sim, const char* name);
void sim_register_chip(Simulator* sim, Chip* chip);

#endif