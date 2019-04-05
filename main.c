#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

enum operations {
    OP_BR, OP_ADD, OP_LD, OP_ST, OP_JSR, OP_AND, OP_LDR, OP_STR,
    OP_RTI, OP_NOT, OP_STI, OP_JMP, OP_RES, OP_LEA, OP_TRAP
};

enum registers {
    R_R0, R_R1, R_R2, R_R3, R_R4, R_R5, R_R6, R_R7
};

uint16_t memory[UINT16_MAX];
uint16_t registers[8];

uint16_t sign_extend(uint16_t x, int n)
{
    if ((x >> (n - 1)) & 1) {
        x |= (0xffff << n); // signed
    }
    return x; // unsigned
}

// Functions to parse an instruction 
uint16_t getOperationCode(uint16_t instruction) { return instruction >> 12; }
uint16_t getFirstOperand(uint16_t instruction) { return (instruction >> 6) & 0x7; }
uint16_t getSecondOperand(uint16_t instruction) { return instruction & 0x7; }
uint16_t getDestination(uint16_t instruction) { return (instruction >> 9) & 0x7; }

uint16_t binToDec(char* bin) {
    int a = 1;
    int dec = 0;
    for (int i = 15; i > -1; i--)
    {
        if (bin[i] == '1') { dec += a; }
        a *= 2;
    }
    return dec;
}

int main()
{
    /*
    // Open input file
    FILE *fp = fopen("./input", "r");
    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    // Read input file
    char ch;
    int i = 0;
    while((ch = fgetc(fp)) != EOF)
    {
        instruction[i] = ch;
        i++;
    }*/
    
    // Handle instruction
    uint16_t instruction = binToDec("0001010001000000"); //5184
    uint16_t operation = getOperationCode(instruction);
    switch(operation) {
        case OP_ADD:;
            uint16_t sr1 = getFirstOperand(instruction);
            uint16_t dr = getDestination(instruction);
            if ((instruction >> 5) & 0x1) // Handle immediate values
            {
                registers[dr] = registers[sr1] + sign_extend(instruction & 0x1F, 5);
            }
            else
            {
                registers[dr] = registers[sr1] + registers[getSecondOperand(instruction)];
            }
            break;

        default:
            printf("Unhandled OP code.");
            abort();
            break;
    }

    // Close input file
    // fclose(fp);

    return 0;
}