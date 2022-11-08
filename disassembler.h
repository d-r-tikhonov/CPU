#ifndef DISASSEMBLER_H_
#define DISASSEMBLER_H_

//=====================================================================================================================================

#include "cpu.h"

//=====================================================================================================================================

typedef struct Disasm_struct
{
    int*        asm_code;
    size_t      ip;
    size_t      Size;
    const char* signature;
    char*       buf_code;
    size_t      cursor;
} disasm_t;

//=====================================================================================================================================

const size_t MAX_LEN_REG_NAME = 5;

//=====================================================================================================================================

int     MakeReadableCode    ();
int     PutCmd              (disasm_t* disasm, const char* cmd_name);
int     HandleRegsDisasm    (disasm_t* disasm, size_t reg_num);
int     PutStackCmd         (disasm_t* disasm);
int     Disassemble         (disasm_t* disasm);
int     WriteUserCode       (disasm_t* disasm, const char* filename);
int     PutNumToCharBuf     (disasm_t* disasm);
int     checkSignDisasm     (disasm_t* disasm, FILE* file_asm);
void    BufCtor             (disasm_t* disasm);
int     getCodeForDisasm    (disasm_t* disasm);

//=====================================================================================================================================

#endif

