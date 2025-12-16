#include <string.h>

#include "adder.h"
#include "primitives.h" // For AND
#include "gates.h"      // For OR, XOR

Chip* chip_create_half_adder() {
    Chip* chip = chip_init_composite("HalfAdder", 2, 2); // In: A, B. Out: Sum, Carry
    Chip* xor = chip_create_xor();
    Chip* and = chip_create_and();
    
    chip_add_sub_chip(chip, xor);
    chip_add_sub_chip(chip, and);

    // Sum = A XOR B
    chip_connect_input(chip, 0, xor, 0);
    chip_connect_input(chip, 1, xor, 1);
    chip_connect_output(chip, 0, xor, 0);

    // Carry = A AND B
    chip_connect_input(chip, 0, and, 0);
    chip_connect_input(chip, 1, and, 1);
    chip_connect_output(chip, 1, and, 0);
    
    return chip;
}


// --- REQUESTED CHIPS ---

Chip* chip_create_full_adder() {
    Chip* chip = chip_init_composite("FullAdder", 3, 2); // In: A, B, Cin. Out: Sum, Cout
    
    Chip* ha1 = chip_create_half_adder();
    Chip* ha2 = chip_create_half_adder();
    Chip* or  = chip_create_or();

    chip_add_sub_chip(chip, ha1);
    chip_add_sub_chip(chip, ha2);
    chip_add_sub_chip(chip, or);

    // HA1(A, B) -> s1, c1
    chip_connect_input(chip, 0, ha1, 0);
    chip_connect_input(chip, 1, ha1, 1);

    // HA2(s1, Cin) -> Sum, c2
    chip_link_pins(chip, ha1, 0, ha2, 0);
    chip_connect_input(chip, 2, ha2, 1);
    
    // Final Sum is Sum of HA2
    chip_connect_output(chip, 0, ha2, 0);

    // Cout = c1 OR c2
    chip_link_pins(chip, ha1, 1, or, 0);
    chip_link_pins(chip, ha2, 1, or, 1);
    chip_connect_output(chip, 1, or, 0);

    return chip;
}

