#ifndef ADDER_H
#define ADDER_H

#include "chip.h"

// Inputs: A, B, Cin. Outputs: Sum, Cout
Chip* chip_create_half_adder();
Chip* chip_create_full_adder();


#endif