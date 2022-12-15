#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

//=====================================================================================================================================

#include "parser.h"
#include "stack.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//=====================================================================================================================================

const size_t STR_MAX_SIZE       = 20;
const size_t VERSION            = 1;
const size_t SIGNATURE          = 'NGIS';
const size_t MAX_RAM_SIZE       = 100;
const size_t MAX_LABEL_SIZE     = 20;
const size_t MAX_LABEL_COUNT    = 20;
const size_t MAX_REGS_COUNT     = 5;
const size_t LABEL_POISON       = 0xCAFEBABE;

//=====================================================================================================================================

struct label_t
{
    int adress;
    char* name;
};

struct infoCMD_t
{
    size_t sign;
    size_t vers;
    size_t size;
    size_t nInt;
    size_t nArgs;
};

struct asm_t
{
    label_t labels[MAX_LABEL_COUNT];
    infoCMD_t info;
    Text commands;
    char* asmArr;
};

struct cpu_t
{
    char* cmdArr;

    stack_t stk;

    int regs[MAX_REGS_COUNT];
    int RAM[MAX_RAM_SIZE];
};

//=====================================================================================================================================

enum ArgTypes
{
    ARG_IMMED = 1 << 5,
    ARG_REG   = 1 << 6,
    ARG_MEM   = 1 << 7,
    ONLY_CMD  = 31,
};

enum Commands
{
    CMD_HLT     = 0,
    CMD_PUSH    = 1,
    CMD_ADD     = 2,
    CMD_SUB     = 3,
    CMD_MUL     = 4,
    CMD_DIV     = 5,
    CMD_OUT     = 6,
    CMD_POP     = 7,

    CMD_JMP     = 10,
};    

enum Regs
{
    rax = 0xA,
    rbx = 0xB,
    rcx = 0xC,
    rdx = 0xD,
};

//=====================================================================================================================================

int     AsmCtor             (asm_t* asmCode, FILE* source);
int     AsmDtor             (asm_t* asmCode);
int     AsmCreateArray      (asm_t* asmCode);
int     WritingBinFile      (asm_t* asmCode, FILE* binaryFile);
int     CpuCtor             (cpu_t* cpu, FILE* binaryFile);
int     CpuDtor             (cpu_t* cpu);
int     Execute             (cpu_t* cpu);
int     Assemble            (int argc, const char* argv[]);
int     Run                 (int argc, const char* argv[]);
int     isRegister          (char* str);
int     isBrackets          (char* str);
int     ParseJumpArg        (char* line, asm_t* asmCode, size_t* ip);
void    ParseLabel          (char* cmd, asm_t* asmCode, size_t ip);
void    ParseArg            (char* line, int command, asm_t* asmCode, size_t* ip);
int     ParseCommonArg      (char* line, int command, asm_t* asmCode, size_t* ip);
int     ParseBracketsArg    (char* line, int command, asm_t* asmCode, size_t* ip);
int     GetPushArg          (int command, size_t* ip, cpu_t* cpu);
int*    GetPopArg           (int command, size_t* ip, cpu_t* cpu);
void    GetJumpArg          (size_t* ip, cpu_t* cpu);

//=====================================================================================================================================

#endif