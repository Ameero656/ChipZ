#include <stdio.h>

#include "simulator.h"
#include "decoder.h" 
#include "table.h"
#include "graph.h"
#include "compiler.h"

int main() {
    Simulator* sim = sim_init();
    Chip* sub16 = chip_create_decoder_4x16();
    
    sim_register_chip(sim, sub16);
    chip_export_graphviz(sub16, "sub16", 4);

    TruthTable* tt = table_generate(sub16);

    table_print(tt, sub16->input_labels, sub16->output_labels);

    table_free(tt);
    sim_free(sim);
    return 0;
}