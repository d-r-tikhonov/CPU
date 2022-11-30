//=====================================================================================================================================

#include "disassembler.h"

//=====================================================================================================================================

#undef DEF_CMD

#define DEF_CMD(name, num, arg, ...)                      \
    case num:                                             \
        PutCmd(disasm, #name);                            \
        switch (arg)                                      \
        {                                                 \
            case 2:                                       \
                PutStackCmd(disasm);                      \
                break;                                    \
            case 1:                                       \
                disasm->ip++;                             \
                WRITE_NUMBER;                             \
                break;                                    \
            default:                                      \
                disasm->ip++;                             \
        }                                                 \
        disasm->buf_code[disasm->cursor++] = '\n';        \
    break;

#define WRITE_NUMBER                                      \
{                                                         \
    PutNumToCharBuf (disasm);                             \
}

//=====================================================================================================================================

const char* DISASM_SIGNATURE        = "SGN";
const char* ASM_FILE_NAME_DISASM    = "asm.bin  ";
const int   MAX_LEN_CMD             = 6;

//=====================================================================================================================================

int MakeReadableCode ()
{
    disasm_t disasm = {nullptr, 0, 0, DISASM_SIGNATURE, nullptr, 0};

    getCodeForDisasm (&disasm);

    Disassemble (&disasm);

    WriteUserCode (&disasm, "Disassembled.txt");

    return 0;
}

//=====================================================================================================================================

int PutCmd (disasm_t* disasm, const char* cmd_name)
{
    strcpy (disasm->buf_code + disasm->cursor, cmd_name);

    disasm->cursor = disasm->cursor + strlen (cmd_name);
    disasm->buf_code[disasm->cursor++] = ' ';

    return 0;
}

//=====================================================================================================================================

int HandleRegsDisasm (disasm_t* disasm, size_t reg_num)
{
    char reg_name[MAX_LEN_REG_NAME] = {'r', 'z', 'x'};

    reg_name[1] = (char)(reg_num + (int)'a');

    strcpy (disasm->buf_code + disasm->cursor, reg_name);

    disasm->cursor = disasm->cursor + strlen (reg_name);

    return 0;
}

//=====================================================================================================================================

int PutNumToCharBuf (disasm_t* disasm)
{
    char* line = itoa ((disasm->asm_code)[disasm->ip++], disasm->buf_code + disasm->cursor, 10);

    disasm->cursor = disasm->cursor + strlen (line);

    return 0;
}

//=====================================================================================================================================

int PutStackCmd (disasm_t* disasm)
{

    size_t cmd_ip = disasm->ip++;

    if (disasm->asm_code[cmd_ip] & ARG_RAM)   disasm->buf_code[disasm->cursor++] = '[';

    if (disasm->asm_code[cmd_ip] & ARG_REG)
    {
        HandleRegsDisasm (disasm, (disasm->asm_code)[disasm->ip++]);
    }

    if (disasm->asm_code[cmd_ip] & ARG_IMMED)
    {
        WRITE_NUMBER;
    }

    if (disasm->asm_code[cmd_ip] & ARG_RAM)   
    {
        disasm->buf_code[disasm->cursor++] = ']';
    }

    return 0;
}

//=====================================================================================================================================

int Disassemble (disasm_t* disasm)
{
    ASSERT (disasm != nullptr);

    while (disasm->ip < disasm->size)
    {
        switch(disasm->asm_code[disasm->ip] & CMD_MASK)
        {
            #include "architecture.h"

            default:
                return UNDEFINED_CMD;
        }
    }
    #undef DEF_CMD

    return 0;
}

//=====================================================================================================================================

int WriteUserCode (disasm_t* disasm, const char* filename)
{
    FILE* stream = fopen (filename, "wb");
    ASSERT (stream != nullptr);

    size_t num_written_sym = fwrite (disasm->buf_code, sizeof (char), disasm->cursor + 1, stream);

    if (num_written_sym != disasm->cursor + 1)
    {
        printf ("Error in function: %s. Error when writing to a file!\n", __func__);
        return -1;
    }

    if (fclose (stream) != 0)
    {
        printf ("Error in function: %s. Error closing file!\n", __func__);
        return errno;
    }

    return 0;
}

//=====================================================================================================================================

int checkSignDisasm (disasm_t* disasm, FILE* file_asm)
{
    char asm_sign[MaxLen] = {};

    fscanf (file_asm, "%3s", asm_sign);

    if (strcmp (asm_sign, disasm->signature))
    {
        printf ("Error in function: %s. Signature error!\n", __func__);
        return -1;
    }

    return 0;
}

//=====================================================================================================================================

void BufCtor (disasm_t* disasm)
{
    disasm->asm_code = (int*) (calloc(disasm->size, sizeof(int)));
    ASSERT (disasm->asm_code != nullptr);

    disasm->buf_code = (char*) (calloc(disasm->size * MAX_LEN_CMD, sizeof(char)));
    ASSERT (disasm->buf_code != nullptr);
}

//=====================================================================================================================================

int getCodeForDisasm (disasm_t* disasm)
{
    FILE* fileASM = fopen (ASM_FILE_NAME_DISASM, "rb");
    ASSERT (fileASM != nullptr);

    if (checkSignDisasm (disasm, fileASM))
    {
        return -1;
    }

    fread (&disasm->size, sizeof (int), 1, fileASM);

    BufCtor (disasm);

    fscanf (fileASM, "\n");

    fread (disasm->asm_code, sizeof (int), disasm->size, fileASM);

    printf ("\n");

    if (fclose (fileASM) != 0)
    {
        printf ("Error in function: %s. Error closing fileASM!\n", __func__);
        return errno;
    }

    return 0;
}

//=====================================================================================================================================
