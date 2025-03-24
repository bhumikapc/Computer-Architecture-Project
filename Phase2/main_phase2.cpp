#include<bits/stdc++.h>
#include <iostream>
#include <fstream>
using namespace std;

// Register file (32 registers)
static unsigned int R[32];

// Global PC (Program Counter)
static unsigned int PC = 0x0; // Initialize PC at address 0

// Flags (Unused in basic RISC-V but possibly for debugging)
// static int N, C, V, Z; 

// Memory (Size: 4000 bytes)
static unsigned char MEM[4000];
static unsigned char inst_MEM[4000];


// Intermediate datapath/control signals
static unsigned int instruction_word;  // Stores current instruction
static unsigned int operand1;          // Stores first operand for ALU
static unsigned int operand2;          // Stores second operand for ALU

//Registers to store intermediate stage values
int rz=0; 
int ry=0;
unsigned int PC_temp; //initialising temporary_PC

int read_word(unsigned char *mem, unsigned int address) 
{
    int *data;
    data = (int*) (mem + address);
    return *data;
}
    
void write_word(unsigned char *mem, unsigned int address, unsigned int data) 
{
    int *data_p;
    data_p = (int*) (mem + address);
    *data_p = data;
}

int hexadecimalToDecimal(string hexVal) 
{ 
    int len = hexVal.size(); 
  
    // Initializing base value to 1, 
    // i.e 16^0 
    int base = 1; 
  
    int dec_val = 0; 
  
    // Extracting characters as digits 
    // from last character 
    for (int i = len - 1; i >= 0; i--) { 
        // If character lies in '0'-'9', 
        // converting it to integral 0-9 
        // by subtracting 48 from ASCII value 
        if (hexVal[i] >= '0' && hexVal[i] <= '9') { 
            dec_val += (int(hexVal[i]) - 48) * base; 
  
            // incrementing base by power 
            base = base * 16; 
        } 
  
        // If character lies in 'A'-'F' , converting 
        // it to integral 10 - 15 by subtracting 55 
        // from ASCII value 
        else if (hexVal[i] >= 'A' && hexVal[i] <= 'F') { 
            dec_val += (int(hexVal[i]) - 55) * base; 
  
            // Incrementing base by power 
            base = base * 16; 
        } 
    } 
    return dec_val; 
} 

void fetch() 
{
    instruction_word = read_word(inst_MEM,PC);
    // cout << "Fetched instruction: 0x" << hex << instruction_word << ", PC: 0x" << hex << PC << endl;
    PC_temp=PC;
    PC=PC+4;
}


struct DecodedInstr {
    unsigned int opcode;
    unsigned int rd, rs1, rs2;
    unsigned int funct3, funct7;
    int imm;
    string inst_type;
};

DecodedInstr inst;
  
