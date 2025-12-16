#include "decoder.h"
#include "primitives.h"
#include "gates.h"

// 1x2 Decoder with Enable
// Inputs: 0: A (Select), 1: E (Enable)
// Outputs: 0: Y0, 1: Y1
Chip* chip_create_decoder_1x2() {
    Chip* chip = chip_init_composite("Dec1x2", 2, 2);
    
    Chip* not = chip_create_not();
    Chip* and0 = chip_create_and();
    Chip* and1 = chip_create_and();

    chip_add_sub_chip(chip, not);
    chip_add_sub_chip(chip, and0);
    chip_add_sub_chip(chip, and1);

    // Logic for Y0 = (NOT A) AND E
    chip_connect_input(chip, 0, not, 0);       // A -> NOT
    chip_link_pins(chip, not, 0, and0, 0);     // NOT A -> AND0
    chip_connect_input(chip, 1, and0, 1);      // E -> AND0
    chip_connect_output(chip, 0, and0, 0);     // AND0 -> Y0

    // Logic for Y1 = A AND E
    chip_connect_input(chip, 0, and1, 0);      // A -> AND1
    chip_connect_input(chip, 1, and1, 1);      // E -> AND1
    chip_connect_output(chip, 1, and1, 0);     // AND1 -> Y1

    return chip;
}

// 2x4 Decoder
// Inputs: 0: A0, 1: A1, 2: E
// Outputs: Y0 - Y3
Chip* chip_create_decoder_2x4() {
    Chip* chip = chip_init_composite("Dec2x4", 3, 4);

    Chip* splitter = chip_create_decoder_1x2(); // Uses A1 to split Enable
    Chip* low      = chip_create_decoder_1x2();
    Chip* high     = chip_create_decoder_1x2();

    chip_add_sub_chip(chip, splitter);
    chip_add_sub_chip(chip, low);
    chip_add_sub_chip(chip, high);

    // Setup Splitter (MSB A1 controls which half is active)
    chip_connect_input(chip, 1, splitter, 0);   // A1 -> Splitter Select
    chip_connect_input(chip, 2, splitter, 1);   // E  -> Splitter Enable

    // Setup Low Decoder (Active when A1=0)
    chip_connect_input(chip, 0, low, 0);        // A0 -> Low Select
    chip_link_pins(chip, splitter, 0, low, 1);  // Splitter Y0 -> Low Enable

    // Setup High Decoder (Active when A1=1)
    chip_connect_input(chip, 0, high, 0);       // A0 -> High Select
    chip_link_pins(chip, splitter, 1, high, 1); // Splitter Y1 -> High Enable

    // Outputs
    chip_connect_output(chip, 0, low, 0);
    chip_connect_output(chip, 1, low, 1);
    chip_connect_output(chip, 2, high, 0);
    chip_connect_output(chip, 3, high, 1);

    return chip;
}

// 3x8 Decoder
// Inputs: 0: A0, 1: A1, 2: A2, 3: E
// Outputs: Y0 - Y7
Chip* chip_create_decoder_3x8() {
    Chip* chip = chip_init_composite("Dec3x8", 4, 8);

    Chip* splitter = chip_create_decoder_1x2(); 
    Chip* low      = chip_create_decoder_2x4();
    Chip* high     = chip_create_decoder_2x4();

    chip_add_sub_chip(chip, splitter);
    chip_add_sub_chip(chip, low);
    chip_add_sub_chip(chip, high);

    // Splitter (MSB A2)
    chip_connect_input(chip, 2, splitter, 0);   // A2 -> Select
    chip_connect_input(chip, 3, splitter, 1);   // E -> Enable

    // Low Decoder (A0, A1)
    chip_connect_input(chip, 0, low, 0);        // A0
    chip_connect_input(chip, 1, low, 1);        // A1
    chip_link_pins(chip, splitter, 0, low, 2);  // Enable from Splitter Y0

    // High Decoder (A0, A1)
    chip_connect_input(chip, 0, high, 0);       // A0
    chip_connect_input(chip, 1, high, 1);       // A1
    chip_link_pins(chip, splitter, 1, high, 2); // Enable from Splitter Y1

    // Outputs
    for (int i = 0; i < 4; i++) {
        chip_connect_output(chip, i, low, i);
        chip_connect_output(chip, i + 4, high, i);
    }

    return chip;
}

// 4x16 Decoder
// Inputs: 0: A0, 1: A1, 2: A2, 3: A3, 4: E
// Outputs: Y0 - Y15
Chip* chip_create_decoder_4x16() {
    Chip* chip = chip_init_composite("Dec4x16", 5, 16);

    Chip* splitter = chip_create_decoder_1x2();
    Chip* low      = chip_create_decoder_3x8();
    Chip* high     = chip_create_decoder_3x8();

    chip_add_sub_chip(chip, splitter);
    chip_add_sub_chip(chip, low);
    chip_add_sub_chip(chip, high);

    // Splitter (MSB A3)
    chip_connect_input(chip, 3, splitter, 0);   // A3 -> Select
    chip_connect_input(chip, 4, splitter, 1);   // E -> Enable

    // Low Decoder (A0-A2)
    for (int i = 0; i < 3; i++) {
        chip_connect_input(chip, i, low, i);
    }
    chip_link_pins(chip, splitter, 0, low, 3);  // Enable

    // High Decoder (A0-A2)
    for (int i = 0; i < 3; i++) {
        chip_connect_input(chip, i, high, i);
    }
    chip_link_pins(chip, splitter, 1, high, 3); // Enable

    // Outputs
    for (int i = 0; i < 8; i++) {
        chip_connect_output(chip, i, low, i);
        chip_connect_output(chip, i + 8, high, i);
    }

    return chip;
}