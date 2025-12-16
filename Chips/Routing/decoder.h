#ifndef DECODER_H
#define DECODER_H

#include "chip.h"

Chip* chip_create_decoder_1x2();
Chip* chip_create_decoder_2x4();
Chip* chip_create_decoder_3x8();
Chip* chip_create_decoder_4x16();

#endif

