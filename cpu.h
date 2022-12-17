#ifndef CPU_H
#define CPU_H

//=====================================================================================================================================

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "parser.h"
#include "stack.h"

//=====================================================================================================================================

const size_t SIGNATURE          = 256 * 256 * 'N' + 256 * 'G' + 'S';
const size_t VERSION            = 1;
const size_t STR_MAX_SIZE       = 30;
const size_t MAX_RAM_SIZE       = 100;
const size_t MAX_LABEL_SIZE     = 20;
const size_t MAX_LABEL_COUNT    = 20;
const size_t MAX_REGS_COUNT     = 10;
const size_t LABEL_POISON       = 0xCAFEBABE;

//=====================================================================================================================================

struct label_t
{
    size_t adress;
    char* name;
};

struct infoCMD_t
{
    size_t sign;
    size_t vers;
    size_t size;
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

    size_t size;

    int regs[MAX_REGS_COUNT];
    int RAM[MAX_RAM_SIZE];
};

//=====================================================================================================================================

#define DEF_CMD(name, num, arg, code) \
    CMD_##name = num,

#define DEF_JMP(name, num, sign)      \
    JMP_##name = num,

enum Commands
{
    #include "architecture.h" 
};

#undef DEF_CMD
#undef DEF_JMP

//=====================================================================================================================================

enum ArgTypes
{
    ARG_IMMED = 1 << 5,
    ARG_REG   = 1 << 6,
    ARG_MEM   = 1 << 7,
    ONLY_CMD  = 31,
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
int     ParseJumpArg        (char* line, int command, asm_t* asmCode, size_t* ip);
int     ParseLabel          (char* cmd, asm_t* asmCode, size_t ip);
int     ParseArg            (char* line, int command, asm_t* asmCode, size_t* ip);
int     ParseCommonArg      (char* line, int command, asm_t* asmCode, size_t* ip);
int     ParseBracketsArg    (char* line, int command, asm_t* asmCode, size_t* ip);
int     GetPushArg          (int command, size_t* ip, cpu_t* cpu);
int*    GetPopArg           (int command, size_t* ip, cpu_t* cpu);
int     GetJumpArg          (size_t* ip, cpu_t* cpu);
int     RunDisasm           (int argc, const char* argv[]);
char*   ReadBin             (FILE* binaryFile);
int     ExecuteDisasm       (char* cmdArr, FILE* disasmFile);
int     DisasmArg           (int curCmd, char* cmdArr, size_t* ip, FILE* disasmFile);

//=====================================================================================================================================

#endif