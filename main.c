#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

enum {
    OP_BR, OP_ADD, OP_LD, OP_ST, OP_JSR, OP_AND, OP_LDR, OP_STR,
    OP_RTI, OP_NOT, OP_LDI, OP_STI, OP_JMP, OP_RES, OP_LEA, OP_TRAP
};

enum {
    R_R0, R_R1, R_R2, R_R3, R_R4, R_R5, R_R6, R_R7, R_PC, R_COND, R_COUNT
};

enum {
    TRAP_GETC = 0x20,    /* Get character from keyboard */
    TRAP_OUT = 0x21,     /* Output a character */
    TRAP_PUTS = 0x22,    /* Output a string */
    TRAP_IN = 0x23,      /* Input a string */
    TRAP_PUTSP = 0x24,   /* Output a string with two character in each memory location [15:8] [7:0] */
    TRAP_HALT = 0x25     /* Halt the program */
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

uint16_t zero_extend(uint16_t x, int n)
{
    return x;
}

void update_cond(uint16_t r)
{
    if (registers[r] == 0)
    {
        registers[R_COND] = 1; // nzp = 001
    }
    else if (registers[r] >> 15)
    {
        registers[R_COND] = 2; // nzp = 010
    }
    else
    {
        registers[R_COND] = 4; // nzp = 100
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
    uint16_t sr1, sr2, dr, offset6, offset9, offset11, nzp, baser;
    switch(operation) {
        case OP_BR:
            offset9 = instruction & 0x1FF;
            nzp = (instruction >> 9) & 0x7;

            if(registers[R_COND] & nzp)
            {
                registers[R_PC] += sign_extend(offset9, 9);
            }
            break;

        case OP_ADD:
            sr1 = getFirstOperand(instruction);
            sr2 = getSecondOperand(instruction);
            dr = getDestination(instruction);

            if ((instruction >> 5) & 0x1)
            {
                registers[dr] = registers[sr1] + sign_extend(instruction & 0x1F, 5);
            }
            else
            {
                registers[dr] = registers[sr1] + registers[sr2];
            }
            update_cond(dr);
            break;

        case OP_LD:
            offset9 = instruction & 0x1FF;
            dr = getDestination(instruction);

            memory[dr] = memory[registers[R_PC] + sign_extend(offset9, 9)];
            update_cond(dr);
            break;

        case OP_ST:
            offset9 = instruction & 0x1FF;
            dr = getDestination(instruction);

            memory[registers[R_PC] + sign_extend(offset9, 9)] = memory[dr];
            update_cond(dr);
            break;

        case OP_JSR:
            offset11 = instruction & 0x7FF;
            baser = (instruction >> 6) & 0x7;

            registers[R_R7] = registers[R_PC];
            if ((instruction >> 11) & 0x1) // JSRR
            {
                registers[R_PC] = baser;
            }
            else // JSR
            {
                registers[R_PC] += sign_extend(offset11, 11);
            }
            break;

        case OP_AND:
            sr1 = getFirstOperand(instruction);
            sr2 = getSecondOperand(instruction);
            dr = getDestination(instruction);

            if ((instruction >> 5) & 0x1)
            {
                registers[dr] = registers[sr1] & sign_extend(instruction & 0x1F, 5);
            }
            else
            {
                registers[dr] = registers[sr1] & registers[sr2];
            }
            update_cond(dr);
            break;

        case OP_LDR:
            dr = getDestination(instruction);
            baser = (instruction >> 6) & 0x7;
            offset6 = instruction & 0x3F;

            memory[dr] = memory[baser + sign_extend(offset6, 6)];
            update_cond(dr);
            break;

        case OP_STR:
            sr1 = getDestination(instruction);
            baser = (instruction >> 6) & 0x7;
            offset6 = instruction & 0x3F;

            memory[baser + sign_extend(offset6, 6)] = sr1;
            break;

        case OP_RTI:;

        case OP_NOT:
            sr1 = getFirstOperand(instruction);
            dr = getDestination(instruction);

            registers[dr] = ~registers[sr1];
            update_cond(dr);
            break;

        case OP_LDI:
            dr = getDestination(instruction);
            offset9 = instruction & 0x1FF;

            memory[dr] = memory[memory[registers[R_PC] + sign_extend(offset9, 9)]];
            update_cond(dr);
            break;

        case OP_STI:
            sr1 = getDestination(instruction);
            offset9 = instruction & 0x1FF;

            memory[memory[registers[R_PC] + sign_extend(offset9, 9)]] = sr1;
            break;

        case OP_JMP:
            baser = (instruction >> 6) & 0x7;

            registers[R_PC] = baser;
            break;

        case OP_RES:
            // reserved, throw illegal opcode exception
            break;

        case OP_LEA:
            dr = getDestination(instruction);
            offset9 = instruction & 0x1FF;

            memory[dr] = registers[PC] + sign_extend(offset9, 9);
            update_cond(dr);
            break;

        case OP_TRAP:;
            uint16_t trapvect8 = 0;

            registers[R_R7] = registers[R_PC];
            registers[R_PC] = memory[trapvect8];
            break;

        default:
            printf("Unhandled OP code.");
            abort();
            break;
    }

    return 0;
}