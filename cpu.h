#ifndef CPU_H_
#define CPU_H_

//=====================================================================================================================================

#include <string.h>
#include <sys\stat.h>
#include <ctype.h>
#include <stdio.h>
#include <math.h>

#include "stack.h"
#include "config.h"
#include "assembler.h"

//=====================================================================================================================================

#define RUN_MODE

#define DEF_CMD(name, num, arg, ...) \
    case num:                        \
    __VA_ARGS__                      \
    break;

#define RETURN_PTR_ARG(arg)          \
{                                    \
    cpu->ip--;                       \
                                     \
    return arg;                      \
}

//=====================================================================================================================================

const int       CMD_MASK        =   31;
const int       Accuracy        = 1000;

//=====================================================================================================================================

enum CPU_ERRORS
{
    WRONG_SIGNATURE   = 1,
    UNDEFINED_CMD     = 2,
    ZERO_DIVISION     = 3,
    INVALID_REG_INDEX = 4,
    INVALID_MEM_INDEX = 5
};

enum PrintRAMFormats
{
    BIN_FORMAT = 0,
    SYM_FORMAT = 1,
    INT_FORMAT = 2
};


struct cpu_t
{
    int*        code;
    int*        regs;
    int*        RAM;

    const char* signature;

    size_t      ip;
    size_t      size;

    stack_t*    stk;
    stack_t*    stkCalls;
};

//=====================================================================================================================================

int     Execute         (cpu_t* cpu);
void    PrintRAM        (size_t format, cpu_t* cpu, size_t len_line);
int*    GetArg          (cpu_t* cpu);
void    PushArg         (cpu_t* cpu);
int     PopArg          (cpu_t* cpu);
void    CpuCtor         (cpu_t* cpu);
void    CPUDtor         (cpu_t* cpu);
int     checkSign       (cpu_t* cpu, FILE* fileASM);
int     getCode         (cpu_t* cpu);
void    SkipNewLines    ();
int     Run             ();

//=====================================================================================================================================

#endif
