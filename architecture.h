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

const size_t STR_MAX_SIZE   = 20;
const size_t VERSION        = 1;
const size_t SIGNATURE      = 'NGIS'; 

//=====================================================================================================================================

struct cmd_t
{
    size_t sign;
    size_t vers;
    size_t size;
    size_t nInt;
};

struct asm_t
{
    cmd_t info;

    Text commands;

    int* asmArr; 
};

//=====================================================================================================================================

enum Commands
{
    CMD_HLT     = 0,
    CMD_PUSH    = 1,
    CMD_ADD     = 2,
    CMD_SUB     = 3,
    CMD_MUL     = 4,
    CMD_DIV     = 5,
    CMD_OUT     = 6,
};    

//=====================================================================================================================================

int     AsmCtor         (asm_t* asmCode, FILE* source);
int     AsmDtor         (asm_t* asmCode);
int     AsmCreateArray  (asm_t* asmCode, FILE* processed);
int     WritingBinFile  (asm_t* asmCode, FILE* binaryFile);
int*    ReadingBinFile  (int* cmdArr, size_t* nMemb, FILE* binaryFile);
int     Execute         (int* cmdArr, size_t* nMemb);
int     Assemble        (int argc, const char* argv[]);
int     Run             (int argc, const char* argv[]);

//=====================================================================================================================================

#endif