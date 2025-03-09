
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
   g++ -o assembler assem2machine.cpp
3. The output will be stored in **`output.mc`** in the format: `<address of instruction> <space> <machine code> <space, comma> <assembly instruction> <space, #> <opcode-func3-func7-rd-rs1-rs2-immediate>` followed by the data segment

## Functions Used

**Note:**  
- **LW cannot take hex input** in the current implementation.  
- The project **does not display errors** for out-of-range immediate values.  

**Main Functions**
1. `DataSegment()` – Reads the **.data** section and stores it in memory.  
2. `Instructions_Manual()` – Reads the **instruction set** from the reference file.  
3. `WriteToFile()` – Converts instructions to **hexadecimal** and writes PC & machine code to `output.mc`.  
4. `TextSegment()` – Reads the **.text** section and stores the assembly instructions.  
5. `ExtractLabels()` – Extracts **labels** and their respective positions in the code.  
6. `sp_to_x2()` – Replaces all occurrences of **sp** with **x2** (since `sp` is an alias for register `x2`).  
7. `Extract_type()` – Identifies the **instruction type** and calls the respective function for machine code conversion.  
8. `printDataMemory()` – Prints the **data memory** in **Venus** format (without memory locations).  
9. `printDataMemory2()` – Prints the **data memory** in the **professor's format**.  

### **Type Processing Functions**
1. `I_Type()` – Processes **I-format** instructions (e.g., `addi`, `lw`, `jalr`).  
2. `R_Type()` – Processes **R-format** instructions (e.g., `add`, `sub`, `mul`).  
3. `S_Type()` – Processes **S-format** instructions (e.g., `sw`, `sb`).  
4. `SB_Type()` – Processes **SB-format** instructions (e.g., `beq`, `bne`).  
5. `U_Type()` – Processes **U-format** instructions (e.g., `lui`, `auipc`).  
6. `UJ_Type()` – Processes **UJ-format** instructions (e.g., `jal`).  

### **Base Conversion Functions**
In addition to instruction processing, the project includes functions to **convert between numerical bases** (decimal, binary, hexadecimal).  

