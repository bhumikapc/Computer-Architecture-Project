
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

## Functions used
Note: LW cannot take hex input here. Also, the project currently does not display errors for out of range immediates.

Main Functions:
1) DataSegment(); //Read Data segment and store it in memory (data)
2) Instructions_Manual();  //Read Instruction set
3) ofstream("output.mc").close();  // Clear the output file (creates and then closes it)
4) TextSegment(); //Read Text segment and store it
5) ExtractLabels(); //Extract all labels with their positions
6) sp_to_x2(); //To replace all sp with x2 (as sp is alias for register x2)
7) Extract_type(); //Extracts Instruction type and calls typenumber and respective type function for writing machine code
8) printDataMemory(); //Prints memory in Venus format (but no memory location)
9) printDataMemory2(); //Prints memory in sir's format

Type Processing Functions:
1) I-Type
2) R_Type
3) S_Type
4) SB_Type
5) U_Type
6) UJ_Type

Appart from these, there are functions to convert from one base to other.
Footer
