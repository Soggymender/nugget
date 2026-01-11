#include "assembler.h"

#include <stdio.h>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <string.h>
#include <iostream>
#include <sstream>
#include <bitset>

#include "vm.h"

using namespace std;

vector<string> registerNames
{
    "R1",
    "R2",
    "R3",
    "R4",
    "R5",
    "R6",
    "R7",
};

vector<string> opcodeNames
{
    "BR",   // 0000
    "ADD",  // 0001
    "LD",   // 0010
    "ST",   // 0011
    "JSR",  // 0100
    "JSRR", // 0100
    "AND",  // 0101
    "LDR",  // 0110
    "STR",  // 0111
    "RTI",  // 1000
    "NOT",  // 1001
    "LDI",  // 1010
    "STI",  // 1011
    
    "JMP",  // 1100
    "RET",  // 1100
            // 1101 reserved
    "LEA",  // 1110
    "TRAP", // 1111
};

vector<string> trapRoutines
{
    "GETC",     // x20
    "OUT",      // x21
    "PUTS",     // x22
    "IN",       // x23
    "PUTSP",    // x24
    "HALT",     // x25
};

vector<string> keywords
{
    "HALT",
    ".ORIG",
    ".FILL",
    ".BLKW",
    ".STRINGZ",
    ".END"
};

// Program variables.

vector<string> instLines;
unordered_map<int,Line> indexToLineMap;

unsigned short startingAddress = 0;
unsigned short curAddress = 0;


// Symbol table variables.

unordered_map<string, Line> symbolTable;

unordered_map<string, unsigned short> registerIndices;

class Opcode;
unordered_map<string, Opcode*> opcodes;

class Opcode
{
public:

    string name;
    unsigned short code;

    Opcode(const char* nameParam, unsigned short codeParam)
        : name(nameParam),
        code(codeParam)
    {
        opcodes[name] = this;
    }
};

unsigned short pc = 0;

void generateSymbolTable(const char* program, vector<Line>& programLines);

std::string unescape(const std::string& str) {

    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
            case 'e': result += '\e'; break;
            case 'n': result += '\n'; break;
            case 't': result += '\t'; break;
            case 'r': result += '\r'; break;
            case '\\': result += '\\'; break;
                // Add more cases as needed
            default:
                result += str[i];
                result += str[i + 1];
                break;
            }
            ++i;
        }
        else {
            result += str[i];
        }
    }
    return result;
}

vector<string> parseString(string& inputString)
{
    string curWord;

    bool inWord = false;
    bool inString = false;
    bool inComment = false;

    vector<string> result;// = new vector<string>(); // Vector to store the split substrings 

    for (char& c : inputString) {

        bool whitespace = (c == ' ' || c == '\t' || c == ',');
        bool newline = (c == '\n');

        if (!inWord && !inString && !inComment) {

            // Skip whitespace.
            if (whitespace || newline)
                continue;

            // If it's a quote, start a string.
            if (c == '"') {
                inString = true;
                continue;
            }
            
            // If it's a ; start a comment.
            if (c == ';') {
                inComment = true;
            }
            else {

                // Start word
                inWord = true;
            }
        }

        if (inWord) {

            // End word?
            if (whitespace || newline) {
                result.push_back(curWord);
                curWord.clear();
                inWord = false;
                continue;
            }

            curWord.push_back(c);
        }

        if (inString) {

            // End string?
            if (c == '"') {
                result.push_back(curWord);
                curWord.clear();
                inString = false;
                continue;
            }

            curWord.push_back(c);
        }

        if (inComment) {
            curWord.push_back(c);
        }
    }

    if (inWord || inString || inComment)
        result.push_back(curWord);

    return result;
}