void decode() 
{
    inst.opcode=0x0;
    inst.funct3=0x0;
    inst.rd=0;
    inst.rs1=0;
    inst.rs2=0;
    inst.funct7=0x0;
    inst.imm=0;
    // Extract opcode (bits 0-6)
    inst.opcode = instruction_word & 0x7F;
    // cout<<inst.opcode<<endl;
    // cout<<instruction_word<<" "<<inst.opcode<<endl;
    if(inst.opcode == 0x23)  // **S-type (Store)**
    {
        inst.funct3 = (instruction_word >> 12) & 0x7;
        if(inst.funct3 == 0x3)
        {
            //sd is I type
            // cout<<"I"<<endl;
            inst.inst_type="I";
            inst.rd = (instruction_word >> 7) & 0x1F;
            inst.rs1 = (instruction_word >> 15) & 0x1F;
            // cout<<(instruction_word >> 20)<<endl;
            inst.imm = (instruction_word >> 20) & 0xFFF;
            if(inst.imm & 0x800)
            {
                inst.imm |= 0xFFFFF000;
            }
            // cout<<"Type- "<<inst.type<<endl;
            // cout<<"Func3- "<<inst.funct3<<endl;
            // cout<<"Destination Register- "<<inst.rd<<endl;
            // cout<<"Source Register- "<<inst.rs1<<endl;
            // cout<<"Immediate Register- "<<inst.imm<<endl;
        }
        else
        {
            // cout<<"S"<<endl;
            inst.inst_type="S";
            inst.rs1 = (instruction_word >> 15) & 0x1F;
            inst.rs2 = (instruction_word >> 20) & 0x1F;
            
            
            // Extract immediate
            int imm4_0 = (instruction_word >> 7) & 0x1F;  // bits 11-7
            int imm11_5 = (instruction_word >> 25) & 0x7F; // bits 31-25
            inst.imm = (imm11_5 << 5) | imm4_0;
            
            // Sign-extend 12-bit immediate
            if (inst.imm & 0x800) inst.imm |= 0xFFFFF000;
            // cout<<"Type- "<<inst.type<<endl;
            // cout<<"Func3- "<<inst.funct3<<endl;
            // cout<<"Source Register1- "<<inst.rs1<<endl;
            // cout<<"Source Register2- "<<inst.rs2<<endl;
            // cout<<"Immediate Register- "<<inst.imm<<endl;
        }
        
    }
    else if(inst.opcode == 0x6F)  // **UJ-type (JAL)**
    {
        inst.rd = (instruction_word >> 7) & 0x1F;
        // cout<<"J"<<endl;
        inst.inst_type="UJ";
        // Extract immediate fields
        // cout<<instruction_word<<endl;
        int imm20 = (instruction_word >> 31) & 0x1;
        int imm10_1 = (instruction_word >> 21) & 0x3FF;
        int imm11 = (instruction_word >> 20) & 0x1;
        int imm19_12 = (instruction_word >> 12) & 0xFF;
        // cout<<imm20<<imm10_1<<imm11<<imm19_12<<endl;
        inst.imm = (imm20 << 19) | (imm19_12 << 11) | (imm11 << 10) | (imm10_1);

        // Sign-extend 21-bit immediate
        if (inst.imm & 0x100000) inst.imm |= 0xFFE00000;
        inst.imm = inst.imm<<1; //for RISCV leave LSB=0
        // cout<<"Type- "<<inst.type<<endl;
        // cout<<"Destination Register- "<<inst.rd<<endl;
        // cout<<"Immediate Register- "<<inst.imm<<endl;
    }
    else if(inst.opcode == 0x63)  // **SB-type (Branch)**
    {
        // cout<<"SB"<<endl;
        inst.inst_type="SB";
        inst.rs1 = (instruction_word >> 15) & 0x1F;
        inst.rs2 = (instruction_word >> 20) & 0x1F;
        inst.funct3 = (instruction_word >> 12) & 0x7;

        // Extract immediate fields
        int imm11 = (instruction_word >> 7) & 0x1;
        int imm4_1 = (instruction_word >> 8) & 0xF;
        int imm10_5 = (instruction_word >> 25) & 0x3F;
        int imm12 = (instruction_word >> 31) & 0x1;
        inst.imm = (imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1);

        // Sign-extend 13-bit immediate
        if (inst.imm & 0x1000) inst.imm |= 0xFFFFE000;
        // cout<<"Type- "<<inst.type<<endl;
        // cout<<"Func3- "<<inst.funct3<<endl;
        // cout<<"Source Register1- "<<inst.rs1<<endl;
        // cout<<"Source Register2- "<<inst.rs2<<endl;
        // cout<<"Immediate Register- "<<inst.imm<<endl;
    }
    else if(inst.opcode == 0x33 || inst.opcode == 0x73 || inst.opcode == 0x01)
    {  // **R-type**
        cout<<"R"<<endl;
        inst.inst_type="R";
        inst.rd = (instruction_word >> 7) & 0x1F;
        inst.rs1 = (instruction_word >> 15) & 0x1F;
        inst.rs2 = (instruction_word >> 20) & 0x1F;
        inst.funct3 = (instruction_word >> 12) & 0x7;
        inst.funct7 = (instruction_word >> 25) & 0x7F;
        // cout<<"Type- "<<inst.type<<endl;
        // cout<<"Func3- "<<inst.funct3<<endl;
        // cout<<"Func7- "<<inst.funct7<<endl;
        // cout<<"Destination Register- "<<inst.rd<<endl;
        // cout<<"Source Register1- "<<inst.rs1<<endl;
        // cout<<"Source Register2- "<<inst.rs2<<endl;
    }
    else if(inst.opcode == 0x17 || inst.opcode == 0x37)  // **U-type (LUI, AUIPC)**
    {
        // cout<<"U"<<endl;
        inst.inst_type="U";
        inst.rd = (instruction_word >> 7) & 0x1F;
        inst.imm = instruction_word & 0xFFFFF000; // Upper 20 bits
        // cout<<"Type- "<<inst.type<<endl;
        // cout<<"Destination Register- "<<inst.rd<<endl;
        // cout<<"Immediate Register- "<<inst.imm<<endl;
        
    }
    else if(inst.opcode == 0x3 || inst.opcode == 0xF || inst.opcode == 0x13 || inst.opcode== 0x1B || inst.opcode== 0x67)  // **I-type**
    {
        // cout<<"I"<<endl;
        inst.inst_type="I";
        inst.rd = (instruction_word >> 7) & 0x1F;
        inst.rs1 = (instruction_word >> 15) & 0x1F;
        inst.funct3 = (instruction_word >> 12) & 0x7;
        // cout<<(instruction_word >> 20)<<endl;
        inst.imm = (instruction_word >> 20) & 0xFFF;
        if(inst.imm & 0x800)
        {
            inst.imm |= 0xFFFFF000;
        }
        // cout<<"Type- "<<inst.type<<endl;
        // cout<<"Func3- "<<inst.funct3<<endl;
        // cout<<"Destination Register- "<<inst.rd<<endl;
        // cout<<"Source Register1- "<<inst.rs1<<endl;
        // cout<<"Immediate Register- "<<inst.imm<<endl;

    }
}


