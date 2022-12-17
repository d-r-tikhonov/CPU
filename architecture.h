//=====================================================================================================================================

#define POP          StackPop   (&cpu->stk)
#define PUSH(arg)    StackPush  (&cpu->stk, (arg))
#define VAR          int
#define GET_PUSH_ARG GetPushArg (currentCmd, &ip, cpu) 
#define GET_POP_ARG  GetPopArg  (currentCmd, &ip, cpu)
#define GET_JMP_ARG  GetJumpArg (&ip, cpu)
#define INDEX_UP     ip++

//=====================================================================================================================================

#define BAD_DIV                                                 \
    {                                                           \
        printf ("Error! Attempt to divide by zero!\n");         \
        abort();                                                \
    }

#define BAD_INP                                                 \
    {                                                           \
        printf ("Incorrect input value\n");                     \
        abort();                                                \
    }

//=====================================================================================================================================

DEF_CMD(HLT,  0, 0, 
{
    printf ("\nThe program is completed!\n");
})

DEF_CMD(PUSH, 1, 1, 
{
    INDEX_UP;
    VAR arg = GET_PUSH_ARG; 
    PUSH (arg);
})

DEF_CMD(ADD, 2, 0, 
{
    VAR secondNum = POP;
    VAR firstNum  = POP;

    PUSH (firstNum + secondNum);
})

DEF_CMD(SUB, 3, 0,
{
    VAR secondNum = POP;
    VAR firstNum  = POP;
    PUSH (firstNum - secondNum);
})

DEF_CMD(MUL, 4, 0,
{
    VAR secondNum = POP;
    VAR firstNum  = POP;

    PUSH (secondNum * firstNum);
})

DEF_CMD(DIV, 5, 0,
{
    VAR secondNum = POP;
    VAR firstNum  = POP;
    
    if (secondNum == 0)
        BAD_DIV;
    
    PUSH (firstNum / secondNum);
})

DEF_CMD(OUT, 6, 0,
{
    printf ("Your answer: ");

    printf ("%d\n", POP);
})

DEF_CMD(INP, 7, 0,
{
    printf ("Enter a number: ");

    VAR num  = 0;

    if (scanf ("%d", &num) != 1)
        BAD_INP;

    PUSH (num);
})

DEF_CMD(POP, 8, 1,
{
    INDEX_UP;
    VAR* arg = GET_POP_ARG;
    *arg = POP;
})

DEF_CMD(INF, 18, 0,
{
    printf ("Infinite number of roots.\n");
})

DEF_CMD(NO_SOL, 19, 0,
{
    printf ("No solutions.\n");
})

DEF_CMD(ROOT_PR, 20, 0,
{
    VAR INTEG = POP;
    VAR FRAC  = POP;
    
    printf("%d,%d\n", INTEG, FRAC);
})


DEF_CMD(SQRT, 21, 0, 
{
    VAR arg = floor (sqrt (POP));
    PUSH (arg);
})

DEF_CMD(MINUS_PR, 22, 0,
{
    printf ("-");
})

//=====================================================================================================================================

DEF_JMP(JMP, 10, || 1 ||)

DEF_JMP(JA, 11, >)

DEF_JMP(JAE, 12, >=)

DEF_JMP(JE, 13, ==)

DEF_JMP(JBE, 14, <=)

DEF_JMP(JB, 15, <)

DEF_JMP(JNE, 16, !=)

//=====================================================================================================================================