#include <stdlib.h>


#include "primitives.h"
#include "vector.h" 
#include "simulator.h"
// --- LOGIC GATES ---



static void evaluate_not(Chip* self) {
    Wire* in = (Wire*) vector_get(self->inputs, 0);
    Wire* out = (Wire*) vector_get(self->outputs, 0);

    out->state = (in->state) ? 0 : 1;
}

static void evaluate_and(Chip* self) {
    Wire* a = (Wire*) vector_get(self->inputs, 0);
    Wire* b = (Wire*) vector_get(self->inputs, 1);
    Wire* out = (Wire*) vector_get(self->outputs, 0);

    out->state = (a->state && b->state) ? 1 : 0;
}


// --- FACTORY FUNCTIONS ---

Chip* chip_create_and() {
    Chip* c = chip_init("AND"); 
    c->evaluate = evaluate_and;
    return c;
}

Chip* chip_create_not() {
    Chip* c = chip_init("NOT");
    c->evaluate = evaluate_not;
    return c;
}