void execute() 
{
    int alu_result = 0;  // ALU result stored in rz
    bool branch_taken = false;  // For branch instructions
    // PC_temp=PC;
    switch(inst.opcode)
    {
        //I type
        case 0x3://load instructions
        {
            alu_result = R[inst.rs1] + inst.imm;  // define address which will be used to calculate memory address
            break;
        }
        
        
        case 0x13:
        {
            switch(inst.funct3)
            {
                case 0x0: //addi
                {
                    alu_result = R[inst.rs1] + inst.imm;
                    break;
                }
                case 0x1: //slli
                {
                    int shift_bits = inst.imm & 0x1F;
                    alu_result = R[inst.rs1]<<shift_bits;
                    break;
                }
                case 0x2: //slti
                {
                    if(R[inst.rs1]<inst.imm)
                    {
                        alu_result = 1;
                        break;
                    }
                    else
                    {
                        alu_result = 0;
                        break;
                    }
                }
                case 0x4: //xori
                {
                    alu_result = R[inst.rs1] ^ inst.imm;
                    break;
                }
                case 0x5: //srli, srai
                {
                    if(inst.funct7 == 0x0) //srli
                    {
                        int shift_bits = inst.imm & 0x1F; //only allow 31 bit shift as 32 bit => initial value replaced
                        alu_result = (unsigned int)R[inst.rs1] >> shift_bits;
                        break;
                    }
                    else if(inst.funct7 == 0x20) //srai
                    {
                        int shift_bits = inst.imm & 0x1F;
                        alu_result = R[inst.rs1] >> shift_bits;
                        break;
                    }
                    break;
                }
                case 0x6: //ori
                {
                    alu_result = R[inst.rs1] | inst.imm;
                    break;
                }
                case 0x7: //andi
                {
                    alu_result = R[inst.rs1] & inst.imm;
                    break;
                }

            }
            break;
        }
        //case 0x1B:
        
        case 0x67:
        {
            //jalr
            PC = (R[inst.rs1] + inst.imm);
            if(PC%4 !=0)
            {
                PC = floor(PC/4)*4;
            }
            break;
            
        }

        //S type
        case 0x23:
        {
            switch(inst.funct3)
            {
                case 0x0: //sb
                {
                    alu_result = R[inst.rs1] + inst.imm;  // Computing memory address
                    break;
                }
                case 0x1: //sh
                {
                    alu_result = R[inst.rs1] + inst.imm;  // Computing memory address
                    break;
                }
                case 0x2: //sw
                {
                    
                    alu_result = R[inst.rs1] + inst.imm;  // Computing memory address
                    break;
                }
                case 0x3: //sd - I type
                {
                    alu_result = R[inst.rs1] + inst.imm;  // Computing memory address
                    break;
                }
                

            }
            break;
        }

        //R type
        case 0x33:
        {
            switch(inst.funct3)
            {
                case 0x00: //add,sub,mul
                {
                    if(inst.funct7 == 0x00) //add
                    {
                        alu_result = R[inst.rs1] + R[inst.rs2];
                        break;
                    }
                    else if(inst.funct7 == 0x20) //sub
                    {
                        alu_result = R[inst.rs1] - R[inst.rs2];
                        break;
                    }
                    else if(inst.funct7 == 0x01) //mul
                    {
                        alu_result = (int32_t)R[inst.rs1] * (int32_t)R[inst.rs2];
                        break;
                    }
                    break;
                }
                case 0x01: //sll
                {
                    int shift_bits = R[inst.rs2] & 0x1F;
                    alu_result = R[inst.rs1]<<shift_bits;
                    break;
                }
                case 0x02: //slt
                {
                    if(R[inst.rs1]<R[inst.rs2])
                    {
                        alu_result = 1;
                        break;
                    }
                    else
                    {
                        alu_result = 0;
                        break;
                    }
                }
                //case 0x3: //sltu
                case 0x04: //xor
                {
                    alu_result = R[inst.rs1] ^ R[inst.rs2];
                    break;
                }
                case 0x05: //srl, sra
                {
                    if(inst.funct7 == 0x00) //srl
                    {
                        int shift_bits = R[inst.rs2] & 0x1F;
                        alu_result = (unsigned int)R[inst.rs1] >> shift_bits;
                        break;
                    }
                    else if(inst.funct7 == 0x20) //sra
                    {
                        int shift_bits = R[inst.rs2] & 0x1F;
                        alu_result = R[inst.rs1] >> shift_bits;
                        break;
                    }
                }
                case 0x6: //or
                {
                    alu_result = R[inst.rs1] | R[inst.rs2];
                    break;
                }
                case 0x7: //and
                {
                    alu_result = R[inst.rs1] & R[inst.rs2];
                    break;
                }

            }
            break;
        }
        
        //SB type
        case 0x63:
        {
            switch(inst.funct3)
            {
                case 0x0: //beq
                {
                    
                    if(R[inst.rs1]== R[inst.rs2])
                    {
                        branch_taken=true;
                        break;
                    }
                    break;
                }
                case 0x1: //bne
                {
                    // cout<<"beq"<<endl;
                    if(R[inst.rs1] != R[inst.rs2])
                    {
                        branch_taken=true;
                        break;
                    }
                    break;
                }
                case 0x4: //blt
                {
                    // cout<<"beq"<<endl;
                    if(R[inst.rs1] < R[inst.rs2])
                    {
                        branch_taken=true;
                        break;
                    }
                    break;
                }
                case 0x5: //bge
                {
                    if(R[inst.rs1] >= R[inst.rs2])
                    {
                        // cout<<"yes"<<endl;
                        branch_taken=true;
                        break;
                    }
                    break;
                }
            }
            if(branch_taken) 
            {
                // cout<<"PCi"<<PC<<endl;
                PC=PC_temp+inst.imm;
                // cout<<"PCf"<<PC<<endl;
            }
            break;
        }

        //U type
        case 0x17:
        {
            //auipc
            //shift is already doe as bits extracted by shifting and with mask for MSB 20 bits
            alu_result=inst.imm + PC_temp;
            break;
        }
        case 0x37:
        {
            //lui
            //shift is already done as bits extracted by shifting and with mask for MSB 20 bits
            alu_result= inst.imm;
            break;
        }

        //UJ type
        case 0x6F:
        {
            //jal
            PC = PC_temp+ inst.imm;  // Jump
            break;
        }

    }
    rz = alu_result;
    
}


