//=====================================================================================================================================

#include "architecture.h"

//=====================================================================================================================================

int Assemble (int argc, const char* argv[])
{
    ASSERT (argc != 2, 1);

    FILE* source = fopen (argv[1], "r");
    ASSERT (source != nullptr, 1);

    FILE* processed = fopen (argv[2], "w+");
    ASSERT (processed != nullptr, 1);

    asm_t asmCode = {};

    AsmCtor (&asmCode, source);

    AsmCreateArray (&asmCode, processed);

    AsmDtor (&asmCode);

    fclose (source);
    fclose (processed);

    return 0;
}

//=====================================================================================================================================

int AsmCtor (asm_t* asmCode, FILE* source)
{
    ASSERT (asmCode != nullptr, 1);
    ASSERT (source  != nullptr, 1);

    TextCtor (&(asmCode->commands), source);

    asmCode->info.sign = SIGNATURE;
    asmCode->info.vers = VERSION;
    asmCode->info.size = asmCode->info.nInt = asmCode->commands.nLines - 1;
    asmCode->asmArr    = (int*) calloc (asmCode->info.size * 2, sizeof (int)); //TODO

    ASSERT (asmCode->asmArr != nullptr, 1);

    return 0;
}

//=====================================================================================================================================

int AsmDtor (asm_t* asmCode)
{
    ASSERT (asmCode != nullptr, 1);
    
    TextDtor (&asmCode->commands);
    free (asmCode->asmArr);

    return 0;
}

//=====================================================================================================================================

int AsmCreateArray (asm_t* asmCode, FILE* processed)
{
    ASSERT (asmCode   != nullptr, 1);
    ASSERT (processed != nullptr, 1);

    size_t index        = 0;
    size_t arrayIndex   = 0;

    for (index = 0, arrayIndex = 0; index < asmCode->info.size; index++)
    {
        char cmd[STR_MAX_SIZE] = {};
        int nChar = 0;

        sscanf (asmCode->commands.lines[index].lineStart, "%s%n", cmd, &nChar);

        if (strcasecmp (cmd, "push") == 0)
        {
            asmCode->asmArr[arrayIndex++] = CMD_PUSH;

            if (sscanf (asmCode->commands.lines[index].lineStart + nChar, "%d", &(asmCode->asmArr[arrayIndex++])) == 0)
            {
                printf ("Error! Incorrectly entered operation: %s", cmd);
                return 1;
            }

            (asmCode->info.nInt)++;
        }
        else if ((strcasecmp (cmd, "add") == 0))
        {   
            asmCode->asmArr[arrayIndex++] = CMD_ADD;
        }
        else if ((strcasecmp (cmd, "sub") == 0))
        {   
            asmCode->asmArr[arrayIndex++] = CMD_SUB;
        }
        else if ((strcasecmp (cmd, "mul") == 0))
        {   
            asmCode->asmArr[arrayIndex++] = CMD_MUL;
        }
        else if ((strcasecmp (cmd, "div") == 0))
        {   
            asmCode->asmArr[arrayIndex++] = CMD_DIV;
        }
        else if ((strcasecmp (cmd, "out") == 0))
        {   
            asmCode->asmArr[arrayIndex++] = CMD_OUT;
        }
        else if ((strcasecmp (cmd, "hlt") == 0))
        {   
            asmCode->asmArr[arrayIndex++] = CMD_HLT;
        }
        else
        {
            printf ("Error! Incorrectly entered operation: %s", cmd);
            return 1;
        }
    }

    WritingBinFile (asmCode, processed);

    return 0;
}

//=====================================================================================================================================

int WritingBinFile (asm_t* asmCode, FILE* binaryFile)
{
    fwrite (&asmCode->info, sizeof (asmCode->info), 1, binaryFile);
    fwrite (asmCode->asmArr, sizeof (int), asmCode->info.nInt, binaryFile);

    return 0;
}

//=====================================================================================================================================
