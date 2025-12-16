#include "subtractor.h"
#include "primitives.h"
#include "gates.h"

Chip* chip_create_half_subtractor() {
    Chip* chip = chip_init_composite("HalfSub", 2, 2); 
    Chip* xor = chip_create_xor();
    Chip* not = chip_create_not();
    Chip* and = chip_create_and();

    chip_add_sub_chip(chip, xor);
    chip_add_sub_chip(chip, not);
    chip_add_sub_chip(chip, and);

    chip_connect_input(chip, 0, xor, 0);
    chip_connect_input(chip, 1, xor, 1);
    chip_connect_output(chip, 0, xor, 0);

    chip_connect_input(chip, 0, not, 0);
    chip_link_pins(chip, not, 0, and, 0);
    chip_connect_input(chip, 1, and, 1); 
    chip_connect_output(chip, 1, and, 0);

    return chip;
}

Chip* chip_create_full_subtractor() {
    Chip* chip = chip_init_composite("FullSub", 3, 2);

    Chip* hs1 = chip_create_half_subtractor();
    Chip* hs2 = chip_create_half_subtractor();
    Chip* or  = chip_create_or();

    chip_add_sub_chip(chip, hs1);
    chip_add_sub_chip(chip, hs2);
    chip_add_sub_chip(chip, or);

    chip_connect_input(chip, 0, hs1, 0);
    chip_connect_input(chip, 1, hs1, 1);

    chip_link_pins(chip, hs1, 0, hs2, 0);
    chip_connect_input(chip, 2, hs2, 1); 
    chip_connect_output(chip, 0, hs2, 0);

    chip_link_pins(chip, hs1, 1, or, 0);
    chip_link_pins(chip, hs2, 1, or, 1);
    chip_connect_output(chip, 1, or, 0);

    return chip;
}

Chip* chip_create_subtractor_2bit() {
    Chip* chip = chip_init_composite("Sub2Bit", 5, 3); 
    
    Chip* fs0 = chip_create_full_subtractor();
    Chip* fs1 = chip_create_full_subtractor();

    chip_add_sub_chip(chip, fs0);
    chip_add_sub_chip(chip, fs1);

    chip_connect_input(chip, 0, fs0, 0);
    chip_connect_input(chip, 1, fs0, 1);
    chip_connect_input(chip, 4, fs0, 2);
    chip_connect_output(chip, 0, fs0, 0); 

    chip_connect_input(chip, 2, fs1, 0);
    chip_connect_input(chip, 3, fs1, 1);
    chip_link_pins(chip, fs0, 1, fs1, 2); 
    
    chip_connect_output(chip, 1, fs1, 0); 
    chip_connect_output(chip, 2, fs1, 1);

    return chip;
}

Chip* chip_create_subtractor_4bit() {
    Chip* chip = chip_init_composite("Sub4Bit", 9, 5); 

    Chip* s2_low  = chip_create_subtractor_2bit();
    Chip* s2_high = chip_create_subtractor_2bit();

    chip_add_sub_chip(chip, s2_low);
    chip_add_sub_chip(chip, s2_high);

    chip_connect_input(chip, 0, s2_low, 0);
    chip_connect_input(chip, 1, s2_low, 1);
    chip_connect_input(chip, 2, s2_low, 2);
    chip_connect_input(chip, 3, s2_low, 3);
    chip_connect_input(chip, 8, s2_low, 4);

    chip_connect_output(chip, 0, s2_low, 0);
    chip_connect_output(chip, 1, s2_low, 1);

    chip_connect_input(chip, 4, s2_high, 0);
    chip_connect_input(chip, 5, s2_high, 1);
    chip_connect_input(chip, 6, s2_high, 2);
    chip_connect_input(chip, 7, s2_high, 3);
    chip_link_pins(chip, s2_low, 2, s2_high, 4);

    chip_connect_output(chip, 2, s2_high, 0);
    chip_connect_output(chip, 3, s2_high, 1);
    chip_connect_output(chip, 4, s2_high, 2);

    return chip;
}

Chip* chip_create_subtractor_8bit() {
    Chip* chip = chip_init_composite("Sub8Bit", 17, 9);

    Chip* s4_low  = chip_create_subtractor_4bit();
    Chip* s4_high = chip_create_subtractor_4bit();

    chip_add_sub_chip(chip, s4_low);
    chip_add_sub_chip(chip, s4_high);

    for (int i = 0; i < 8; i++) {
        chip_connect_input(chip, i, s4_low, i);
    }
    chip_connect_input(chip, 16, s4_low, 8);

    for (int i = 0; i < 4; i++) {
        chip_connect_output(chip, i, s4_low, i);
    }

    for (int i = 0; i < 8; i++) {
        chip_connect_input(chip, i + 8, s4_high, i);
    }
    chip_link_pins(chip, s4_low, 4, s4_high, 8);

    for (int i = 0; i < 4; i++) {
        chip_connect_output(chip, i + 4, s4_high, i);
    }
    chip_connect_output(chip, 8, s4_high, 4);

    return chip;
}

Chip* chip_create_subtractor_16bit() {
    Chip* chip = chip_init_composite("Sub16Bit", 33, 17);

    Chip* s8_low  = chip_create_subtractor_8bit();
    Chip* s8_high = chip_create_subtractor_8bit();

    chip_add_sub_chip(chip, s8_low);
    chip_add_sub_chip(chip, s8_high);

    for (int i = 0; i < 16; i++) {
        chip_connect_input(chip, i, s8_low, i);
    }
    chip_connect_input(chip, 32, s8_low, 16);

    for (int i = 0; i < 8; i++) {
        chip_connect_output(chip, i, s8_low, i);
    }

    for (int i = 0; i < 16; i++) {
        chip_connect_input(chip, i + 16, s8_high, i);
    }
    chip_link_pins(chip, s8_low, 8, s8_high, 16);

    for (int i = 0; i < 8; i++) {
        chip_connect_output(chip, i + 8, s8_high, i);
    }
    chip_connect_output(chip, 16, s8_high, 8);

    return chip;
}