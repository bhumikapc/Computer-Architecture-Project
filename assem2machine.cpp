#include<bits/stdc++.h>
#define ll long long int
#define ull unsigned long long
#define START 268435456 //0X10000000 (for data segment start)
using namespace std;
vector<string>codeinit; //To store input code (text segment)
vector<string>code;     //To store code after considering load and pseudo instructions
vector<string>instructions_manual;
ll sizeIset,size;
int binary[32];
ll pccount = 0;
string line, opcodes, func3s, func7s, rds, rs1s, rs2s, immediates, instype;

typedef struct 
{
	string s;
	int index;
}lab;

vector<lab>Label;

string datamemory[4000]; // Initial Size of Data Memory is Fixed 4000 Bytes (saves 2 hex digits)
string datamemory2[1000]; //Saves word by word (8 hex digits)
string dataIndex[1000]; //To save index of data segment in memory

struct datafile
{
    string name;
    string type;
    vector<string> value;
};

struct seg
{
    string name;
    ll position;
};

vector<seg>datalabel;

//Inverts decimal, input decimal must be sent in positive form (imme>0)
ull InvertDecimal(ll imme,int bit) //Invert using 2's complement
{
	vector<int>b_form;
	//imme=-imme;
	// Convert decimal to binary (32 bits)
    for (int i = bit - 1; i >= 0; i--) {
        b_form.push_back((imme >> i) & 1);
    }

	// Step 1: One's complement (flip all bits)
    for (int i = 0; i < bit; i++) {
        b_form[i] = 1 - b_form[i];  // Flip 0 to 1 and 1 to 0
    }

    // Step 2: Add 1 (to get two's complement)
    for (int i = bit - 1; i >= 0; i--) {
        if (b_form[i] == 0) {  
            b_form[i] = 1;  // Add 1 and stop
            break;
        }
        b_form[i] = 0;  // Propagate carry
    }

    if (instype == "SB"|| instype == "UJ"){
        for (int i = bit - 1; i > 0; i--) {
        b_form[i] = b_form[i - 1];
         }
    // Set b_form[0] to 1
        b_form[0] = 1;
    }

    //Converting back to decimal (without sign)
	ull num=0;
	ll mul=1;
	for(int i=bit-1;i>=0;i--)
	{
		num += b_form[i]*mul;
		mul*=2;
	}
	return num;
}

//Converting Decimal to Hexadecimal:String (both positive and Complemented ones) within given length
string DecimalToHex(ll num, int len) {
    string ans;
    for(int i=0;i<len;i++)
    {
        ll rem = num%16;
        if(rem<=9)
        {
            ans += (rem+48);
        }
        else
        {
            ans += (rem-10+65);
        }
        num/=16;
    }
    reverse(ans.begin(),ans.end());

    return ans;
}

// To convert dec to hex without len for PC
string DecimalToHex(ll num) {
    stringstream ss;
    ss << hex << num;
    return ss.str();
}

//Handles String to Decimal conversion when it is Decimal/Hex value within given length
ll StringToDecimal(string s,int len)
{
    int length = s.size();
    ll num = 0;
    bool isHex = (s.find('x') != string::npos || s.find('X') != string::npos);
    bool isNeg = (s[0]=='-');

    if(!isHex)//If decimal
    {
        ll mul = 1;
        int last = 0;
        if(isNeg){		
       		last = 1;
        }
        //Converting string to decimal value
        for(int i = length-1;i>=last;i--) 
        {
            num += (s[i]-48)*mul; //'0' has ascii value 48
            mul*=10;
        }
        //Inverting negative decimals using 2's complement
        if(isNeg)
        {
        	num = InvertDecimal(num,len*4);  
        }
    }

    else //If already Hexadecimal
    { 
            string hexPart = isNeg ? s.substr(1) : s;
        num = 0;
        // Convert hex string to number
        stringstream ss;
        ss << std::hex << hexPart;
        ss >> num;
        
        hexPart = hexPart.substr(2); //remove 0x
        char firstDigit = hexPart[0];
        
        // Check if the first digit corresponds to 8-F range (sign bit set)
        if ((firstDigit >= '8' && firstDigit <= 'F') || (firstDigit >= '8' && firstDigit <= 'f') && (isNeg==false)){
            isNeg = true;}
        
        else if ((firstDigit >= '8' && firstDigit <= 'F') || (firstDigit >= '8' && firstDigit <= 'f') && (isNeg==true))
            isNeg = false;
        
        // Apply negative if it started with '-'
        //cout<<isNeg<<endl;
        //cout<<hexPart<<endl;
        //cout<<num<<endl;
        if (isNeg)
            num = InvertDecimal(num, len*4);
    }
    
    return num;
}

