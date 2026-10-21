# ChipZ

A hierarchical digital logic simulator written in C. Only AND and NOT are coded as primitives; every other chip is built by wiring smaller chips together.

## Features

- **Composite chips:** NAND, OR, XOR, half/full adder, and 1x2 up to 4x16 decoders
- **Settle-based evaluation:** sub-chips re-run until all internal wires stop changing, so build order doesn't matter. Circuits that never settle are reported after 100 passes
- **Truth tables:** sweep every input combination of a chip, print it or export to CSV
- **Graphviz export:** draw any chip at a chosen depth, from a single black box down to gates

## Layout

```
Logic/      chip, simulator, primitives (AND, NOT), truth tables, Graphviz export
Chips/
  arithmetic/   gates (NAND, OR, XOR), half and full adder
  routing/      decoders
Language/   ChipZ language front end (tokenizer only, so far)
Util/       vector (dynamic array)
main.c      demo
```

## Build

```
gcc -I Logic -I Chips/arithmetic -I Chips/routing -I Language -I Util \
    main.c Logic/*.c Chips/*/*.c Language/*.c Util/*.c -o chipz
```

## Example

```c
Chip* fa = chip_create_full_adder();

TruthTable* tt = table_generate(fa);
table_print(tt, fa->input_labels, fa->output_labels);
table_free(tt);

chip_export_graphviz(fa, "full_adder.dot", 2);   // dot -Tpng full_adder.dot -o full_adder.png
```

## Status

Work in progress. To be implemented: custom ChipZ language/compiler.
