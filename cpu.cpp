//=====================================================================================================================================

#include "architecture.h"

//=====================================================================================================================================

int Run (int argc, const char* argv[])
{
    ASSERT (argc != 2, 1);
    
    FILE* data = fopen (argv[2], "r");
    ASSERT (data != nullptr, 1);

    size_t nMemb    = 0;
    int*   cmdArr   = nullptr;

    cmdArr = ReadingBinFile (cmdArr, &nMemb, data);

    Execute (cmdArr, &nMemb);

    free (cmdArr);
    fclose (data);

    return 0;
}

//=====================================================================================================================================

int* ReadingBinFile (int* cmdArr, size_t* nMemb, FILE* binaryFile)
{
    cmd_t binInfo = {};
    fread (&binInfo, sizeof (cmd_t), 1, binaryFile);

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

    cmdArr = (int*) calloc (binInfo.nInt + 1, sizeof (int));
    fread (cmdArr, sizeof (int), binInfo.nInt, binaryFile);
    *nMemb = binInfo.nInt;
    
    return cmdArr;
}

//=====================================================================================================================================

int Execute (int* cmdArr, size_t* nMemb)
{
    stack_t stk = {};
    StackCtor (&stk);

    size_t index = 0;

    int firstNum    = 0;
    int secondNum   = 0;

    while (index <= *nMemb)
    {
        #define CALC(stk, sign)                         \
            firstNum    = StackPop (stk);               \
            secondNum   = StackPop (stk);               \
            StackPush (stk, firstNum sign secondNum);  

        switch (cmdArr[index])
        {
            case CMD_PUSH:
            {
                index = index + 1;

                printf ("push %d\n", cmdArr[index]);

                StackPush (&stk, cmdArr[index]);
                break;
            }

            case CMD_ADD:
            {
                printf ("add\n");

                CALC(&stk, +);
                break;
            }

            case CMD_SUB:
            {
                printf ("sub\n");

                CALC(&stk, -);
                break;
            }

            case CMD_MUL:
            {
                printf ("mul\n");

                CALC(&stk, *);
                break;
            }

            case CMD_DIV:
            {
                printf ("div\n");

                CALC(&stk, /);
                break;
            }

            case CMD_OUT:
            {
                printf ("out\n");

                printf ("%d\n", StackPop (&stk));
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

        index++;
    }

    #undef CALC

    StackDtor (&stk);

    return 0;
}

//=====================================================================================================================================