//Length is in number of hex digits
string StringToHex(string s,int len){
   bool isHex = (s.find('x') != string::npos || s.find('X') != string::npos);
   bool isNeg = (s[0]=='-');
   ull num = 0;
   string ans;
   if (isHex){
          // Remove '-' for parsing if negative
    string hexPart = isNeg ? s.substr(1) : s;

    // Convert hex string to number
    stringstream ss;
    ss << std::hex << hexPart;
    ss >> num;
    //cout<<hexPart<<endl;
    //cout<<isNeg<<endl;
    //cout<<num<<endl;
    if (isNeg){
        num = InvertDecimal(num, len*4); 
        cout<<num<<endl;
        ans = DecimalToHex(num, len);
    }
    else {
        ans = hexPart.substr(2);
        while (ans.size() < len) 
            ans = "0" + ans;
        //cout<<"ans: "<<ans<<endl;
        if(ans.size() > len)
            ans = ans.substr(ans.size()-len); 
    }
   }
   
   else{
       num =  StringToDecimal(s, len);
       ans = DecimalToHex(num, len);
   }
   return ans;
}

//Handles Character to Hex conv (string) when it is any string in general
string charToHex(char c) {
    stringstream ss;
    ss << hex << int(c);  // Convert char to int, then to hex
    return ss.str();
}

// Function to convert 32-bit binary array to a hex string
string BinaryToHex(int binary[32]) {
    stringstream ss;
    for (int i = 0; i < 32; i += 4) { // Process 4 bits at a time
        int num = 0;
        for (int j = 0; j < 4; j++) {
            num = (num << 1) | binary[i + j]; // Convert 4 bits to a decimal value
        }
        ss << hex << num;
    }
    return ss.str();
}

//convert binary (array) to string including start and end positions
string binaryToString(int start, int end) {
    string result = "";
    for (int i = start; i <= end; i++) {
        result += to_string(binary[i]);
    }
    return result;
}

//convert input vector to string including start and end positions
string binaryToString(int start, int end, const vector<int>& binary) {
    string result = "";
    for (int i = start; i <= end; i++) {
        result += to_string(binary[i]);
    }
    return result;
}

//convert Decimal to Binary (vector) of given size
vector<int> decimalToBinary(int num, int bits) {
    vector<int> binconv(bits, 0); // Initialize vector with 'bits' size filled with 0
    for (int i = bits - 1; i >= 0 && num > 0; i--) {
        binconv[i] = num % 2;
        num /= 2;
    }
    return binconv;
}