vector<string> splitString(string& inputString, string& delimiters)
{
	vector<string> result; // Vector to store the split substrings 
	int startPos = 0;
	int endPos = 0;

	// Loop until endPos is not equal to string::npos 
	while ((endPos = (int)inputString.find_first_of(delimiters, startPos)) != string::npos) {

//		if (endPos != startPos) { // Checking if the 
			// substring is non-empty 
			result.push_back(inputString.substr(startPos, endPos - startPos));
//		}

		startPos = endPos + 1; // Update startPos to the position 
	}

	if (startPos != inputString.length()) {
		result.push_back(inputString.substr(startPos));
	}


	return result;
}

bool IsKeyword(string& word)
{
    // Check for registers.
    for (string& registerName : registerNames) {

        if (!word.compare(registerName))
            return true;
    }

    // Check for opcodes.
    for (string& opcodeName : opcodeNames) {

        if (!word.compare(opcodeName))
            return true;
    }

    // Check for TRAP routines.
    for (string& trapRoutine : trapRoutines) {

        if (!word.compare(trapRoutine))
            return true;
    }

    // Check for keywords.
    for (string& keyword : keywords) {

        if (!word.compare(keyword))
            return true;
    }

    // BR variants
    if (word.rfind("BR", 0) != string::npos)
        return true;

    return false;
}

void HandleOpcode(Line& line, unsigned short* image, unsigned short& imageSize, vector<Line> &programLines, unsigned short* testImage);
void HandleTrapRoutine(Line& line, unsigned short* image, unsigned short& imageSize, vector<Line>& programLines, unsigned short* testImage);
void HandleLabel(Line& line, unsigned short* image, unsigned short& imageSize, vector<Line>& programLines, unsigned short* testImage);
void HandleSpecial(Line& line, unsigned short* image, unsigned short& imageSize, vector<Line>& programLines, unsigned short* testImage);


void vm_assemble(const char* program, unsigned short* image, unsigned short& imageSize, unsigned short* testImage)
{
    std::vector<Line> programLines;

    // Clear the image buffer.
    memset(image, 0, sizeof(image));

	generateSymbolTable(program, programLines);

    // Setup register index look-up.
    registerIndices["R0"] = 0;
    registerIndices["R1"] = 1;
    registerIndices["R2"] = 2;
    registerIndices["R3"] = 3;
    registerIndices["R4"] = 4;
    registerIndices["R5"] = 5;
    registerIndices["R6"] = 6;
    registerIndices["R7"] = 7;
    registerIndices["R8"] = 8;

    // Setup opcode look-up.
    Opcode opBr("BR",       0b0000);
    Opcode opAdd("ADD",     0b0001);
    Opcode opLd("LD",       0b0010);
    Opcode opSt("ST",       0b0011);
    Opcode opJsr("JSR",     0b0100);
    Opcode opJsrr("JSRR",   0b0100);
    Opcode opAnd("AND",     0b0101);
    Opcode opLdr("LDR",     0b0110);
    Opcode opStr("STR",     0b0111);
    Opcode opRti("RTI",     0b1000);
    Opcode opNot("NOT",     0b1001);
    Opcode opLdi("LDI",     0b1010);
    Opcode opSti("STI",     0b1011);
    Opcode opJmp("JMP",     0b1100);
    Opcode opRet("RET",     0b1100);
                         // 0b1101 reserved
    Opcode opLea("LEA",     0b1110);
    Opcode opTrap("TRAP",   0b1111);
 
    image[imageSize++] = startingAddress;

    pc = startingAddress;

//    cout << "\n\n\n";

    for (Line& line : programLines)
    {
        cout << line.lineNumber << ": " << line.sentence << "\n";

        int startingImageSize = imageSize;

        HandleOpcode(line, image, imageSize, programLines, testImage);
        HandleTrapRoutine(line, image, imageSize, programLines, testImage);
        HandleLabel(line, image, imageSize, programLines, testImage);
        HandleSpecial(line, image, imageSize, programLines, testImage);

        if (imageSize > startingImageSize) {

            // Map the imageSize (which is also idx) to the line.
            indexToLineMap[startingImageSize] = line;

            pc++;
        }
    }

    /*
    for (int i = 0; i < imageSize; i++) {

        image[i] = swap16(image[i]);
    }
    */
}

