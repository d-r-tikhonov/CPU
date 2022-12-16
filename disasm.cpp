//=====================================================================================================================================

#include "cpu.h"

//=====================================================================================================================================

int RunDisasm (int argc, const char* argv[])
{
    ASSERT (argc >= 3, -1);

    FILE* data = fopen (argv[1], "rb");
    ASSERT (data != nullptr, -1);

    FILE* disasmFile = fopen (argv[3], "w+");
    ASSERT (disasmFile != nullptr, -1);

    char* cmdArr = nullptr;

    cmdArr = ReadBin (data);
    ASSERT (cmdArr != nullptr, -1);

    ExecuteDisasm (cmdArr, disasmFile);

    free (cmdArr);

    fclose (disasmFile);

    return 0;
} 

//=====================================================================================================================================

char* ReadBin (FILE* binaryFile)
{
    ASSERT (binaryFile != nullptr, nullptr);

    infoCMD_t binInfo = {};

    fread (&binInfo, sizeof (infoCMD_t), 1, binaryFile);

    ASSERT (binInfo.sign == SIGNATURE, nullptr);
    ASSERT (binInfo.vers == VERSION,   nullptr);

    size_t size = binInfo.size + sizeof (int) * binInfo.nArgs + 1;

    char* cmdArr = (char*) calloc (1, size);
    ASSERT (cmdArr != nullptr, nullptr);

    fread (cmdArr, sizeof (char), size, binaryFile);

    return cmdArr;
}

//=====================================================================================================================================

int ExecuteDisasm (char* cmdArr, FILE* disasmFile)
{
    ASSERT (cmdArr     != nullptr, -1);
    ASSERT (disasmFile != nullptr, -1);

    size_t ip = 0;

    while (*(cmdArr + ip))
    {
        int curCmd = *(cmdArr + ip);

        #define DEF_CMD(name, num, arg, code)                       \
            case CMD_##name:                                        \
            {                                                       \
                fprintf (disasmFile, #name);                        \
                if (arg)                                            \
                {                                                   \
                    DisasmArg (curCmd, cmdArr, &ip, disasmFile);    \
                }                                                   \
                fprintf (disasmFile, "\n");                         \
                break;                                              \
            }

        #define DEF_JMP(name, num, sign)                            \
            case JMP_##name:                                        \
            {                                                       \
                fprintf (disasmFile, #name);                        \
                int argIm = 0;                                      \
                memcpy (&argIm, cmdArr + ip + 1, sizeof (int));     \
                fprintf (disasmFile, " %02X\n", argIm);             \
                ip += sizeof (int);                                 \
                break;                                              \
            }   

        switch (curCmd & ONLY_CMD)
        {
            #include "architecture.h"

            default:
                printf ("An unknown operation was encountered: %02X\n", *(cmdArr + ip));
                abort();
        }

        ip++;
    }

    #undef DEF_CMD
    #undef DEF_JMP

    fclose (disasmFile);

    return 0;
}

//=====================================================================================================================================

int DisasmArg (int curCmd, char* cmdArr, size_t* ip, FILE* disasmFile)
{
    ASSERT (cmdArr != nullptr, -1);
    ASSERT (ip     != nullptr, -1);

    fprintf (disasmFile, (curCmd & ARG_MEM) ? " [" : " ");

    if ((curCmd & ARG_IMMED) && (curCmd & ARG_REG))
    {
        int argIm = 0;
        int argRe = 0;

        memcpy (&argIm, cmdArr + *ip + 1, sizeof (int));
        memcpy (&argRe, cmdArr + *ip + 1 + sizeof (int), sizeof (int));

        fprintf (disasmFile, "%d+R%cX", argIm, 'A' + argRe - 1);

        *ip += sizeof (int) + sizeof (int);
    }

    else if (curCmd & ARG_IMMED)
    {
        int argIm = 0;

        memcpy (&argIm, cmdArr + *ip + 1, sizeof (int));

        fprintf (disasmFile, "%d", argIm);

        *ip += sizeof (int);
    }

    else if (curCmd & ARG_REG)
    {
        int argRe = 0;

        memcpy (&argRe, cmdArr + *ip + 1, sizeof (int));

        fprintf (disasmFile, "R%cX", 'A' + argRe - 1);

        *ip += sizeof (int);
    }


    else
    {
        printf ("Invalid argument: %0X", curCmd ^ ONLY_CMD);
        abort();
    }

    fprintf (disasmFile, (curCmd & ARG_MEM) ? "]" : "");

    return 0;
}

//=====================================================================================================================================
