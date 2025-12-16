#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "vector.h"


static int is_primitive(Chip* chip) {
    return (chip->input_buffers->size == 0 && chip->sub_chips->size == 0);
}


// Renders the chip as a single "Black Box" (Depth 0)
static void render_blackbox(FILE* f, Chip* chip) {
    // 1. Draw Inputs
    fprintf(f, "  { rank=source; ");
    for (int i = 0; i < chip->input_buffers->size; i++) {
        const char* label = (chip->input_labels && i < chip->input_labels->size) 
                            ? (char*)vector_get(chip->input_labels, i) : "In";
        fprintf(f, "\"in_%d\" [label=\"%s\", shape=circle, style=filled, fillcolor=lightgreen]; ", i, label);
    }
    fprintf(f, "}\n");

    // 2. Draw Outputs
    fprintf(f, "  { rank=sink; ");
    for (int i = 0; i < chip->output_buffers->size; i++) {
        const char* label = (chip->output_labels && i < chip->output_labels->size) 
                            ? (char*)vector_get(chip->output_labels, i) : "Out";
        fprintf(f, "\"out_%d\" [label=\"%s\", shape=doublecircle, style=filled, fillcolor=lightcoral]; ", i, label);
    }
    fprintf(f, "}\n");

    // 3. Draw The Main Chip Node
    fprintf(f, "  \"root_chip\" [label=\"%s\", shape=box3d, style=filled, fillcolor=lightgray, height=2, width=3];\n", chip->name);

    // 4. Connect Inputs -> Chip
    for (int i = 0; i < chip->input_buffers->size; i++) {
        fprintf(f, "  \"in_%d\" -> \"root_chip\";\n", i);
    }

    // 5. Connect Chip -> Outputs
    for (int i = 0; i < chip->output_buffers->size; i++) {
        fprintf(f, "  \"root_chip\" -> \"out_%d\";\n", i);
    }
}

