#ifndef GATES_H
#define GATES_H

#include "chip.h"

Chip* chip_create_nand();
Chip* chip_create_or();
Chip* chip_create_xor();

#endif