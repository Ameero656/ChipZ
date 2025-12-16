#include <stdlib.h>
#include <stdio.h>
#include "simulator.h"

Simulator* sim_init() {
    Simulator* sim = malloc(sizeof(Simulator));
    if (!sim) return NULL;
    
    sim->all_chips = vector_init();
    sim->all_wires = vector_init();
    return sim;
}

void sim_free(Simulator* sim) {
    if (!sim) return;

    printf("DEBUG: Simulator cleaning up...\n");

    for (int i = 0; i < sim->all_wires->size; i++) {
        Wire* w = (Wire*) vector_get(sim->all_wires, i);
        free(w);
    }
    vector_free_shallow(sim->all_wires);

    for (int i = 0; i < sim->all_chips->size; i++) {
        Chip* c = (Chip*) vector_get(sim->all_chips, i);
        
        chip_free_shallow(c); 
    }
    vector_free_shallow(sim->all_chips);

    free(sim);
    printf("DEBUG: Cleanup complete.\n");
}

Wire* sim_create_wire(Simulator* sim, const char* name) {
    Wire* w = malloc(sizeof(Wire));
    w->state = 0;
    w->name = name;

    vector_push_back(sim->all_wires, w);
    return w;
}

void sim_register_chip(Simulator* sim, Chip* chip) {
    if (chip) {
        vector_push_back(sim->all_chips, chip);
    }
}

