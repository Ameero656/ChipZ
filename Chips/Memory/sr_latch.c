#include "sr_latch.h"
#include "primitives.h"
#include "gates.h"

Chip* chip_create_sr_latch() {
    Chip* chip = chip_init_composite("SR_Latch", 2, 2);
    
    Chip* or1 = chip_create_or();
    Chip* not1 = chip_create_not();
    
    Chip* or2 = chip_create_or();
    Chip* not2 = chip_create_not();

    chip_add_sub_chip(chip, or1);
    chip_add_sub_chip(chip, not1);
    chip_add_sub_chip(chip, or2);
    chip_add_sub_chip(chip, not2);

    chip_link_pins(chip, or1, 0, not1, 0);
    chip_link_pins(chip, or2, 0, not2, 0);

    chip_connect_input(chip, 1, or1, 0);
    chip_link_pins(chip, not2, 0, or1, 1);
    
    chip_connect_input(chip, 0, or2, 0);
    chip_link_pins(chip, not1, 0, or2, 1);

    chip_connect_output(chip, 0, not1, 0);
    chip_connect_output(chip, 1, not2, 0);

    return chip;
}