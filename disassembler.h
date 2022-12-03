#ifndef DISASSEMBLER_H_
#define DISASSEMBLER_H_

//=====================================================================================================================================

#include "cpu.h"

//=====================================================================================================================================

struct disasm_t
{
    int*        asm_code;
    size_t      ip;
    size_t      size;
    const char* signature;
    char*       buf_code;
    size_t      cursor;
};

//=====================================================================================================================================

const size_t MAX_LEN_REG_NAME = 5;

//=====================================================================================================================================

int     MakeReadableCode    ();
int     PutCmd              (disasm_t* disasm, const char* cmdName);
int     HandleRegsDisasm    (disasm_t* disasm, size_t regNum);
int     PutStackCmd         (disasm_t* disasm);
int     Disassemble         (disasm_t* disasm);
int     WriteUserCode       (disasm_t* disasm, const char* fileName);
int     PutNumToCharBuf     (disasm_t* disasm);
int     checkSignDisasm     (disasm_t* disasm, FILE* fileASM);
void    BufCtor             (disasm_t* disasm);
int     getCodeForDisasm    (disasm_t* disasm);

//=====================================================================================================================================

#endif

