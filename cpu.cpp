//=====================================================================================================================================

#include "architecture.h"

//=====================================================================================================================================

int Run (int argc, const char* argv[])
{
    ASSERT (argc != 2, 1);
    
    FILE* data = fopen (argv[2], "r");
    ASSERT (data != nullptr, 1);

    // size_t nMemb    = 0;
    // int*   cmdArr   = nullptr;

    // cmdArr = ReadingBinFile (cmdArr, &nMemb, data);

    cpu_t cpu = {};
    CpuCtor (&cpu, data);

    Execute (&cpu);

    printf ("EXECUTE!");

    CpuDtor (&cpu);
    // free (cmdArr);
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

    if (binInfo.sign != SIGNATURE)
    {
        printf ("Error! The signature is uncorrected!\n");
        abort ();
    }

    if (binInfo.vers != VERSION)
    {
        printf ("Error! The version is uncorrected!\n");
        abort ();
    }

    cpu->cmdArr = (char*) calloc (1, binInfo.size + sizeof (int) * binInfo.nArgs + 1);
    ASSERT (cpu->cmdArr != nullptr, 1);

    fread (cpu->cmdArr, sizeof (char), binInfo.size + sizeof (int) * binInfo.nArgs, binaryFile);
    
    return 0;
}

//=====================================================================================================================================

int CpuDtor (cpu_t* cpu)
{
    ASSERT (cpu  != nullptr, -1);

    StackDtor (&cpu->stk);
    free (cpu->cmdArr);

    return 0;
    
}

//=====================================================================================================================================

// int* ReadingBinFile (int* cmdArr, size_t* nMemb, FILE* binaryFile)
// {
//     infoCMD_t binInfo = {};
//     fread (&binInfo, sizeof (infoCMD_t), 1, binaryFile);

//     if (binInfo.sign != SIGNATURE)
//     {
//         printf ("Error! The signature is uncorrected!\n");
//         abort ();
//     }

//     if (binInfo.vers != VERSION)
//     {
//         printf ("Error! The version is uncorrected!\n");
//         abort ();
//     }

//     cmdArr = (int*) calloc (binInfo.nInt + 1, sizeof (int));
//     fread (cmdArr, sizeof (int), binInfo.nInt, binaryFile);
//     *nMemb = binInfo.nInt;
    
//     return cmdArr;
// }

//=====================================================================================================================================

int Execute (cpu_t* cpu)
{
    ASSERT (cpu != nullptr, -1);

    int ip  = 0;

    int firstNum  = 0;
    int secondNum = 0;

    int currentCmd = 0;

    #define CALC(stk, sign)                         \
        firstNum    = StackPop (stk);               \
        secondNum   = StackPop (stk);               \
        StackPush (stk, firstNum sign secondNum);  

    while (*(cpu->cmdArr + ip))
    {
        currentCmd = *(cpu->cmdArr + ip);

        switch (currentCmd & ONLY_CMD)
        {
            case CMD_PUSH:
            {
                printf ("push");

                ip++;
                int arg = GetPushArg (currentCmd, &ip, cpu);
                StackPush (&cpu->stk, arg);

                break;
            }

            case CMD_POP:
            {   
                ip++;
                int* arg = GetPopArg (currentCmd, &ip, cpu);
                *arg = StackPop (&cpu->stk);
                break;
            }

            case CMD_JMP:
            {
                ip++;
                GetJumpArg (&ip, cpu);
            }

            case CMD_ADD:
            {
                printf ("add\n");

                CALC(&cpu->stk, +);
                break;
            }

            case CMD_SUB:
            {
                printf ("sub\n");

                CALC(&cpu->stk, -);
                break;
            }

            case CMD_MUL:
            {
                printf ("mul\n");

                CALC(&cpu->stk, *);
                break;
            }

            case CMD_DIV:
            {
                printf ("div\n");

                CALC(&cpu->stk, /);
                break;
            }

            case CMD_OUT:
            {
                printf ("OUT: ");

                printf ("%d\n", StackPop (&cpu->stk));
                break;
            }

            case CMD_HLT:
            {
                printf ("hlt\n");

                break;
            }

            default:
                printf ("Error! Unknown operation in %s!\n", __func__);
        }

        ip++;
    }

    #undef CALC

    return 0;
}

//=====================================================================================================================================

int GetPushArg (int command, int* ip, cpu_t* cpu)
{
    ASSERT (ip   != nullptr, -1);
    ASSERT (cpu  != nullptr, -1);

    int    arg          = 0;
    int    value        = 0;
    int    currentReg   = 0; 
    short  ipCtrl       = 0;

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
            abort ();
        }
        else
        {
            arg = cpu->RAM[arg];
        }
    }

    if ((command & (ARG_IMMED | ARG_REG | ARG_MEM)) == 0)
    {
        printf ("Syntax error: %x\n", command);
        abort ();
    }

    *ip += ipCtrl * sizeof (int) - 1;

    return arg;
}

//=====================================================================================================================================

int* GetPopArg (int command, int* ip, cpu_t* cpu)
{
    ASSERT (ip   != nullptr, nullptr);
    ASSERT (cpu  != nullptr, nullptr);

    int arg         = 0; 
    int value       = 0;
    int currentReg  = 0; 

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
            *ip += sizeof(int);

            arg += cpu->regs[currentReg];
        }

        if (arg >= MAX_RAM_SIZE)
        {
            printf ("Syntax error: %x\n", command);
            abort ();
        }
        else
        {
            sleep (1); //TODO
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
        abort ();
    }

    return nullptr;
}

//=====================================================================================================================================

void GetJumpArg (int* ip, cpu_t* cpu)
{
    ASSERT (ip  != nullptr, (void) -1);
    ASSERT (cpu != nullptr, (void) -1);

    int arg = 0;

    memcpy (&arg, cpu->cmdArr + *ip, sizeof (int));

    if (arg < 0)
    {
        printf ("Bad jumping on %d...\n", arg);
        abort();
    }

    *ip = arg - 1;
}

//=====================================================================================================================================