static unsigned short zext(unsigned short x, int bit_count) {

//    if (x & 0x8000)
//        x |= (0x1 << bit_count) - 1;

    return x;
}

static unsigned short GetValue(string& valString, int bit_count) {

    char formatChar = valString[0];
    string trimmed;

    unsigned short val = 0;

    if (formatChar == '#') {

        if (valString[1] == '+')
            trimmed = valString.substr(2);
        else
            trimmed = valString.substr(1);

        val = stoi(trimmed);
    }
    else if (formatChar == 'x') {

        if (valString[1] == '+')
            trimmed = valString.substr(2);
        else
            trimmed = valString.substr(1);

        val = stoi(trimmed, nullptr, 16);
    }

    return zext(val, 5);
}


void HandleOpcode(Line& line, unsigned short* image, unsigned short& imageSize, vector<Line>& programLines, unsigned short* testImage)
{
    //if (!line.words[0].compare("BR")) {
    if (line.words[0].rfind("BR", 0) != string::npos) {

        unsigned int n = 0;
        unsigned int z = 0;
        unsigned int p = 0;

        for (char& c : line.words[0]) {

            n = n | (c == 'n' ? 1 : 0);
            p = p | (c == 'p' ? 1 : 0);
            z = z | (c == 'z' ? 1 : 0);
        }

        unsigned short pcOffset = 0;
        string valString = line.words[1].substr(1);
        if (line.words[1][0] == '#') {
            pcOffset = stoi(valString);
        }
        else if (line.words[1][0] == 'x') {
            pcOffset = stoi(valString, nullptr, 16);
        }
        else {
            unsigned short address = symbolTable[line.words[1]].address;
            pcOffset = address - (pc + 1);
        }

        unsigned short pcOffset9 = pcOffset;// zext(pcOffset, 9);

        // Encode.
        image[imageSize++] = (0b0000 << 12) | (n << 11) | (z << 10) | (p << 9) | (0x1FF & pcOffset9);
    } 
    else if (!line.words[0].compare("ADD")) {

        unsigned short dr = registerIndices[line.words[1]];
        unsigned short sr1 = registerIndices[line.words[2]];

        bool flag = false;
        unsigned short sr2 = 0;
        unsigned short imm5 = 0;
        if (registerIndices.find(line.words[3]) != registerIndices.end()) {
            sr2 = registerIndices[line.words[3]];
        }
        else {
        
            imm5 = GetValue(line.words[3], 5);
            flag = true;
        }

        // Encode.
        image[imageSize] = (0b0001 << 12) | (dr << 9) | (sr1 << 6);
        if (flag)
            image[imageSize] |= (1 << 5) | (imm5 & 0x1F);
        else
            image[imageSize] |= (0b00 << 3) | (sr2 & 0x7);

        imageSize++;

    }
    else if (!line.words[0].compare("LD")) {

        unsigned short dr = registerIndices[line.words[1]];
        
        unsigned short pcOffset = 0;
        string valString = line.words[2].substr(1);
        if (line.words[2][0] == '#') {
            pcOffset = stoi(valString);
        }
        else if (line.words[2][0] == 'x') {
            pcOffset = stoi(valString, nullptr, 16);
        }
        else {
            unsigned short address = symbolTable[line.words[2]].address;
            pcOffset = address - (pc + 1);
        }
        
        unsigned short pcOffset9 = zext(pcOffset, 9);

        // Encode.
        image[imageSize++] = (0b0010 << 12) | (dr << 9) | (0x1FF & pcOffset9);
    
    } else if (!line.words[0].compare("ST")) {

        unsigned short sr = registerIndices[line.words[1]];
        unsigned short address = symbolTable[line.words[2]].address;
        unsigned short pcOffset = address - (pc + 1);
        unsigned short pcOffset9 = zext(pcOffset, 9);

        // Encode.
        image[imageSize++] = (0b0011 << 12) | (sr << 9) | (0x1FF & pcOffset9);

    }
    else if (!line.words[0].compare("JSR")) {

        unsigned short address = symbolTable[line.words[1]].address;
        unsigned short pcOffset = address - (pc + 1);
        unsigned short pcOffset11 = zext(pcOffset, 11);

        // Encode.
        image[imageSize++] = (0b0100 << 12) | (0b1 << 11) | (0x7FF & pcOffset11);
    }
    else if (!line.words[0].compare("JSSR")) {
    
        image[imageSize++];

    } else if (!line.words[0].compare("AND")) {

        unsigned short dr = registerIndices[line.words[1]];
        unsigned short sr1 = registerIndices[line.words[2]];

        bool flag = false;
        unsigned short sr2 = 0;
        unsigned short imm5 = 0;
        if (registerIndices.find(line.words[3]) != registerIndices.end()) {
            sr2 = registerIndices[line.words[3]];
        }
        else {
            imm5 = zext(symbolTable[line.words[3]].address, 5);
            flag = true;
        }

        // Encode.
        image[imageSize] = (0b0101 << 12) | (dr << 9) | (sr1 << 6);
    
        if (flag)
            image[imageSize] |= (1 << 5) | imm5 & 0x1F;
        else
            image[imageSize] |= sr2 & 0x7;

        imageSize++;

    }
    else if (!line.words[0].compare("LDR")) {

        unsigned short dr = registerIndices[line.words[1]];
        unsigned short baseR = registerIndices[line.words[2]];

        unsigned short pcOffset = 0;
        string valString = line.words[3].substr(1);
        if (line.words[3][0] == '#') {
            pcOffset = stoi(valString);
        }
        else if (line.words[3][0] == 'x') {
            pcOffset = stoi(valString, nullptr, 16);
        }
        else {
            //assert(false);
        }

        unsigned short pcOffset6 = zext(pcOffset, 6);

        // Encode.
        image[imageSize++] = (0b0110 << 12) | ((0x7 & dr) << 9) | ((0x7 & baseR) << 6) | (0x3F & pcOffset6);
    }
    else if (!line.words[0].compare("STR")) {

        unsigned short sr = registerIndices[line.words[1]];
        unsigned short baseR = registerIndices[line.words[2]];

        unsigned short pcOffset = 0;
        string valString = line.words[3].substr(1);
        if (line.words[3][0] == '#') {
            pcOffset = stoi(valString);
        }
        else if (line.words[3][0] == 'x') {
            pcOffset = stoi(valString, nullptr, 16);
        }
        else {
            //assert(false);
        }

        unsigned short pcOffset6 = zext(pcOffset, 6);

        // Encode.
        image[imageSize++] = (0b0111 << 12) | ((0x7 & sr) << 9) | ((0x7 & baseR) << 6) | (0x3F & pcOffset6);
    }
    else if (!line.words[0].compare("RTI")) {
        image[imageSize++];
    }
    else if (!line.words[0].compare("NOT")) {

        unsigned short dr = registerIndices[line.words[1]];
        unsigned short sr = registerIndices[line.words[2]];

        // Encode.
        image[imageSize++] = (0b1001 << 12) | (dr << 9) | (sr << 6) | (1 << 5) | 0b11111;

    }
    else if (!line.words[0].compare("LDI")) {

        unsigned short dr = registerIndices[line.words[1]];
        unsigned short address = symbolTable[line.words[2]].address;
        unsigned short pcOffset = address - (pc + 1);
        unsigned short pcOffset9 = zext(pcOffset, 9);

        // Encode.
        image[imageSize++] = (0b1010 << 12) | (dr << 9) | (0x1FF & pcOffset9);
    }
    else if (!line.words[0].compare("STI")) {

        unsigned short sr = registerIndices[line.words[1]];

        Line& otherLine = symbolTable[line.words[2]];
        unsigned short pcOffset = otherLine.address - (pc + 1);
        unsigned short pcOffset9 = zext(pcOffset, 9);

        // Encode.
        image[imageSize++] = (0b1011 << 12) | (sr << 9) | (0x1FF & pcOffset9);
    }
    else if (!line.words[0].compare("JMP")) {
    
        // Extract bits 8:6 into base register
        // Instruction pointer = base register

        /* This is just a copy paste. Incorrect parsing.
        unsigned short sr = registerIndices[line.words[2]];

        // Encode.
        image[imageSize++] = (0b1001 << 12) | (dr << 9) | (sr << 6) | (1 << 5) | 0b11111;
        */

        image[imageSize++];
    }
    else if (!line.words[0].compare("RET")) {

        image[imageSize++] = 0b1100000111000000;
    }
    else if (!line.words[0].compare("LEA")) {

        unsigned short dr = registerIndices[line.words[1]];
        unsigned short address = symbolTable[line.words[2]].address;
        unsigned short pcOffset = address - (pc + 1);
        unsigned short pcOffset9 = zext(pcOffset, 9);

        // Encode.
        image[imageSize++] = (0b1110 << 12) | (dr << 9) | (0x1FF & pcOffset9);
    }
    else if (!line.words[0].compare("TRAP")) {

        /*
        "GETC",     // x20
        "OUT",      // x21
        "PUTS",     // x22
        "IN",       // x23
        "PUTSP",    // x24
        "HALT",     // x25
        */

        unsigned short trapCode = 0;
        string valString = line.words[1].substr(1);
        if (line.words[1][0] == 'x') {
            trapCode = stoi(valString, nullptr, 16);
        }

        image[imageSize++] = (0b1111 << 12) | (trapCode & 0xFF);
    }
}

