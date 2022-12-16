//=====================================================================================================================================

#include "cpu.h"

//=====================================================================================================================================

const char*  LABEL_NAME_POSION  = "FREE";

//=====================================================================================================================================

int Assemble (int argc, const char* argv[])
{
    ASSERT (argc >= 2, -1);

    FILE* source = fopen (argv[1], "r");
    ASSERT (source != nullptr, -1);

    FILE* processed = fopen (argv[2], "wb+");
    ASSERT (processed != nullptr, -1);

    asm_t asmCode = {};

    AsmCtor (&asmCode, source);

    AsmCreateArray (&asmCode);
    AsmCreateArray (&asmCode);    

    WritingBinFile (&asmCode, processed);

    AsmDtor (&asmCode);

    fclose (source);
    fclose (processed);

    return 0;
}

//=====================================================================================================================================

int AsmCtor (asm_t* asmCode, FILE* source)
{
    ASSERT (asmCode != nullptr, -1);
    ASSERT (source  != nullptr, -1);

    TextCtor (&(asmCode->commands), source);

    asmCode->info.sign = SIGNATURE;
    asmCode->info.vers = VERSION;
    asmCode->info.size = asmCode->commands.nLines;
    asmCode->info.nArgs = 0;

    asmCode->asmArr = (char*) calloc (asmCode->info.size * 2, sizeof (int));
    ASSERT (asmCode->asmArr != nullptr, -1);

    for (size_t index = 0; index < MAX_LABEL_COUNT; index++)
    {
        asmCode->labels[index].adress = LABEL_POISON;
        asmCode->labels[index].name   = (char*) LABEL_NAME_POSION;
    }

    return 0;
}

//=====================================================================================================================================

int AsmDtor (asm_t* asmCode)
{
    ASSERT (asmCode != nullptr, -1);
    
    TextDtor (&asmCode->commands);
    free (asmCode->asmArr);

    return 0;
}

//=====================================================================================================================================

