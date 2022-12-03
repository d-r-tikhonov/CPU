//=====================================================================================================================================

#include "cpu.h"

//=====================================================================================================================================

const char*     NameFileCode    = "asm.bin";    
const char*     SignatureCPU    = "SGN";
const size_t    RamSize         = 5000;
const size_t    RegSize         = 30;

//=====================================================================================================================================

int Run ()
{
    stack_t stk = {};
    StackCtor (&stk);
    
    stack_t stkCalls = {};
    StackCtor (&stkCalls);

    cpu_t cpu = {nullptr, nullptr, nullptr, SignatureCPU, 0, 0, &stk, &stkCalls};
    getCode (&cpu);

    if (Execute (&cpu))
    {
        printf ("Error in function: %s.\n", __func__);
    }

    CPUDtor (&cpu);

    return 0;
}

//=====================================================================================================================================

int Execute (cpu_t* cpu)
{
    ASSERT (cpu != nullptr);

    stack_t* stk = cpu->stk;
    ASSERT (stk != nullptr);

    while (cpu->ip < cpu->size)
    {
        switch (cpu->code[cpu->ip] & CMD_MASK)
        {
            #include "architecture.h"

            default:
                printf ("Error in function: %s. Unknown command!\n", __func__);
                return UNDEFINED_CMD;
        }

        cpu->ip++;
    }
    
    #undef DEF_CMD

    return 0;
}

//=====================================================================================================================================

void SkipNewLines ()
{
    char symbol = 0;

    while ((symbol = getchar()) != '\n')
    {
        continue;   
    }          
}

//=====================================================================================================================================

void PrintRAM (size_t format, cpu_t* cpu, size_t lineLen)
{
    int* ptrRAM = cpu->RAM;

    switch (format)
    {
        case BIN_FORMAT:
        {
            size_t screenSize = RamSize + RamSize / lineLen + 1;

            char* screen = (char*) calloc (screenSize, sizeof (char));

            size_t screenIndex = 0;

            for (size_t i = 0; i < RamSize; i++)
            {
                if (i % lineLen == 0)
                {
                    screen[screenIndex] = '\n';
                    screenIndex++;
                }

                if (ptrRAM[i])
                    screen[screenIndex] = '#';
                else
                    screen[screenIndex] = '.';

                screenIndex++;
            }

            screen[screenIndex] = '\0';
            fwrite (screen, sizeof (char), screenSize, stdout);

            break;
        }

        case SYM_FORMAT:
        {
            for (size_t i = 0; i < RamSize; i++)
            {
                if (i % lineLen == 0)
                {
                    printf ("\n");
                }

                printf ("%c", ptrRAM[i]);
            }
            break;
        }

        default:
            printf ("Error in function: %s. Unexpected format or printing RAM!\n", __func__);
            break;
    }

    printf ("\n");
}

//=====================================================================================================================================

int* GetArg (cpu_t* cpu)
{
    size_t cmd_ip = cpu->ip++;

    if (cpu->code[cmd_ip] & ARG_RAM)
    {
        static int index;

        index = 0;

        if (cpu->code[cmd_ip] & ARG_REG)
        {
            index += cpu->regs[cpu->code[(cpu->ip)++]];
        }

        if (cpu->code[cmd_ip] & ARG_IMMED)
        {
            index += cpu->code[cpu->ip++];
        }

        RETURN_PTR_ARG(cpu->RAM + index);
    }
    else
    {
        static int arg;
        arg = 0;

        if (cpu->code[cmd_ip] & ARG_REG)
        {
            if (!(cpu->code[cmd_ip] & ARG_IMMED))
            {
                int* ptr_to_reg = cpu->regs + cpu->code[(cpu->ip++)];
                RETURN_PTR_ARG(ptr_to_reg);
            }

            arg += cpu->regs[cpu->code[(cpu->ip)++]];
        }

        if (cpu->code[cmd_ip] & ARG_IMMED)
        {
            arg += cpu->code[cpu->ip++] * Accuracy;
        }

        RETURN_PTR_ARG(&arg);
    }
}

//=====================================================================================================================================

void CpuCtor (cpu_t* cpu)
{
    cpu->code = (int*) calloc (cpu->size, sizeof(int));
    ASSERT (cpu->code != nullptr);

    cpu->RAM = (int*) calloc (RamSize, sizeof(int));
    ASSERT (cpu->RAM != nullptr);

    cpu->regs = (int*) calloc (RegSize, sizeof(int));
    ASSERT (cpu->regs != nullptr);
}

//=====================================================================================================================================

void CPUDtor (cpu_t* cpu)
{
    ASSERT (cpu->RAM != nullptr);
    free   (cpu->RAM);

    ASSERT (cpu->regs != nullptr);
    free   (cpu->regs);

    StackDtor (cpu->stk);
    StackDtor (cpu->stkCalls);
}

//=====================================================================================================================================

int checkSign (cpu_t* cpu, FILE* fileASM)
{
    char asm_sign[MaxLen] = {};

    fscanf (fileASM, "%3s", asm_sign);

    if (strcmp (asm_sign, cpu->signature))
    {
        printf ("Error in function: %s. Signature error!\n", __func__);
        return WRONG_SIGNATURE;
    }

    size_t rightVersion = ReadVersion (VersionFile);
    size_t currentVersion = 0;

    fread (&currentVersion, sizeof (int), 1, fileASM);

    if (rightVersion != currentVersion)
    {
        printf ("Error in function: %s. Version error!\n", __func__);
        return -1;
    }

    return 0;
}

//=====================================================================================================================================

int getCode (cpu_t* cpu)
{
    FILE* fileASM = fopen (NameFileCode, "rb");
    ASSERT (fileASM != nullptr);

    if (checkSign (cpu, fileASM))
    {
        return WRONG_SIGNATURE;
    }

    fread (&cpu->size, sizeof (int), 1, fileASM);
    CpuCtor (cpu);

    fscanf (fileASM, "\n");
    fread (cpu->code, sizeof (int), cpu->size, fileASM);

    printf ("\n");
    
    if (fclose (fileASM) != 0)
    {
        printf ("Error in function: %s. Error closing fileASM!\n", __func__);
        return errno;
    }

    return 0;
}

//=====================================================================================================================================