void mem() 
{
    int mem_out = 0; // Output from memory access
    unsigned int address = rz; // Get computed memory address from execute()

    switch(inst.opcode)
    {
        // 🟢 **I-Type Load Instructions**
        case 0x3:
        {
            switch(inst.funct3) 
            {
                case 0x0: // LB (Load Byte)
                {
                    int8_t byte_value = *(int8_t*)&MEM[address - 0x10000000];  // Read signed byte
                    mem_out = (int)byte_value;  // Sign-extend to 32-bit
                    break;
                }
                case 0x1: // LH (Load Halfword)
                {
                    int16_t halfword_value = *(int16_t*)&MEM[address - 0x10000000];  // Read signed halfword
                    mem_out = (int)halfword_value;  // Sign-extend to 32-bit
                    break;
                }
                case 0x2: // LW (Load Word)
                {
                    int32_t word_value = *(int32_t*)&MEM[address - 0x10000000];  // Read 32-bit word
                    mem_out = (int)word_value;  // Store 32-bit result
                    break;
                }
                case 0x3: // LD (Load Doubleword, RV64 only)
                {
                    int64_t doubleword_value = *(int64_t*)&MEM[address - 0x10000000];  // Read 64-bit doubleword
                    mem_out = (int)doubleword_value;  // Store 64-bit result
                    break;
                }
            }
            ry = mem_out; // Store memory output for write-back stage
            break;
        }

        // 🟡 **S-Type Store Instructions**
        case 0x23:
        {
            switch(inst.funct3)
            {
                case 0x0: // SB (Store Byte)
                {
                    *(int8_t*)&MEM[address - 0x10000000] = (int8_t)R[inst.rs2]; // Store 8-bit value
                    break;
                }
                case 0x1: // SH (Store Halfword)
                {
                    *(int16_t*)&MEM[address - 0x10000000] = (int16_t)R[inst.rs2]; // Store 16-bit value
                    break;
                }
                case 0x2: // SW (Store Word)
                {
                    
                    *(int32_t*)&MEM[address - 0x10000000] = (int32_t)R[inst.rs2]; // Store 32-bit value
                    break;
                }
                case 0x3: // SD (Store Doubleword, RV64 only)
                {
                    *(int64_t*)&MEM[address - 0x10000000] = (int64_t)R[inst.rs2]; // Store 64-bit value
                    break;
                }
            }
            break;
        }
    }
}