//Read Data section (.data) and store values in Data segment of memory
void DataSegment()
{
    ifstream file("input.asm"); //Opens the test file
    if (!file.is_open()) return;

    string word;
    vector<datafile> stored; //To store each new definition (statements of .data section)
    int start = 0, unknownLabel = 1;

    while (file >> word)
    {
        if (start == 2) break; // Skip after ".text"

        if (start == 0 && word == ".data") 
        {
            start = 1; // Start processing data section
            continue;
        }
        
        if (start == 1)
        {
            if (word == ".text")
            {
                start = 2; // End data section
                continue;
            }

            datafile temp;
            size_t colonPos = word.find(':');

            if (colonPos != string::npos) // Label found (array: .word ...)
            {
                temp.name = word.substr(0, colonPos); // Get label name
                //If array:.word (no space)
                if(colonPos + 1 < word.size() && word[colonPos+1]=='.'){
                    temp.type = word.substr(colonPos+2);
                }
                else{
                    // Read the type (skip spaces and dots)
                    file >> word;
                    if (!word.empty() && word[0] == '.')
                        word.erase(word.begin());
                    temp.type = word;
                }
            }
            else // No label, directly ".word ..." (for unnamed data)
            {
                if (word[0] == '.')
                    word.erase(word.begin());
                temp.type = word;
                temp.name = "UnknownLabel" + to_string(unknownLabel++);
            }

            // Get values (like "10 20 30")
            string line;
            getline(file, line);
            stringstream ss(line);
            while (ss >> word)
                temp.value.push_back(word);

            stored.push_back(temp);
        }
    }
    
    file.close();
    int pos = 0;
    for(int i = 0; i < stored.size(); i++)
    {

        seg temp;   temp.name = stored[i].name;    temp.position = pos + START;  
        datalabel.push_back(temp);
        if(stored[i].type == "byte")
        {
            for(int j = 0; j < stored[i].value.size(); j++)
            {
                string s = StringToHex(stored[i].value[j],2);
                datamemory[pos++] = s;
            }
        }
        else if(stored[i].type == "word")
        {
            for(int j = 0; j < stored[i].value.size(); j++)
            {
                string s = StringToHex(stored[i].value[j],8);
                string temp;    
                temp+=s[6]; temp+=s[7];
                datamemory[pos++] = temp; temp.clear();
                temp+=s[4]; temp+=s[5];
                datamemory[pos++] = temp; temp.clear();
                temp+=s[2]; temp+=s[3];
                datamemory[pos++] = temp; temp.clear();
                temp+=s[0]; temp+=s[1];
                datamemory[pos++] = temp; temp.clear();
            }
        }
        else if(stored[i].type == "half")
        {
             for(int j = 0; j < stored[i].value.size(); j++)
            {
                string s = StringToHex(stored[i].value[j],4);
                string temp;    
                temp+=s[2]; temp+=s[3];
                datamemory[pos++] = temp; temp.clear();
                temp+=s[0]; temp+=s[1];
                datamemory[pos++] = temp; temp.clear();
            }
        }
         else if(stored[i].type == "asciz")
        {
            //cout<<stored[i].value.size()<<endl;
             for(int j = 0; j < stored[i].value.size(); j++)
            {
                string element = stored[i].value[j];
                element.erase(remove(element.begin(), element.end(), '\"'), element.end()); //Remove double quotes
                //cout<<element<<endl;
                for(int k = 0; k < element.size(); k++){
                    //cout<<element[k]<<endl;
                    string s = charToHex(element[k]); //As size of ascii is 8 bits
                    datamemory[pos++] = s; 
                }
            }
            datamemory[pos++] = "00"; //as asciiz adds null character at end by default
        }
        else if(stored[i].type == "dword")
        {
             for(int j = 0; j < stored[i].value.size(); j++)
            {
                string s = StringToHex(stored[i].value[j],16);
                string temp; 
                for (int i = 14; i >= 0; i -= 2) {  // Iterate in reverse, two characters at a time
                    temp += s[i];
                    temp += s[i + 1];
                    datamemory[pos++] = temp;
                    temp.clear();
                }
                /*Basically:
                temp+=s[14]; temp+=s[15];   
                datamemory[pos++] = temp; temp.clear();
                temp+=s[12]; temp+=s[13];
                datamemory[pos++] = temp; temp.clear();
                temp+=s[10]; temp+=s[11];
                datamemory[pos++] = temp; temp.clear();
                temp+=s[8]; temp+=s[9];
                datamemory[pos++] = temp; temp.clear();
                temp+=s[6]; temp+=s[7];   
                datamemory[pos++] = temp; temp.clear();
                temp+=s[4]; temp+=s[5];
                datamemory[pos++] = temp; temp.clear();
                temp+=s[2]; temp+=s[3];
                datamemory[pos++] = temp; temp.clear();
                temp+=s[0]; temp+=s[1];
                datamemory[pos++] = temp; temp.clear();*/
            }
        }
        else {
            //cout<<"HEllo"<<endl;
            ofstream file("output.mc", ios_base::app);
            file.close();
            continue;
                    }

    }
}

//To extract ISA representations from Instructions Manual provided (separate text file)
void Instructions_Manual() {
    ifstream myFile("Instructions_Manual.txt");
    string line;
    if (!myFile) return;  // Check if file opens successfully

    instructions_manual.clear();  // Ensure vector is empty before adding lines
    while (getline(myFile, line)) {  // Read entire line, including spaces
        instructions_manual.push_back(line);  // Store each line in vector
    }
    
    sizeIset = instructions_manual.size();
    myFile.close();
}