void HandleTrapRoutine(Line& line, unsigned short* image, unsigned short& imageSize, vector<Line>& programLines, unsigned short* testImage)
{
    // Trap codes can specify opcode TRAP and be handled as an opcode, or
    // use the keyword directly.
    if (!line.words[0].compare("GETC")) {
        image[imageSize++] = (0b1111 << 12) | (0x20 & 0xFF);
    } else if (!line.words[0].compare("OUT")) {
        image[imageSize++] = (0b1111 << 12) | (0x21 & 0xFF);
    }
    else if (!line.words[0].compare("PUTS")) {
        image[imageSize++] = (0b1111 << 12) | (0x22 & 0xFF);
    }
    else if (!line.words[0].compare("IN")) {
        image[imageSize++] = (0b1111 << 12) | (0x23 & 0xFF);
    }
    else if (!line.words[0].compare("PUTSP")) {
        image[imageSize++] = (0b1111 << 12) | (0x24 & 0xFF);
    }
    else if (!line.words[0].compare("HALT")) {
        image[imageSize++] = (0b1111 << 12) | (0x25 & 0xFF);
    }
}

void HandleLabel(Line& line, unsigned short* image, unsigned short& imageSize, vector<Line>& programLines, unsigned short* testImage)
{
    if (symbolTable.find(line.words[0]) != symbolTable.end()) {

        unsigned short val = 0;

        if (line.numInstWords == 1) {
            //image[imageSize++] = 0; // What to do with a label occupying its own line?
            
            // If a label is on its own line, make its address the next address.
            line.address++;

        } else {
            if (line.words[2].rfind("#", 0) == 0) {
                string valString = line.words[2].substr(1);
                val = stoi(valString, nullptr, 10);
            }
            else if (line.words[2].rfind("x", 0) == 0) {
                string valString = line.words[2].substr(1);
                val = stoi(valString, nullptr, 16);
            }
            else {
                // Is it a label?
                if (symbolTable.find(line.words[2]) != symbolTable.end()) {
                    val = symbolTable[line.words[2]].address;
                }
            }

            if (!line.words[1].compare(".FILL")) {
                image[imageSize++] = val;
            }
            else if (!line.words[1].compare(".BLKW")) {
                for (int i = 0; i < line.numInstWords; i++) {
                    image[imageSize++] = 0;
                }
            }
            else if (!line.words[1].compare(".STRINGZ")) {
                // Strings are apparently stored with one char per 16 bit word...
                for (char& c : line.words[2]) {
                    image[imageSize++] = c;
                    pc++;
                }
                image[imageSize++] = 0;
                // Do not increment PC for the terminating zero. The caller will do it.
            }
        }
    }
}

