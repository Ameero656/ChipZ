#ifndef GRAPH_H
#define GRAPH_H

#include "simulator.h"
#include "chip.h"

void chip_export_graphviz(Chip* chip, const char* filename, int depth);

#endif