// Recursive function to render internals (Depth >= 1)
static void render_chip(FILE* f, Chip* chip, int current_depth, int max_depth, const char* prefix) {
    if (!chip) return;

    // 1. Internal Ports
    fprintf(f, "  { rank=source; ");
    for (int i = 0; i < chip->input_buffers->size; i++) {
        const char* label = (chip->input_labels && i < chip->input_labels->size) 
                            ? (char*)vector_get(chip->input_labels, i) : "In";
        fprintf(f, "\"%sin_%d\" [label=\"%s\", shape=circle, style=filled, fillcolor=lightgreen, fontsize=10]; ", prefix, i, label);
    }
    fprintf(f, "}\n");

    fprintf(f, "  { rank=sink; ");
    for (int i = 0; i < chip->output_buffers->size; i++) {
        const char* label = (chip->output_labels && i < chip->output_labels->size) 
                            ? (char*)vector_get(chip->output_labels, i) : "Out";
        fprintf(f, "\"%sout_%d\" [label=\"%s\", shape=doublecircle, style=filled, fillcolor=lightcoral, fontsize=10]; ", prefix, i, label);
    }
    fprintf(f, "}\n");

    // 2. Sub-Components
    for (int i = 0; i < chip->sub_chips->size; i++) {
        Chip* sub = (Chip*) vector_get(chip->sub_chips, i);
        
        // Expand if we haven't hit the visual limit
        int should_expand = (current_depth < max_depth) && !is_primitive(sub);

        if (should_expand) {
            char sub_prefix[256];
            snprintf(sub_prefix, sizeof(sub_prefix), "%sg%d_", prefix, i); 

            fprintf(f, "  subgraph \"cluster_%sg%d\" {\n", prefix, i);
            fprintf(f, "    label=\"%s\"; style=dashed;\n", sub->name);
            render_chip(f, sub, current_depth + 1, max_depth, sub_prefix);
            fprintf(f, "  }\n");
        } else {
            fprintf(f, "  \"%sgate_%d\" [label=\"%s\", shape=box, style=filled, fillcolor=lightblue];\n", prefix, i, sub->name);
        }
    }

    // 3. Wires
    #define GET_TARGET_ID(gate, gate_idx, input_idx, buffer) \
        if ((current_depth < max_depth) && !is_primitive(gate)) { \
            sprintf(buffer, "\"%sg%d_in_%d\"", prefix, gate_idx, input_idx); \
        } else { \
            sprintf(buffer, "\"%sgate_%d\"", prefix, gate_idx); \
        }

    // Main Inputs -> Gates/Outputs
    for (int i = 0; i < chip->input_buffers->size; i++) {
        Wire* src_wire = (Wire*) vector_get(chip->input_buffers, i);
        
        for (int g = 0; g < chip->sub_chips->size; g++) {
            Chip* gate = (Chip*) vector_get(chip->sub_chips, g);
            for (int k = 0; k < gate->inputs->size; k++) {
                if (chip_get_input(gate, k) == src_wire) {
                    char target_id[256];
                    GET_TARGET_ID(gate, g, k, target_id);
                    fprintf(f, "  \"%sin_%d\" -> %s;\n", prefix, i, target_id);
                }
            }
        }
        for (int out = 0; out < chip->output_buffers->size; out++) {
            if ((Wire*)vector_get(chip->output_buffers, out) == src_wire) {
                fprintf(f, "  \"%sin_%d\" -> \"%sout_%d\";\n", prefix, i, prefix, out);
            }
        }
    }

    // Gate Outputs -> Gates/Outputs
    for (int g_src = 0; g_src < chip->sub_chips->size; g_src++) {
        Chip* src_gate = (Chip*) vector_get(chip->sub_chips, g_src);
        int src_expanded = (current_depth < max_depth) && !is_primitive(src_gate);

        for (int out_idx = 0; out_idx < src_gate->outputs->size; out_idx++) {
            Wire* wire = chip_get_output(src_gate, out_idx);
            if (!wire) continue;

            char src_node[256];
            if (src_expanded) snprintf(src_node, sizeof(src_node), "\"%sg%d_out_%d\"", prefix, g_src, out_idx);
            else snprintf(src_node, sizeof(src_node), "\"%sgate_%d\"", prefix, g_src);

            for (int g_dest = 0; g_dest < chip->sub_chips->size; g_dest++) {
                if (g_src == g_dest) continue;
                Chip* dest_gate = (Chip*) vector_get(chip->sub_chips, g_dest);
                for (int in_k = 0; in_k < dest_gate->inputs->size; in_k++) {
                    if (chip_get_input(dest_gate, in_k) == wire) {
                        char target_id[256];
                        GET_TARGET_ID(dest_gate, g_dest, in_k, target_id);
                        fprintf(f, "  %s -> %s;\n", src_node, target_id);
                    }
                }
            }
            for (int out = 0; out < chip->output_buffers->size; out++) {
                if ((Wire*)vector_get(chip->output_buffers, out) == wire) {
                     fprintf(f, "  %s -> \"%sout_%d\";\n", src_node, prefix, out);
                }
            }
        }
    }
}

// --- Main Export Function ---

void chip_export_graphviz(Chip* chip, const char* filename, int depth) {
    if (!chip) return;
    FILE* f = fopen(filename, "w");
    if (!f) return;

    fprintf(f, "digraph LogicCircuit {\n");
    fprintf(f, "  rankdir=LR;\n"); 
    fprintf(f, "  compound=true;\n"); 
    fprintf(f, "  node [fontname=\"Helvetica\"];\n");
    fprintf(f, "  edge [fontname=\"Helvetica\", fontsize=10];\n");

    if (depth == 0) {
        // Depth 0: Black Box Mode (Just inputs/outputs and the chip shell)
        render_blackbox(f, chip);
    } else {
        // Depth 1+: Internals. 
        // We pass 'depth - 1' as the recursion limit.
        // User Depth 1 -> Max Recursion 0 (Show contents, but don't expand them) -> Looks like your image.
        render_chip(f, chip, 0, depth - 1, "");
    }

    fprintf(f, "}\n");
    fclose(f);
    printf("Graph exported to %s (Depth: %d)\n", filename, depth);
}