void HandleSpecial(Line& line, unsigned short* image, unsigned short& imageSize, vector<Line>& programLines, unsigned short* testImage)
{
    // For now this specifically exists to handle dangling .FILL commands.
    // We'll see what other special circumstances arise.

    if (line.numInstWords == 2) {

        unsigned short val = 0;

        if (!line.words[0].compare(".FILL")) {

            if (line.words[1].rfind("#", 0) == 0) {
                string valString = line.words[1].substr(1);
                val = stoi(valString, nullptr, 10);
            }
            else if (line.words[1].rfind("x", 0) == 0) {
                string valString = line.words[1].substr(1);
                val = stoi(valString, nullptr, 16);
            }
            else {
                val = symbolTable[line.words[1]].address;
            }

            image[imageSize++] = val;
        }
    }
}

// Reassemble a string from individual words surrounded by quotes.
// Usually in context of a variable definition.

void BuildString(vector<string> &words)
{
    for (int i = 0; i < words.size(); i++)
    {
        if (words[i].rfind('\"', 0) == 0) {
            
            int start = i;
            std::string newString;

            words[i] = words[i].substr(1);

            do {
                if (i > start)
                    newString += " ";

                newString += words[i];
                i++;

                if (i >= words.size())
                    break;

            } while (words[i].rfind('\"', words[i].length() - 1) != 0);

            words[start] = newString;
            words.erase(words.begin() + start + 1, words.begin() + i);
        }
    }}