//To extract all code from text Segment and store in code vector
void TextSegment() {
   ifstream myFile("input.asm");
    if (!myFile) {
        cerr << "Error: Cannot open input.asm" << endl; //check once
    }
	string line;
	int flag = 0;
	int start = 0;
	while(getline(myFile,line))
	{
		if(line==".data")
			flag = 1;
		if(line==".text")
		{
			flag = 0;
			continue;
		}
		if(flag!=1)
			code.push_back(line);
	}
    cout<<"hello";
    cout<<code.size()<<endl;

    myFile.close(); 
}

//To extract all labels from the code (along with index starting from 0) and save in Label(s,index)
void ExtractLabels() {
    codeinit = code;  // Deep copy of the vector
    int count = -1;
    for (const string& line : codeinit) {
        string instruction;
        size_t start = 0;
        //Storing label names and positions
        while (start < line.size() && isspace(line[start])) { //skip spaces
            start++;
        }
        size_t end = start;
        while (end < line.size() && !isspace(line[end]) && line[end] != ':') { //extract first word (label/instruction name)
            instruction += line[end];
            end++;
        }

        if (end < line.size() && line[end] == ':') {  //If label, then push into vector and extract following instruction name
            lab temp;
            temp.s = instruction;
            temp.index = count + 1;
            Label.push_back(temp);

            end++; // Move past the ':'
            while (end < line.size() && isspace(line[end])) { //skip spaces
                end++;
            }
            instruction.clear();
            while (end < line.size() && !isspace(line[end])) { //extract word (instruction name) after label
                instruction += line[end];
                end++;
            }
        }

        //Doing this to keep the instruction count for label index (checking if instruction names are valid)
        if (instruction == "la") {
            count += 2;
            continue;
        } else if (instruction == "lb" || instruction == "lh" || instruction == "lw"|| instruction == "ld") {
            string dataLabelName;
            size_t dataLabelStart = line.rfind(' ');
            if (dataLabelStart != string::npos) {
                dataLabelStart++; // Move past the space
                dataLabelName = line.substr(dataLabelStart);
            }

            //If found in datalable, then ins takes 2 cycles as memory access. If not, then ignore and just do count+1 (below)
            bool foundDataLabel = false;
            for (const seg& dl : datalabel) {
                if (dl.name == dataLabelName) {
                    foundDataLabel = true;
                    count += 2;
                    break;
                }
            }
            if (foundDataLabel) continue;
        }

        bool instructionFound = false;
        for (const string& format : instructions_manual) {
            string formatInstruction = format.substr(0, format.find(' '));
            if (instruction == formatInstruction) {
                count++;
                instructionFound = true;
                break;
            }
        }
        if (instructionFound) continue;
    }
}

//lw, lh,...function

//To replace all sp with x2 (as sp is alias for register x2)
void sp_to_x2() {
    for (string &line : code) {
        size_t pos = 0;
        while ((pos = line.find("sp", pos)) != string::npos) {
            // Ensure 'sp' is a standalone register name (not part of a word)
            if ((pos == 0 || line[pos - 1] == ' ' || line[pos - 1] == '(' || line[pos - 1] == ',') &&
                (pos + 2 == line.size() || line[pos + 2] == ' ' || line[pos + 2] == ')' || line[pos + 2] == ',')) {
                line.replace(pos, 2, "x2");
            }
            pos += 2; // Move past the replaced "x2"
        }
    }
}

// Convert to hexadecimal and write pc & machine code to output.mc
void WriteToFile() {
    ofstream file("output.mc", ios_base::app);

    // Write program counter in hex (8 digits)
    file << "0x" << DecimalToHex(pccount) << " ";

    // Convert binary to hex and write to file (8 hex digits)
    file << "0x" << BinaryToHex(binary) << " , ";

    //Write original code
    file << line << " ";
    
    //Write parts of machine code as comments
    file<<"# "<<opcodes<<"-"<<func3s<<"-"<<func7s<<"-"<<rds<<"-"<<rs1s<<"-"<<rs2s<<"-"<<immediates<<"\n";
    
    // Increment program counter by 4 bytes (RISC-V word size)
    pccount += 4;

    file.close();
}

//To get label offset
int LabelOffset(const string &label, int ind) {
    for (const auto &lbl : Label) {  // Iterate through all labels
        if (label == lbl.s) {  // Match label name
            return (lbl.index - ind) * 2;  // Calculate and return offset (branch offsets are specified in terms of 2-byte halfwords)
        }
    }
    return 0;  // Default if label not found
}

