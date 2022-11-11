//=====================================================================================================================================

#include "cpu.h"

//=====================================================================================================================================

const char*     CPU_SIGNATURE   = "DRT";
const char*     NAME_FILE_CODE  = "ASM.txt";
const size_t    RAM_SIZE        = 5000;
const size_t    REG_SIZE        = 30;

//=====================================================================================================================================

int Run ()
{
    stack_t stk1 = {};
    StackCtor (&stk1);
    
    stack_t stkCalls1 = {};
    StackCtor (&stkCalls1);

    CPU cpu = {NULL, NULL, NULL, CPU_SIGNATURE, 0, 0, &stk1, &stkCalls1};
    getCode (&cpu);

    if (Execute(&cpu))
    {
        printf ("Error in function: %s.\n", __func__);
    }

    CPUDtor (&cpu);

    return 0;
}

int Execute (CPU* cpu)
{
    ASSERT (cpu != nullptr);

    stack_t* stk = cpu->Stk;
    ASSERT (stk != nullptr);

    while (cpu->ip < cpu->Size)
    {
        switch (cpu->code[cpu->ip] & CMD_MASK)
        {
            #include "strcmp_for_asm.h"

            default:
                printf ("Error in function: %s. Unknown command!\n", __func__);
                return UNDEFINED_CMD;
        }

        cpu->ip++;
    }
    
    #undef DEF_CMD

    return 0;
}

void SkipNewLines ()
{
    char symbol = 0;

    while ((symbol = getchar()) != '\n')
    {
        continue;   
    }          
}

void PrintRAM (size_t format, CPU* cpu, size_t len_line)
{
    int* ptr_RAM = cpu->RAM;

    switch (format)
    {
        case BIN_FORMAT:
        {
            size_t screen_size = RAM_SIZE + RAM_SIZE / len_line + 1;

            char* screen = (char*) calloc (screen_size, sizeof (char));

            size_t screen_index = 0;

            for (size_t i = 0; i < RAM_SIZE; i++)
            {
                if (i % len_line == 0)
                {
                    screen[screen_index] = '\n';
                    screen_index++;
                }

                if (ptr_RAM[i])
                    screen[screen_index] = '#';
                else
                    screen[screen_index] = '.';

                screen_index++;
            }

            screen[screen_index] = '\0';
            fwrite (screen, sizeof(char), screen_size, stdout);

            break;
        }

        case SYM_FORMAT:
        {
            for (size_t i = 0; i < RAM_SIZE; i++)
            {
                if (i % len_line == 0)
                {
                    printf ("\n");
                }

                printf ("%c", ptr_RAM[i]);
            }
            break;
        }

        default:
            printf ("Error in function: %s. Unexpected format or printing RAM!\n", __func__);
    }

    printf ("\n");
}

int* GetArg (CPU* cpu)
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
            arg += cpu->code[cpu->ip++] * ACCURACY;
        }

        RETURN_PTR_ARG(&arg);
    }
}

void CpuCtor (CPU* cpu)
{
    cpu->code = (int*) calloc (cpu->Size, sizeof(int));
    ASSERT (cpu->code != nullptr);

    cpu->RAM = (int*) calloc (RAM_SIZE, sizeof(int));
    ASSERT (cpu->RAM != nullptr);

    cpu->regs = (int*) calloc (REG_SIZE, sizeof(int));
    ASSERT (cpu->regs != nullptr);
}

void CPUDtor (CPU* cpu)
{
    ASSERT (cpu->RAM != nullptr);
    free   (cpu->RAM);

    ASSERT (cpu->regs != nullptr);
    free   (cpu->regs);

    StackDtor (cpu->Stk);
    StackDtor (cpu->StkCalls);
}

int checkSign (CPU* cpu, FILE* file_asm)
{
    char asm_sign[MAX_LEN_SIGN] = {};

    fscanf (file_asm, "%3s", asm_sign);

    if (strcmp (asm_sign, cpu->signature))
    {
        printf ("Error in function: %s. Signature error!\n", __func__);
        return WRONG_SIGNATURE;
    }

    size_t right_version = ReadVersion (VersionFile);
    size_t gachi_version = 0;

    fread (&gachi_version, sizeof(int), 1, file_asm);

    if (right_version != gachi_version)
    {
        printf ("Error in function: %s. Version error!\n", __func__);
        return -1;
    }

    return 0;
}

int getCode (CPU* cpu)
{
    FILE* file_asm = fopen (NAME_FILE_CODE, "rb");
    ASSERT (file_asm != nullptr);

    if (checkSign(cpu, file_asm))
    {
        return WRONG_SIGNATURE;
    }

    fread (&cpu->Size, sizeof(int), 1, file_asm);
    CpuCtor (cpu);

    fscanf (file_asm, "\n");
    fread (cpu->code, sizeof(int), cpu->Size, file_asm);

    printf ("\n");

    fclose (file_asm);

    return 0;
}