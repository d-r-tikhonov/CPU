#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

//=====================================================================================================================================

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

#include "errors.h"
#include "config.h"

//=====================================================================================================================================

const size_t MAX_LEN_LABEL_NAME = 30;
const size_t MAX_LEN_MASK       = 15;
const int    MAX_LEN_LINE       = 30;

//=====================================================================================================================================

typedef struct LABEL
{
    int   Value;
    char  Name[MAX_LEN_LABEL_NAME];
} label_field;

typedef struct ASM_CODE
{
    int*         Ptr;
    size_t       Ip;
    size_t       Size;
    label_field* Labels;
} asm_t;

enum CMD_NAMES
{
    CMD_HLT  = 0,
    CMD_PUSH = 1,
    CMD_POP  = 2,
    CMD_ADD  = 3,
    CMD_SUB  = 4,
    CMD_MUL  = 5,
    CMD_DIV  = 6,
    CMD_IN   = 7,
    CMD_OUT  = 8,
    CMD_JUMP = 9,
    CMD_JB   = 10,
    CMD_JBE  = 11,
    CMD_JA   = 12,
    CMD_JAE  = 13,
    CMD_JE   = 14,
    CMD_JNE  = 15,
    CMD_NAME = 16,
    CMD_LAB  = 17,
    CMD_DRAW = 18
};

//=====================================================================================================================================

typedef struct prop_line
{
    char* Loc;
    size_t Len;
}type_prop_line;

typedef struct buffer_char
{
    char* Ptr;
    size_t Size;
    unsigned long int Num_lines;
}type_buf_char;

typedef struct buffer_ptrs
{
    type_prop_line** Ptr;
    size_t Size;
}type_buf_ptrs;

typedef struct buffer_structs
{
    type_prop_line* Ptr;
    size_t Size;
}type_buf_structs;

//=====================================================================================================================================

int     PrintASM                (asm_t* asm_code);
int     SkipSpace               (char** cursor);
int     HandleNum               (asm_t* asm_code, size_t cmd_code, char* ptr_arg, const char* mask1, const char* ram_mask1);
int     HandleReg               (asm_t* asm_code, size_t cmd_code, char* ptr_arg, const char* mask1, const char* ram_mask1);
elem_t  PutArg                  (size_t cmd_code, char* ptr_arg, asm_t* asm_code);
int     HandleRegname           (asm_t* asm_code, char* reg_name);
int     SearchLabelByName       (label_field* labels, char* name);
int     UserCodeToASM           (type_buf_char* ptr_user_code, type_buf_structs* ptr_arr_structs, asm_t* asm_code);
int     FuncName                (char* ptr_arg, asm_t* asm_code);
int     FuncJump                (char* ptr_arg, asm_t* asm_code);
int     FuncLab                 (char* ptr_arg, asm_t* asm_code);
size_t  IdentifyNumLabel        (char* ptr_arg, asm_t* asm_code);
int     WriteASM                (int* ptr_asm, const char* filename, size_t buf_size);
int     WriteHead               (FILE* file, size_t buf_size);
int     PutBuffer               (FILE* w_file, int* ptr_asm, size_t buf_size);
size_t  ReadVersion             (const char* filename);
int     UpdateVersion           (const char* filename, size_t* ptr_version);
int     WriteVersion            (FILE* file, size_t version);
int     HandleCmdArgs           (int argc, char** argv);
int     Assemble                (int argc, char** argv);
int     HandleRegAndNum         (asm_t* asm_code, size_t cmd_code, char* ptr_arg, const char* mask1, 
                                const char* mask2, const char* ram_mask1, const char* ram_mask2);
int     ReadFile                (const char* filename, type_buf_char* ptr_text_buf, type_buf_structs* ptr_arr_structs);
int     ReadInBuffer            (FILE* file, type_buf_char* ptr_text_buf);
FILE*   OpenReadingFile         (const char* filename); //
int     MakePointersToLines     (type_buf_char* ptr_text_buf, type_buf_structs* ptr_arr_structs);
void    CreateArrayStructs      (type_buf_char* ptr_text_buf, type_buf_structs* ptr_arr_structs);
int     FileSize                (FILE* file);
int     GetAmountsOfLines       (type_buf_char* ptr_text_buf);
bool    isLetter                (char sym);
FILE*   OpenWFile               (const char* filename); //
int     isLineEmpty             (char* ptr_line);
int     isEndOfFile             (char sym);

//=====================================================================================================================================

#endif