void write_back() 
{
    switch(inst.opcode)
    {
        
        //I type
        case 0x3://load instructions
        {
            if(inst.rd==0)
            {
                break;
            }
            R[inst.rd] = ry;
            break;
        }
        
        case 0x13:
        {
            if(inst.rd==0)
            {
                break;
            }
            R[inst.rd] = rz;
            break;
        }
        
        case 0x67:
        {
            //jalr
            if(inst.rd==0)
            {
                break;
            }
            R[inst.rd] = PC_temp;
            break;
            
        }

        //R type
        case 0x33:
        {
            if(inst.rd==0)
            {
                break;
            }
            R[inst.rd] = rz;
            break;
        }

        //U type
        case 0x17:
        {
            //auipc
            if(inst.rd==0)
            {
                break;
            }
            R[inst.rd] = rz;
            break;
        }
        case 0x37:
        {
            //lui
            if(inst.rd==0)
            {
                break;
            }
            R[inst.rd] = rz;
            break;
        }

        //UJ type
        case 0x6F:
        {
            //jal
            if(inst.rd==0)
            {
                break;
            }
            R[inst.rd] = PC_temp;
            break;
        }

    }    
}


void print_memory(const string& filename) {
    ofstream outFile(filename);  // Open file for writing
    if (!outFile) {
        cerr << "Error: Unable to open file " << filename << " for writing!" << endl;
        return;
    }

    outFile << "DATA MEMORY:\n";
    outFile << "--------------------------------\n";

    for (unsigned int i = 0; i < 4000; i += 4) {
        unsigned int value = *(unsigned int*)(MEM + i); // Read 4 bytes
        outFile << "DATA MEMORY [0x" << hex << setw(4)<< setfill('0') << i+ + 0x10000000  << "]:  = 0x" 
                << hex << setw(8) << setfill('0') << value << endl;
    }

    outFile << "--------------------------------\n";
    outFile.close();  // Close the file

    cout << "Data Memory saved to " << filename << endl;  // Notify user
}

void print_inst_memory(const string& filename) {
    ofstream outFile(filename);  // Open file for writing
    if (!outFile) {
        cerr << "Error: Unable to open file " << filename << " for writing!" << endl;
        return;
    }

    outFile << "INSTRUCTION MEMORY:\n";
    outFile << "--------------------------------\n";

    for (unsigned int i = 0; i < 4000; i += 4) {
        unsigned int value = *(unsigned int*)(inst_MEM + i); // Read 4 bytes
        outFile << "INSTRUCTION MEMORY [0x" << hex << setw(4) << setfill('0') << i << "]: = 0x" 
                << hex << setw(8) << setfill('0') << value << endl;
    }

    outFile << "--------------------------------\n";
    outFile.close();  // Close the file

    cout << "Instruction Memory saved to " << filename << endl;  // Notify user
}

void print_registers(const string& filename) 
{
    ofstream outFile(filename);  // Open file for writing
    if (!outFile) {
        cerr << "Error: Unable to open file " << filename << " for writing!" << endl;
        return;
    }

    outFile << "REGISTER FILE:\n";
    outFile << "--------------------------------\n";

    for (int i = 0; i < 32; i++) {
        outFile << "x" << setw(2) << setfill('0') << i  // Print register number (x0, x1, ..., x31)
             << ": = 0x" << hex << setw(8) << setfill('0') << R[i] <<endl; // Print register value in hex
        
        if ((i + 1) % 4 == 0)  // Print 4 registers per line
            cout << endl;
    }

    outFile << "--------------------------------\n";
    outFile.close();  // Close the file

    cout << "Instruction Memory saved to " << filename << endl;  // Notify user
}