//To process I type Instructions
void I_Type(int index, int ManualIndex) {
    ll rd = 0, rs1 = 0, imm = 0;
    bool hasOffset = false, isNegative = false, lwform = false;
    
    line = code[index];
    size_t pos = line.find('(');
    hasOffset = (pos != string::npos); // Check if offset format (with parentheses)

    // Parse rd (destination register)
    size_t j = line.find("x") + 1;
    while (isdigit(line[j])) rd = rd * 10 + (line[j++] - '0');

    // Parse rs1 (source register)
    size_t k = line.find("x", j) + 1;

    //check if "lw x1,100,x2" form or "addi x1,x2,100" form
    for (int a = j; a < k; ++a) { //j is ens of rd, k is start of rs1
        if (isdigit(line[a])) { // Check if the character is a digit (0-9) 
            lwform = true;
        }
    }

    // Parse rd (destination register)
    while (isdigit(line[k])) rs1 = rs1 * 10 + (line[k++] - '0');

    // Parse immediate value
    size_t i = 0;
    if(lwform)
        i = line.find_first_of("-0123456789", j);
    else
        i = line.find_first_of("-0123456789", k);
    int posi = line.find(' ', i); // Find first space from index i
    string imm_str = line.substr(i, posi - i-1); // Substring from i to (pos - 1)
    imm = StringToDecimal(imm_str, 3);
    // if (line[i] == '-') { isNegative = true; i++; }
    //while (isdigit(line[i])) imm = imm * 10 + (line[i++] - '0');
    //if (isNegative) imm = InvertDecimal(imm, 12); // Two's complement for negative immediates

    // Encode binary
    string reference = instructions_manual[ManualIndex];
    j = 25;
    k = reference.find(' ') + 1; // Skip to binary section

    // opcode (7 bits)
    for (int l = 0; l < 7; l++) binary[j+l] = (reference[k++] - '0');

    j = 24;
    // rd (5 bits)
    for (int k = 0; k < 5; k++) binary[j--] = rd % 2, rd /= 2;

    // funct3 (3 bits)
    j = 17;
    k = reference.find(' ',k) + 1; // find f3
    for (int l = 0; l < 3; l++) binary[j+l] = (reference[k++] - '0');
    j = 16;

    // rs1 (5 bits)
    for (int k = 0; k < 5; k++) binary[j--] = rs1 % 2, rs1 /= 2;

    // immediate (12 bits)
    for (int k = 0; k < 12; k++) binary[j--] = imm % 2, imm /= 2;

    // Convert binary parts to strings
    opcodes = binaryToString(25, 31);
    func3s = binaryToString(17, 19);
    func7s = "NULL";
    rds = binaryToString(20, 24);
    rs1s = binaryToString(12, 16);
    rs2s = "NULL";
    immediates = binaryToString(0, 11);

    // Convert to hexadecimal and write pc & machine code to output.mc
    WriteToFile();
}

//To process R type Instructions
void R_Type(int index, int ManualIndex) {
    ll rd = 0, rs1 = 0, rs2 = 0;
    line = code[index];

    // Parse rd (destination register)
    size_t i = line.find("x") + 1;
    while (isdigit(line[i])) rd = rd * 10 + (line[i++] - '0');

    // Parse rs1 (source register)
    i = line.find("x", i) + 1;
    while (isdigit(line[i])) rs1 = rs1 * 10 + (line[i++] - '0');

    // Parse rs2 (source register)
    i = line.find("x", i) + 1;
    while (isdigit(line[i])) rs2 = rs2 * 10 + (line[i++] - '0');

    // Encode binary
    string reference = instructions_manual[ManualIndex];

    // Parse Format and fill binary array
    int k = 25;
    int j = reference.find(' ') + 1; // Skip to binary section

    // opcode
    for (int l = 0; l < 7; l++) binary[k++] = reference[j++] - '0';

    // rd
    for (int l = 0; l < 5; l++) binary[24 - l] = (rd >> l) & 1;

    j += 1; // Skip space

    // funct3
    for (int l = 0; l < 3; l++) binary[17 + l] = reference[j++] - '0';

    // rs1
    for (int l = 0; l < 5; l++) binary[16 - l] = (rs1 >> l) & 1;

    // rs2
    for (int l = 0; l < 5; l++) binary[11 - l] = (rs2 >> l) & 1;

    j += 1; // Skip space

    // func7
    for (int l = 0; l < 7; l++) binary[l] = reference[j++] - '0';

    // Convert binary parts to strings
    opcodes = binaryToString(25, 31);
    func3s = binaryToString(17, 19);
    func7s = binaryToString(0, 6);
    rds = binaryToString(20, 24);
    rs1s = binaryToString(12, 16);
    rs2s = binaryToString(7, 11);
    immediates = "NULL";

    // Convert to hexadecimal and write pc & machine code to output.mc
    WriteToFile();
}

