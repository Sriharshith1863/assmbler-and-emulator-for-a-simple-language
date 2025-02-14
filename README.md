Two-Pass Assembler and Emulator
Project Overview
This project implements a Two-Pass Assembler and Emulator for a custom assembly language. The assembler converts assembly code into machine code while performing label resolution, error handling, and operand validation. The emulator then executes the generated machine code, simulating the behavior of a real processor.

Features
Assembler
Implements a two-pass assembly process to handle forward references and symbol resolution.
Generates three output files:
Machine Code File (machineCode.txt) – The compiled machine code in hexadecimal format.
Listing File (listingFile.lst) – A human-readable file mapping source lines to machine code.
Error File (error.txt) – Reports syntax, semantic, and operand-related errors.
Supports hexadecimal, octal, and decimal operands, validating operand ranges dynamically.
Handles label resolution and branch offset calculation for relative addressing.

Emulator
Reads the machine code file and executes the instructions in a simulated memory environment.
Supports fundamental operations including arithmetic, branching, stack manipulation, and memory access.
Implements error handling for unknown opcodes, memory overflows, and invalid instructions.