int CountInstWords(vector<string> words)
{
    for (int i = 0; i < words.size(); i++)
    {
        if (words[i].rfind(";", 0) == 0) {
            return i;
        }
    }

    return (int)words.size();
}
    
void generateSymbolTable(const char* program, vector<Line>& programLines)
{
    string newline = "\n";
    string whitespace = " \t,";

    string programString = program;
    instLines = splitString(programString, newline);
    int curLine = 1;

    bool increment = true;

    for (string instLine : instLines) {
    
        Line line;
        line.lineNumber = curLine++;
        line.sentence = instLine;
        
        if (line.sentence.length() == 0) {
            continue;
        }

//        cout << curLine << ": " << instLine << "\n";
//        line.words = splitString(instLine, whitespace);
//        BuildString(line.words);

        instLine = unescape(instLine); // Remove double slash within quotes, including newline.
        line.words = parseString(instLine);


        line.numInstWords = CountInstWords(line.words);


        if (line.words[0][0] == ';') {

            programLines.push_back(line);
            continue;
        }

		if (line.words[0] == ".ORIG") {
			string addressString = line.words[1].substr(1);
			startingAddress = stoi(addressString, 0, 16);
			curAddress = startingAddress;
			continue;
		}

		line.address = curAddress;
        if (increment) {
            curAddress++;
        }
        else
            increment = true;

        if (!IsKeyword(line.words[0])) {

            // This is a label.
            
            if (line.numInstWords == 1)
                increment = false;

            if (line.numInstWords > 1) {

                // If .BLKW is specified it will allocate n words.
                // and the instruction pointer will move forward that amount.
                if (!line.words[1].compare(".BLKW")) {
                    unsigned short n = 0;
                    n = stoi(line.words[2]);

                    line.numWords = n;
   
                    curAddress += n - 1; // (curAddress was already incremented)
                }

                // If .STRINGZ is specified it will allocate length words.
                // and the instruction pointer will move forward that amount.
                if (!line.words[1].compare(".STRINGZ")) {

                    int length = (int)line.words[2].length();
                    curAddress += length;// -1;
                }
            }

            symbolTable[line.words[0]] = line;
        }

        programLines.push_back(line);
    }
}