//To process S type Instructions
void S_Type(int index, int ManualIndex) {
    ll rs1 = 0, rs2 = 0, imme = 0;
    line = code[index];

    // Parse rs2 (source register)
    size_t i = line.find("x") + 1;
    while (isdigit(line[i])) rs2 = rs2 * 10 + (line[i++] - '0');

    // Extract immediate (handle hex or decimal)
    i = line.find_first_of("-0123456789", i);
    int pos = line.find(' ', i); // Find first space from index i
    string imm_str = line.substr(i, pos - i-1); // Substring from i to (pos - 1)
    imme = StringToDecimal(imm_str, 3);

    // Parse rs1 (source register)
    i = line.find("x", i) + 1;
    while (isdigit(line[i])) rs1 = rs1 * 10 + (line[i++] - '0');

    // Encode binary
    string reference = instructions_manual[ManualIndex];

    // Parse Format and directly set binary values
    int j = reference.find(' ') + 1; // Skip to binary section

    // opcode
    for (int k = 25; k <= 31; k++) binary[k] = reference[j++] - '0'; 

    // imm[4:0]
    for (int k = 24; k >= 20; k--) binary[k] = imme & 1, imme >>= 1;  

    j += 1; // Skip space

    // funct3
    for (int k = 17; k <= 19; k++) binary[k] = reference[j++] - '0';

    // rs1
    for (int k = 16; k >= 12; k--) binary[k] = rs1 & 1, rs1 >>= 1;  

     // rs2
    for (int k = 11; k >= 7; k--) binary[k] = rs2 & 1, rs2 >>= 1;   

    // imm[11:5]
    for (int k = 6; k >= 0; k--) binary[k] = imme & 1, imme >>= 1;  

    // Convert binary parts to strings
    opcodes = binaryToString(25, 31);
    func3s = binaryToString(17, 19);
    func7s = "NULL";
    rds = "NULL";
    rs1s = binaryToString(12, 16);
    rs2s = binaryToString(7, 11);
    immediates = binaryToString(0, 6);    

    // Convert to hexadecimal and write pc & machine code to output.mc
    WriteToFile();
}

// To process SB-type Instructions
void SB_Type(int index, int ManualIndex) {
    ll rs1 = 0, rs2 = 0, imme = 0;
    line = code[index];
    string label;
    bool isNegative;
    
    // Parse rs1
    size_t i = line.find('x') + 1;
    while (isdigit(line[i])) rs1 = rs1 * 10 + (line[i++] - '0');
    
    // Parse rs2
    i = line.find('x', i) + 1;
    while (isdigit(line[i])) rs2 = rs2 * 10 + (line[i++] - '0');
    
    // Parse label
    i = line.find_first_not_of(" ,", i); //returns index of  first character that is NOT a space (' ') or a comma (',')
    while (i < line.size() && line[i] != ' ' && line[i] != '#') label += line[i++];
    imme = LabelOffset(label, index);
    if (imme < 0) {
        isNegative = true;
        imme = InvertDecimal(-imme, 12);
    }

    vector<int> binconv;
    //convert label imm to binary
    if(isNegative)
        binconv = decimalToBinary(imme, 12);
    else
        binconv = decimalToBinary(imme/2, 12);
    reverse(binconv.begin(), binconv.end()); // Reverse the vector so that binconv[0] is LSB
    
    // Encode binary
    string reference = instructions_manual[ManualIndex];
    size_t j = reference.find(' ') + 1;

    // opcode
    for (int k = 25; k <= 31; k++) binary[k] = reference[j++] - '0';
    j++;

    // funct3
    for (int k = 17; k <= 19; k++) binary[k] = reference[j++] - '0';
    
    // rs1
    for (int k = 16; k >= 12; k--) binary[k] = rs1 & 1, rs1 >>= 1;
    
    // rs2
    for (int k = 11; k >= 7; k--) binary[k] = rs2 & 1, rs2 >>= 1;

    // Immediate fields
    int z = 3;
    for (int k = 20; k <= 23; k++) binary[k] = binconv[z--];
    binary[24] = binconv[10]; // bit 11
    z = 9;
    for (int k = 1; k <= 6; k++) binary[k] = binconv[z--];
    binary[0] = binconv[11]; // bit 12

    // Convert binary parts to strings
    opcodes = binaryToString(25, 31);
    func3s = binaryToString(17, 19);
    func7s = "NULL";
    rds = "NULL";
    rs1s = binaryToString(12, 16);
    rs2s = binaryToString(7, 11);
    
    reverse(binconv.begin(), binconv.end()); // Reverse the vector so that binconv[0] is MSB
    immediates = binaryToString(0, 11, binconv);
    
    // Convert to hexadecimal and write pc & machine code to output.mc
    WriteToFile();
}