int AsmCreateArray (asm_t* asmCode)
{
    ASSERT (asmCode != nullptr, -1);

    size_t ip = 0;
    size_t index = 0;

    for (index = 0, ip = 0; index < asmCode->info.size; index++)
    {
        char cmd[STR_MAX_SIZE] = {};
        int  nChar             = 0 ;

        sscanf (asmCode->commands.lines[index].lineStart, "%s%n", cmd, &nChar);
   
        #define DEF_CMD(name, num, arg, code)                                                                   \
            if (strcasecmp (cmd, #name) == 0)                                                                   \
            {                                                                                                   \
                if (arg)                                                                                        \
                {                                                                                               \
                    ParseArg (asmCode->commands.lines[index].lineStart + nChar, CMD_##name, asmCode, &ip);      \
                }                                                                                               \
                else                                                                                            \
                {                                                                                               \
                    *(asmCode->asmArr + ip++) = CMD_##name;                                                     \
                }                                                                                               \
            }                                                                                                   \
            else

        #define DEF_JMP(name, num, sign)                                                                        \
            if (strcasecmp (cmd, #name) == 0)                                                                   \
            {                                                                                                   \
                ParseArg (asmCode->commands.lines[index].lineStart + nChar, JMP_##name, asmCode, &ip);          \
            }                                                                                                   \
            else

        #include "architecture.h"

        if (strchr (cmd, ':') != nullptr)
        {   
            ParseLabel (cmd, asmCode, ip); 
        }
        else
        {
            printf ("Incorrectly entered operation on %llu line: %s", index,  cmd);
            abort();
        }
    }
    
    #undef DEF_CMD
    #undef DEF_JMP

    return 0;
}

//=====================================================================================================================================

int WritingBinFile (asm_t* asmCode, FILE* binaryFile)
{
    ASSERT (asmCode    != nullptr, -1);
    ASSERT (binaryFile != nullptr, -1);

    fwrite (&asmCode->info, sizeof (asmCode->info), 1, binaryFile);
    fwrite (asmCode->asmArr, sizeof (char), asmCode->info.size + asmCode->info.nArgs * sizeof (int), binaryFile);

    return 0;
}

//=====================================================================================================================================

int isRegister (char* str)
{
    ASSERT (str != nullptr, -1);

    if (strlen (str) == 3 && str[0] == 'r' && str[2] == 'x' && str[1] >= 'a' && str[1] <= 'd')
    {
        return str[1] - 'a' + 1;
    }

    if (strlen (str) == 3 && str[0] == 'R' && str[2] == 'X' && str[1] >= 'A' && str[1] <= 'D')
    {
        return str[1] - 'A' + 1;
    }
    
    return 0;
}

//=====================================================================================================================================

int isBrackets (char* str)
{
    ASSERT (str != nullptr, -1);

    char* open  = strchr (str, '[');
    char* close = strchr (str, ']');

    if (open == nullptr && close == nullptr)
    {
        return 0;
    }

    if (open == nullptr && close != nullptr)
    {
        printf ("Syntax error! Skipped '['...\n");
        
        return -1;
    }
    else if (open != nullptr && close == nullptr)
    {
        printf ("Syntax error! Skipped ']'...\n");

        return -1;
    }

    return 1;
}

//=====================================================================================================================================

int ParseArg (char* line, int command, asm_t* asmCode, size_t* ip)
{
    ASSERT (line    != nullptr, -1);
    ASSERT (asmCode != nullptr, -1);
    ASSERT (ip      != nullptr, -1);

    int arg = 0;

    if (command >= JMP_JMP && command <= JMP_JNE)
    {
        arg = ParseJumpArg (line, command, asmCode, ip);
    }
    else
    {
        switch (isBrackets (line))
        {
        case 0:
            arg = ParseCommonArg (line, command, asmCode, ip);
            break;

        case 1:
            arg = ParseBracketsArg (line, command, asmCode, ip);
            break;

        default:
            arg = 1;
            break;
        }
    }

    if (arg != 0)
    {
        printf ("Error in %s! Invalid argument: %s...\n", __func__, line);

        return 1;
    }

    return 0;
}

//=====================================================================================================================================

int ParseJumpArg (char* line, int command, asm_t* asmCode, size_t* ip)
{
    ASSERT (line    != nullptr, -1);
    ASSERT (asmCode != nullptr, -1);
    ASSERT (ip      != nullptr, -1);

    *(asmCode->asmArr + *ip) = command;

    int  currentValue = 0;
    char currentLabel[MAX_LABEL_SIZE] = {};

    if (strchr (line, ':') == nullptr)
    {
        if (sscanf (line, "%d", &currentValue) == 1)
        {
            memcpy (asmCode->asmArr + *ip + 1, &currentValue, sizeof (int));
        }
        else
        {            
            return 1;
        }
    }
    else
    {
        if (sscanf (line, " :%d", &currentValue) == 1 && currentValue >= 0 && currentValue <= MAX_LABEL_COUNT)
        {
            if (asmCode->labels[currentValue].adress == LABEL_POISON)
            {
                currentValue = LABEL_POISON;

                memcpy (asmCode->asmArr + *ip + 1, &currentValue, sizeof (int));
            }
            else
            {
                memcpy (asmCode->asmArr + *ip + 1, &asmCode->labels[currentValue].adress, sizeof (int));
            }
        }
        else if (sscanf (line, " :%s", currentLabel) == 1)
        {
            for (size_t num = 0; num < MAX_LABEL_COUNT; num++)
            {
                if (strcmp (asmCode->labels[num].name, currentLabel) == 0)
                {
                    memcpy (asmCode->asmArr + *ip + 1, &asmCode->labels[num].adress, sizeof (int));
                    currentValue = num;

                    break;
                }
            }
        }
        else
        {
            return 1;
        }
    }

    *ip = *ip + sizeof (int) + 1;

    asmCode->info.nArgs = asmCode->info.nArgs + 1;

    return 0;
}

//=====================================================================================================================================

int ParseLabel (char* cmd, asm_t* asmCode, size_t ip)
{
    ASSERT (cmd     != nullptr, -1);
    ASSERT (asmCode != nullptr, -1);

    int  currentAdress                      = -1;
    char currentTextLabel[MAX_LABEL_SIZE]   = {};
    int  labelLen                           = 0;

    if (sscanf (cmd, "%d:", &currentAdress) == 1)
    {
        if (currentAdress >= 0 && currentAdress < MAX_LABEL_COUNT && asmCode->labels[currentAdress].adress == LABEL_POISON)
        {
            asmCode->labels[currentAdress].name   = cmd;
            asmCode->labels[currentAdress].adress = ip;
        }
        else
        {
            printf ("Error! Invalid label: %s in %s\n", cmd, __func__);
            return 1;
        }
    }
    else if (sscanf (cmd, "%s%n", currentTextLabel, &labelLen) == 1)
    {
        currentTextLabel[labelLen - 1] = '\0';
        int label = 0;

        for (size_t num = 0; num < MAX_LABEL_COUNT; num++)
        {
            if (strcmp (asmCode->labels[num].name, LABEL_NAME_POSION) == 0)
            {
                asmCode->labels[num].name = currentTextLabel;

                asmCode->labels[num].adress = ip;

                currentAdress = num;

                label = 1;

                break;
            }
            else if (asmCode->labels[num].adress != LABEL_POISON)
            {
                label = 1;
                
                break;
            }
        }

        if (label == 0)
        {
            printf ("Label error in %s...", __func__);
            return 1;
        }
    }
    else
    {
        printf ("Label error: %s in %s\n", cmd, __func__);
        return 1;
    }

    return 0;
}

//=====================================================================================================================================

int ParseCommonArg (char* line, int command, asm_t* asmCode, size_t* ip)
{
    ASSERT (line    != nullptr, -1);
    ASSERT (asmCode != nullptr, -1);
    ASSERT (ip      != nullptr, -1);

    int  curValue              = 0;
    int  intReg                = 0;
    char curReg[STR_MAX_SIZE]  = {};
    
    if (strchr (line, '+') == nullptr)
    {
        if (command != CMD_POP && sscanf (line, "%d", &curValue) == 1)
        {
            *(asmCode->asmArr + *ip) = CMD_PUSH | ARG_IMMED; 

            memcpy (asmCode->asmArr + *ip + 1, &curValue, sizeof (int));
        }
        else if (sscanf (line, "%s", curReg) == 1  && (intReg = isRegister (curReg)) != -1)
        {
            *(asmCode->asmArr + *ip) = command | ARG_REG;
            memcpy (asmCode->asmArr + *ip + 1, &intReg, sizeof (int));
        }
        else
            return 1;
        
        *ip += 1 + sizeof (int);
        (asmCode->info.nArgs)++;
    }
    
    else
    {
        if (command != CMD_POP && sscanf (line, "%d+%s", &curValue, curReg) == 2 && (intReg = isRegister (curReg)) != -1)
        {
            *(asmCode->asmArr + *ip) = CMD_PUSH | ARG_IMMED | ARG_REG;  
            
            memcpy (asmCode->asmArr + *ip + 1, &curValue, sizeof (int));
            memcpy (asmCode->asmArr + *ip + 1 + sizeof (int), &intReg, sizeof (int));
        }

        else
            return 1;

        *ip += 2 * sizeof (int) + 1;
        (asmCode->info.nArgs) += 2;
    }

    return 0;
}

//=====================================================================================================================================

int ParseBracketsArg (char* line, int command, asm_t* asmCode, size_t* ip)
{
    ASSERT (line    != nullptr, -1);
    ASSERT (asmCode != nullptr, -1);
    ASSERT (ip      != nullptr, -1);
    

    char* arg = strchr(line, '[') + 1;
    
    int  curValue               = 0;
    int  intReg                 = 0;
    char curReg[STR_MAX_SIZE]   = {};
    
    if (strchr (line, '+') == nullptr)
    {
        if (sscanf (arg, "%d", &curValue) == 1)
        {
            *(asmCode->asmArr + *ip) = command | ARG_IMMED | ARG_MEM; 
            memcpy (asmCode->asmArr + *ip + 1, &curValue, sizeof (int));
        }
        else if (sscanf (arg, "%s", curReg) == 1)
        {
            curReg[strlen (curReg) - 1] = '\0';

            if ((intReg = isRegister (curReg)) != -1)
            {
                *(asmCode->asmArr + *ip) = command | ARG_REG | ARG_MEM;
                memcpy (asmCode->asmArr + *ip + 1, &intReg, sizeof (int));
            }
            else
                return 1;
        }
        else
            return 1;

        *ip += 1 + sizeof (int);
        (asmCode->info.nArgs)++;
    }
    
    else
    {
        if (sscanf (arg, "%d+%s", &curValue, curReg) == 2)
        {
            curReg[strlen (curReg) - 1] = '\0';

            if ((intReg = isRegister (curReg)) != -1)
            {
                *(asmCode->asmArr + *ip) = command | ARG_IMMED | ARG_REG | ARG_MEM;  
            
                memcpy (asmCode->asmArr + *ip + 1, &curValue, sizeof (int));
                memcpy (asmCode->asmArr + *ip + 1 + sizeof (int), &intReg, sizeof (int));
            }
            else
                return 1;
        }
        else
            return 1;

        *ip += 2 * sizeof (int) + 1;
        (asmCode->info.nArgs) += 2;
    }

    return 0;
}

//=====================================================================================================================================