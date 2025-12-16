#include "gates.h"
#include "primitives.h" // For AND, NOT


Chip* chip_create_nand() {
    Chip* nand = chip_init_composite("NAND", 2, 1);
    
    Chip* and_chip = chip_create_and();
    Chip* not_chip = chip_create_not();

    chip_add_sub_chip(nand, and_chip);
    chip_add_sub_chip(nand, not_chip);

    chip_connect_input(nand, 0, and_chip, 0);
    chip_connect_input(nand, 1, and_chip, 1);

    chip_link_pins(nand, and_chip, 0, not_chip, 0);

    chip_connect_output(nand, 0, not_chip, 0);

    return nand;
}

// IMPL: A OR B = NOT( NOT(A) AND NOT(B) )
Chip* chip_create_or() {
    Chip* chip = chip_init_composite("OR", 2, 1);

    Chip* not_a = chip_create_not();
    Chip* not_b = chip_create_not();
    Chip* and   = chip_create_and();
    Chip* not_out = chip_create_not();

    chip_add_sub_chip(chip, not_a);
    chip_add_sub_chip(chip, not_b);
    chip_add_sub_chip(chip, and);
    chip_add_sub_chip(chip, not_out);

    chip_connect_input(chip, 0, not_a, 0); // In0 -> NOT A
    chip_connect_input(chip, 1, not_b, 0); // In1 -> NOT B

    // AND the inverted inputs
    chip_link_pins(chip, not_a, 0, and, 0); // NOT A -> AND[0]
    chip_link_pins(chip, not_b, 0, and, 1); // NOT B -> AND[1]

    // Invert the result
    chip_link_pins(chip, and, 0, not_out, 0); // AND Out -> NOT Out
    chip_connect_output(chip, 0, not_out, 0); // Final Output

    return chip;
}

// IMPL: A XOR B = (A OR B) AND (NOT (A AND B))
Chip* chip_create_xor() {
    Chip* chip = chip_init_composite("XOR", 2, 1);

    // 1. Create Internals
    Chip* or_gate  = chip_create_or();  // Uses our new composite OR
    Chip* and_gate = chip_create_and(); 
    Chip* not_gate = chip_create_not(); // To make the NAND part
    Chip* final_and = chip_create_and();

    chip_add_sub_chip(chip, or_gate);
    chip_add_sub_chip(chip, and_gate);
    chip_add_sub_chip(chip, not_gate);
    chip_add_sub_chip(chip, final_and);

    // 2. Wiring
    
    // Path 1: (A OR B)
    chip_connect_input(chip, 0, or_gate, 0);
    chip_connect_input(chip, 1, or_gate, 1);

    // Path 2: NOT (A AND B) aka NAND
    chip_connect_input(chip, 0, and_gate, 0);
    chip_connect_input(chip, 1, and_gate, 1);
    chip_link_pins(chip, and_gate, 0, not_gate, 0);

    // Path 3: Combine them (OR_out AND NAND_out)
    chip_link_pins(chip, or_gate, 0, final_and, 0);
    chip_link_pins(chip, not_gate, 0, final_and, 1);

    // Output
    chip_connect_output(chip, 0, final_and, 0);

    return chip;
}