// To process U-type Instructions
void U_Type(int index, int ManualIndex) {
    ll rd = 0, imme = 0;
    line = code[index];

    // Parse rd (destination register)
    size_t i = line.find("x") + 1;
    while (isdigit(line[i])) rd = rd * 10 + (line[i++] - '0');

    // Extract immediate (handle hex or decimal)
    i = line.find_first_of("-0123456789", i);
    int pos = line.find(' ', i); // Find first space from index i
    string imm_str = line.substr(i, pos - i-1); // Substring from i to (pos - 1)
    imme = StringToDecimal(imm_str, 5); //U_Type treats immediate as unsigned (error if signed)

    // Parse Format and fill binary array
    string reference = instructions_manual[ManualIndex];
    size_t j = reference.find(' ') + 1;

    // opcode
    for (int k = 25; k <= 31; k++) binary[k] = reference[j++] - '0'; 

    // rd
    for (int k = 24; k >= 20; k--) binary[k] = rd & 1, rd >>= 1; 

    // imme
    for (int k = 19; k >= 0; k--) binary[k] = imme & 1, imme >>= 1; 

    // Convert binary parts to strings
    opcodes = binaryToString(25, 31);
    func3s = "NULL";
    func7s = "NULL";
    rds = binaryToString(20, 24);
    rs1s = "NULL";
    rs2s = "NULL";
    immediates = binaryToString(0, 19);

    // Convert to hexadecimal and write pc & machine code to output.mc
    WriteToFile();
}

// To process UJ type Instructions
void UJ_Type(int index, int ManualIndex) {
    ll rd = 0, imme = 0;
    line = code[index];
    string label;
    bool isNegative;

    // Parse rd (destination register)
    size_t i = line.find("x") + 1;
    while (isdigit(line[i])) rd = rd * 10 + (line[i++] - '0');

    // Skip spaces and commas
    i = line.find_first_not_of(" ,", i);

    // Parse label
    while (i < line.size() && line[i] != ' ' && line[i] != '#') label += line[i++];
    imme = LabelOffset(label, index);
    if (imme < 0) imme = InvertDecimal(imme, 20);

    if (imme < 0) {
        isNegative = true;
        imme = InvertDecimal(-imme, 20);
    }
   
    vector<int> binconv;
    //convert label imm to binary
    if(isNegative)
        binconv = decimalToBinary(imme, 20);
    else
        binconv = decimalToBinary(imme/2, 20);
    reverse(binconv.begin(), binconv.end()); // Reverse the vector so that binconv[0] is LSB

    // Encode binary
    string reference = instructions_manual[ManualIndex];
    size_t j = reference.find(' ') + 1; // Skip to binary section

    // opcode (funct7)
    for (int l = 0; l < 7; l++) binary[25 + l] = reference[j++] - '0';
    j++;

    // rd
    for (int l = 0; l < 5; l++) binary[24 - l] = (rd >> l) & 1;

    
    // Immediate fields
    binary[0] = binconv[19]; // bit 20
    int z = 9;
    for (int k = 1; k <= 10; k++) binary[k] = binconv[z--];
    binary[11] = binconv[10]; // bit 11
    z = 18;
    for (int k = 12; k <= 19; k++) binary[k] = binconv[z--];
    
    // Convert binary parts to strings
    opcodes = binaryToString(25, 31);
    func3s = "NULL";
    func7s = "NULL";
    rds = binaryToString(20, 24);
    rs1s = "NULL";
    rs2s = "NULL";
    
    reverse(binconv.begin(), binconv.end()); // Reverse the vector so that binconv[0] is MSB
    immediates = binaryToString(0, 19, binconv);

    // Convert to hexadecimal and write pc & machine code to output.mc
    WriteToFile();
}

