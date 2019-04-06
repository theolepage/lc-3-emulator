#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

enum {
    OP_BR, OP_ADD, OP_LD, OP_ST, OP_JSR, OP_AND, OP_LDR, OP_STR,
    OP_RTI, OP_NOT, OP_STI, OP_JMP, OP_RES, OP_LEA, OP_TRAP
};

enum {
    R_R0, R_R1, R_R2, R_R3, R_R4, R_R5, R_R6, R_R7, R_PC, R_COND, R_COUNT
};

uint16_t memory[UINT16_MAX];
uint16_t registers[R_COUNT]; // R_COUNT = 11

uint16_t sign_extend(uint16_t x, int n)
{
    if ((x >> (n - 1)) & 1) {
        x |= (0xFFFF << n); // signed
    }
    return x; // unsigned
}

void update_cond(uint16_t r)
{
    if (registers[r] == 0)
    {
        registers[R_COND] = 1;
    }
    else if (registers[r] >> 15)
    {
        registers[R_COND] = 2;
    }
    else
    {
        registers[R_COND] = 4;
    }
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
    // int running = 1;
    // registers[R_PC] = 0x3000;
    // while (running)
    // {
    //     uint16_t instruction = memory[registers[R_PC]++];
    // }
    
    // Handle instruction
    uint16_t instruction = binToDec("0001010001000000"); //5184
    uint16_t operation = getOperationCode(instruction);
    uint16_t sr1, dr;
    switch(operation) {
        case OP_ADD:
            sr1 = getFirstOperand(instruction);
            dr = getDestination(instruction);
            if ((instruction >> 5) & 0x1)
            {
                registers[dr] = registers[sr1] + sign_extend(instruction & 0x1F, 5);
            }
            else
            {
                registers[dr] = registers[sr1] + registers[getSecondOperand(instruction)];
            }
            update_cond(dr);
            break;

        case OP_AND:
            sr1 = getFirstOperand(instruction);
            dr = getDestination(instruction);
            if ((instruction >> 5) & 0x1)
            {
                registers[dr] = registers[sr1] & sign_extend(instruction & 0x1F, 5);
            }
            else
            {
                registers[dr] = registers[sr1] & registers[getSecondOperand(instruction)];
            }
            update_cond(dr);
            break;

        case OP_NOT:
            sr1 = getFirstOperand(instruction);
            dr = getDestination(instruction);
            registers[dr] = ~registers[sr1];
            update_cond(dr);
            break;

        case OP_BR:
            // uint16_t offset = instruction & 0x1FF;
            // uint16_t n = (instruction >> 11) & 0x1;
            // uint16_t z = (instruction >> 10) & 0x1;
            // uint16_t p = (instruction >> 9) & 0x1;
            // if(registers[R_COND])
            // {
            //     registers[R_PC] += sign_extend(offset, 9);
            // }
            break;

        default:
            printf("Unhandled OP code.");
            abort();
            break;
    }

    return 0;
}