void run_RISCVsim(const string& filename) 
{
    ofstream outFile(filename);  // Open file for writing
    if (!outFile) {
        cerr << "Error: Unable to open file " << filename << " for writing!" << endl;
        return;
    }
    outFile << "SIMULATION DETAILS:\n";
    
    int cycle=0;
    while(1)
    {
        
        cycle++;
        cout<<cycle<<endl;
        fetch();
        
        if(instruction_word == 0x0)
        {
            cout << "Program completed: Reached exit instruction.\n";
            break;
        }
        outFile << "--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
        outFile<<"CYCLE: "<<cycle<<endl;
        outFile<<"STAGE1:"<<endl;
        outFile<<"Fetch: PC fetched as "<<PC_temp<<" and updated to "<<PC<<" , Instruction Fetched: 0x"<<hex<<instruction_word<<endl;
        outFile<<"STAGE2:"<<endl;
        decode();
        outFile<<"Decode: Instruction decoded as: type- "<<inst.inst_type<<" opcode- "<<inst.opcode<<endl;
        outFile<<"STAGE3:"<<endl;
        execute();
        outFile<<"Execute: The output of ALU is stored in temporary Register Rz= "<<rz<<endl;
        outFile<<"STAGE4:"<<endl;
        cout<<"PC"<<PC<<endl;
        mem();
        outFile<<"Memory Access: Memory Access performed if required and for Load instruction value is stored in Ry (default=0)= "<<ry<<endl;
        outFile<<"STAGE5:"<<endl;
        write_back();
        outFile<<"Register Writeback: Required data written to Register File"<<endl;
        outFile << "--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
    //   print_registers();
      
    }
    
    
    outFile.close();  // Close the file

    cout << "Simulation Results saved to " << filename << endl;  // Notify user
    
    // print_memory();
    
}
  
void reset_proc() 
{
    // Reset all registers to 0
    for (int i = 0; i < 32; i++) 
    {
        R[i]=0;
    }

    // Reset PC (Program Counter)
    PC=0;

    // Reset memory to 0
    memset(MEM, 0, sizeof(MEM));

    // Reset ALU result and temporary PC storage
    rz=0;
    ry=0;
    PC_temp=0;

    cout << "Processor reset: Registers, PC, and Memory cleared.\n";
}

void load_program_memory(const string& file_name) 
{
    ifstream file(file_name);  // Open file for reading

    if (!file) {
        cerr << "Error opening input memory file: " << file_name << endl;
        exit(1);
    }

    unsigned int address, instruction;

    // Read hex address and instruction pairs
    // while (file >> hex >> address >> instruction) 
    // {
    //     cout<<address<<instruction<<endl;
    //     write_word(MEM, address, instruction);
    // }
    string line;
    while (getline(file, line))  // Read each line
    {
        stringstream ss(line);
        unsigned int address, instruction;
        // char comma;  // To handle the comma between values
        
        if (ss >> hex >> address >> instruction) {  
            // cout << "Read Address: 0x" << hex << address << " Instruction: 0x" << instruction << endl;
            write_word(inst_MEM, address, instruction);
            
        }
    }

    file.close();
    cout << "Memory loaded successfully from " << file_name << endl;
}


void write_data_memory() 
{
    ofstream file("data_out.mem");  // Open file in write mode

    if (!file) {
        cerr << "Error opening data_out.mem for writing\n";
        return;
    }

    // Write all memory contents in 4-byte chunks
    for (unsigned int i = 0; i < 4000; i += 4) {
        file << hex << i << " " << read_word(MEM, i) << endl;
    }

    file.close();
    cout << "Memory contents written to data_out.mem\n";
}

void swi_exit() 
{
    write_data_memory();  // Save memory before exit
    exit(0);  // Stop execution
}
  
int main(int argc, char** argv) 
{
    // Check if the correct number of arguments is provided
    if (argc < 2) 
    {
        cerr << "Incorrect number of arguments.\n";
        cerr << "Usage: ./myRISCVSim <input mc file>\n";
        return 1;  // Return error code instead of using exit()
    }

    string prog_mc_file = argv[1];  // Store input filename

    // Reset the processor
    reset_proc();

    // Load the program memory
    load_program_memory(prog_mc_file);
    

    // Run the simulator
    run_RISCVsim("simulation_results.txt");
    print_inst_memory("instruction_memory.txt");
    print_memory("data_memory.txt");
    print_registers("register_file.txt");


    return 0;  // Return success code
}