//To call respective type functions
void typenumber(string ins,int index,int ManualIndex)
{
	if(ins=="I")
		I_Type(index,ManualIndex);
	if(ins=="R")
		R_Type(index,ManualIndex);
	if(ins=="S")
		S_Type(index,ManualIndex);
    if(ins=="SB")
		SB_Type(index,ManualIndex);
    if(ins=="U")
		U_Type(index,ManualIndex);
	if(ins=="UJ")
		UJ_Type(index,ManualIndex);
}

//Extracts Instruction type and calls typenumber and respective type function for writing machine code
void Extract_type() {
    size = code.size();

    for (int i = 0; i < size; i++) {
        string ins;
        int j = 0;

        // Extract the instruction name (first word)
        while (j < code[i].size() && code[i][j] != ' ') {
            ins += code[i][j++];
        }

        // Handle label case (ending with ':')
        if (!ins.empty() && ins.back() == ':' && code[i].size() > ins.size()) {
            ins.clear();
            while (j < code[i].size() && code[i][j] == ' ') j++; // Skip spaces
            while (j < code[i].size() && code[i][j] != ' ') ins += code[i][j++];
        }

        // Match the instruction with known formats
        for (int k = 0; k < sizeIset; k++) {
            string type, type1;
            int k1 = 0;

            // Extract type from format
            while (instructions_manual[k][k1] != ' ') {
                type += instructions_manual[k][k1++];
            }

            if (ins == type) {
                // Extract type1 from the end
                k1 = instructions_manual[k].size() - 1;
                while (instructions_manual[k][k1] != ' ') {
                    type1 = instructions_manual[k][k1--] + type1; // Prepend to reverse while extracting
                }
                instype = type1;
                typenumber(type1, i, k);
                break;
            }
        }
    }
}

void printDataMemory(){
    ofstream file("output.mc", ios_base::app);
string s = "-------------------------------------------------------";
file << s << "\n";
//file<<"hello";
file<<"+3 +2 +1 +0"<<"\n";
for (int i = 0; i < 100; i += 4) {  // Increment by 4 after processing each group
    for (int j = 3; j >= 0; j--) {
        file << datamemory[i + j] << " ";  // Access the 4 elements in reverse
    }
    file << "\n";
}

file << s << "\n";

}

void printDataMemory2(){
    
    ofstream file("output.mc", ios_base::app);
    string s = "-------------------------------------------------------";
    file << s << "\n";
    //file<<"hello";
    string str;
    dataIndex[0] = DecimalToHex(268435456);
    int k = 0;
    for (int i = 0; i < 100; i += 4) {  // Increment by 4 after processing each group
        for (int j = 3; j >= 0; j--) {
            str = str + datamemory[i + j];  // Access the 4 elements in reverse
        }
        dataIndex[k+1] = DecimalToHex(268435456 + (i+4));
        datamemory2[k++] = str; 
        str = "";
        file<<"0x"<<dataIndex[k-1]<<" "<<"0x"<<datamemory2[k-1]<<"\n";
    }

    file << s << "\n";

}

int main(){

    for(int i=0;i<4000;i++)   //Initialize Data memory
		datamemory[i] = "00";

    DataSegment(); //Read Data segment and store it in memory (data)
   
    Instructions_Manual();  //Read Instruction set
    
    ofstream("output.mc").close();  // Clear the output file (creates and then closes it)
    
    TextSegment(); //Read Text segment and store it

    ExtractLabels(); //Extract all labels with their positions

    sp_to_x2(); //To replace all sp with x2 (as sp is alias for register x2)

    Extract_type(); //Extracts Instruction type and calls typenumber and respective type function for writing machine code

    //printDataMemory(); //Prints memory in Venus format (but no memory location)

    printDataMemory2(); //Prints memory in sir's format

    return 0;
}

