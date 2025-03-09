# Computer Architecture Project: Assembly to Machine Code Conversion

This repository contains a C++ program that converts **RISC-V Assembly Code** into **Machine Code**.

## Supported Instructions

The program performs conversions for the following instructions based on the **RISC-V 32-bit Architecture Reference Sheet**:

### **R-Type Instructions**  
`add`, `and`, `or`, `sll`, `slt`, `sra`, `srl`, `sub`, `xor`, `mul`, `div`, `rem`

### **I-Type Instructions**  
`addi`, `andi`, `ori`, `lb`, `ld`, `lh`, `lw`, `jalr`

### **S-Type Instructions**  
`sb`, `sw`, `sd`, `sh`

### **SB-Type (Branch) Instructions**  
`beq`, `bne`, `bge`, `blt`

### **U-Type Instructions**  
`auipc`, `lui`

### **UJ-Type (Jump) Instructions**  
`jal`

## Languages and References Used

- **Language:** C++
- **Reference:** RISC-V 32-bit Architecture Specification

## How to Compile and Run

1. Place the assembly instructions to be converted inside the **`input.asm`** file.
2. Compile the program using a C++ compiler:
   ```sh
   g++ -o assembler Assembly_to_Machine.cpp
3. The output will be stored in **`output.mc`** in the format: `<address of instruction> <space> <machine code> <space, comma> <assembly instruction> <space, #> <opcode-func3-func7-rd-rs1-rs2-immediate>` followed by the data segment
