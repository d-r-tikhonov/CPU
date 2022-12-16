//=====================================================================================================================================

#include "cpu.h"

//=====================================================================================================================================

int Run (int argc, const char* argv[])
{
    ASSERT (argc >= 2, -1);
    
    FILE* data = fopen (argv[2], "rb");
    ASSERT (data != nullptr, -1);

    cpu_t cpu = {};
    CpuCtor (&cpu, data);

    Execute (&cpu);

    CpuDtor (&cpu);
    fclose (data);

    return 0;
}

//=====================================================================================================================================

int CpuCtor (cpu_t* cpu, FILE* binaryFile)
{
    ASSERT (cpu        != nullptr, -1);
    ASSERT (binaryFile != nullptr, -1);

    StackCtor (&cpu->stk);

    infoCMD_t binInfo = {};
    fread (&binInfo, sizeof (infoCMD_t), 1, binaryFile);

    ASSERT (binInfo.sign == SIGNATURE, -1);
    ASSERT (binInfo.vers == VERSION,   -1);

    cpu->size = binInfo.size + sizeof (int) * binInfo.nArgs + 1;

    cpu->cmdArr = (char*) calloc (1, cpu->size);
    ASSERT (cpu->cmdArr != nullptr, -1);

    fread (cpu->cmdArr, sizeof (char), binInfo.size + sizeof (int) * binInfo.nArgs, binaryFile);
    
    return 0;
}

//=====================================================================================================================================

int CpuDtor (cpu_t* cpu)
{
    ASSERT (cpu != nullptr, -1);

    StackDtor (&cpu->stk);
    free (cpu->cmdArr);

    return 0;
    
}

//=====================================================================================================================================

int Execute (cpu_t* cpu)
{
    ASSERT (cpu != nullptr, -1);

    size_t ip = 0;

    while (*(cpu->cmdArr + ip))
    {
        int currentCmd = *(cpu->cmdArr + ip);

        #define DEF_CMD(name, num, arg, code)               \
            case CMD_##name:                                \
                code                                        \
                break;

        #define DEF_JMP(name, num, sign)                    \
            case JMP_##name:                                \
            {                                               \
                INDEX_UP;                                   \
                if ((currentCmd & ONLY_CMD) == JMP_JMP)     \
                {                                           \
                    GET_JMP_ARG;                            \
                }                                           \
                else                                        \
                {                                           \
                    VAR firstNum  = POP;                    \
                    VAR secondNum = POP;                    \
                                                            \
                    if (firstNum sign secondNum)            \
                    {                                       \
                        GET_JMP_ARG;                        \
                    }                                       \
                    else                                    \
                        ip += sizeof (int) - 1;             \
                }                                           \
                break;                                      \
            }

        switch (currentCmd & ONLY_CMD)
        {
            #include "architecture.h"

            default:
                printf ("Error! An unknown operation was encountered: %d\n", *(cpu->cmdArr + ip));
                return 1;
        }

        ip++;
    }

    #undef DEF_CMD
    #undef DEF_JMP

    return 0;
}

//=====================================================================================================================================

int GetPushArg (int command, size_t* ip, cpu_t* cpu)
{
    ASSERT (ip   != nullptr, -1);
    ASSERT (cpu  != nullptr, -1);

    size_t arg          = 0;
    int    value        = 0;
    int    currentReg   = 0; 
    int    ipCtrl       = 0;

    if (command & ARG_IMMED)
    {   
        memcpy (&value, cpu->cmdArr + *ip, sizeof (int));
        arg += value;

        ipCtrl++;
    }

    if (command & ARG_REG)
    {
        if (command & ARG_IMMED)
        {
            memcpy (&currentReg, cpu->cmdArr + *ip + sizeof (int), sizeof (int));
            arg += cpu->regs[currentReg];

            ipCtrl++;
        }
        else 
        {
            memcpy (&currentReg, cpu->cmdArr + *ip, sizeof (int));
            arg += cpu->regs[currentReg];

            ipCtrl++;
        }
    }

    if (command & ARG_MEM)
    {
        if (arg > MAX_RAM_SIZE)
        {
            printf ("Syntax error: %x\n", command);
            return -1;
        }
        else
        {
            arg = cpu->RAM[arg];
        }
    }

    if ((command & (ARG_IMMED | ARG_REG | ARG_MEM)) == 0)
    {
        printf ("Syntax error: %x\n", command);
        return -1;
    }

    *ip += ipCtrl * sizeof (int) - 1;

    return arg;
}

//=====================================================================================================================================

int* GetPopArg (int command, size_t* ip, cpu_t* cpu)
{
    ASSERT (ip   != nullptr, nullptr);
    ASSERT (cpu  != nullptr, nullptr);

    size_t arg         = 0; 
    int    value       = 0;
    int    currentReg  = 0; 

    if (command & ARG_MEM)
    {
        if (command & ARG_IMMED)
        {   
            memcpy (&value, cpu->cmdArr + *ip, sizeof (int));
            *ip += sizeof (int);

            arg += value;
        }

        if (command & ARG_REG)
        {
            memcpy (&currentReg, cpu->cmdArr + *ip, sizeof (int));
            *ip += sizeof (int);

            arg += cpu->regs[currentReg];
        }

        if (arg >= MAX_RAM_SIZE)
        {
            printf ("Syntax error: %x\n", command);
            return nullptr;
        }
        else
        {
            *ip -= 1;

            return &cpu->RAM[arg];
        }
    }
    else if (command & ARG_REG) 
    {
        memcpy (&currentReg, cpu->cmdArr + *ip, sizeof (int));
        *ip += sizeof (int) - 1;

        return cpu->regs + currentReg;

    }
    else
    {
        printf ("Syntax error: %x\n", command);
        return nullptr;
    }

    return nullptr;
}

//=====================================================================================================================================

int GetJumpArg (size_t* ip, cpu_t* cpu)
{
    ASSERT (ip  != nullptr, -1);
    ASSERT (cpu != nullptr, -1);

    int arg = 0;

    memcpy (&arg, cpu->cmdArr + *ip, sizeof (int));

    if (arg < 0)
    {
        printf ("Bad jumping on %d...\n", arg);
        return 1;
    }

    *ip = arg - 1;

    return 0;
}

//=====================================================================================================================================
