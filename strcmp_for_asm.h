#ifndef COMMANDS_FOR_CPU_H_
#define COMMANDS_FOR_CPU_H_

//=====================================================================================================================================

#define DO_JUMP        {cpu->ip = cpu->code[(cpu->ip)+1] - 2;}

#define DO_POP         ({(double)StackPop(stk) / ACCURACY;})

#define DO_PUSH(arg)   StackPush(stk, (arg) * ACCURACY);

#define REMEMBER_CALL  {StackPush(cpu->StkCalls, cpu->ip);};

#define RETURN_TO_CALL {cpu->ip = StackPop(cpu->StkCalls) + 1;};

#define SKIP_ARG       {(cpu->ip)++;};

#define COND_JUMP(condition)                                 \
{                                                            \
    if (condition)                                           \
    {                                                        \
        DO_JUMP;                                             \
    }                                                        \
    else                                                     \
    {                                                        \
        SKIP_ARG;                                            \
    }                                                        \
}

// for assembler

#define WRITE_CMD_NUM   {(asm_code->Ptr)[(asm_code->Ip)++] = cmd_code;};

#define WRITE_ARG_JUMP  {FuncJump(cursor, asm_code);};

#define WRITE_STACK_ARG {PutArg(cmd_code, cursor, asm_code);};

#define WRITE_JUMP                                                           \
{                                                                            \
    WRITE_CMD_NUM;                                                           \
    WRITE_ARG_JUMP;                                                          \
}

DEF_CMD(HLT, 0, 0,
#ifndef RUN_MODE
    WRITE_CMD_NUM;
#else
    {
        printf ("MEOW!\n");
        return 0;
    };
#endif
)

DEF_CMD(PUSH, 1, 2,
#ifndef RUN_MODE
    WRITE_STACK_ARG;
#else
{
    DO_PUSH(((double)*GetArg(cpu)) / ACCURACY);
};
#endif
)

DEF_CMD(POP, 2, 2,
#ifndef RUN_MODE
    WRITE_STACK_ARG;
#else
{
    int* ptr = GetArg(cpu);

    *ptr = DO_POP * ACCURACY;
};
#endif
)

DEF_CMD(ADD , 3, 0,
#ifndef RUN_MODE
    WRITE_CMD_NUM;
#else
{
    double a = DO_POP;
    double b = DO_POP;

    DO_PUSH(a + b);
};
#endif
)

DEF_CMD(SUB , 4, 0,
#ifndef RUN_MODE
    WRITE_CMD_NUM;
#else
{
    double a = DO_POP;
    double b = DO_POP;

    DO_PUSH(b - a);
};
#endif
)

DEF_CMD(MUL , 5, 0,
#ifndef RUN_MODE
    WRITE_CMD_NUM;
#else
{
    DO_PUSH(DO_POP * DO_POP);
};
#endif
)

DEF_CMD(DIV , 6, 0,
#ifndef RUN_MODE
    WRITE_CMD_NUM;
#else
{
    double a = DO_POP;
    double b = DO_POP;

    if (a == 0)
    {
        printf ("ZERO DIVISION ERROR: can't divide by zero\n");
        return ZERO_DIVISION;
    }
    else
    {
        DO_PUSH(b / a);
    }
};
#endif
)

DEF_CMD(IN  , 7, 0,
#ifndef RUN_MODE
    WRITE_CMD_NUM;
#else
{
    printf ("enter a number: ");

    int value = 0;

    while (!scanf("%d", &value))
    {
        printf ("please, enter a fucking number:\n");
        SkipNewLines ();
    }

    SkipNewLines ();

    DO_PUSH(value);
};
#endif
)

DEF_CMD(OUT , 8, 0,
#ifndef RUN_MODE
    WRITE_CMD_NUM;
#else
{
    double val = DO_POP;

    if (floor(val) == val)          printf("Out print is %d \n", (int)val);
    else                            printf("Out print is %lg\n",      val);
};
#endif
)

DEF_CMD(JUMP, 9, 1,
#ifndef RUN_MODE
    WRITE_JUMP;
#else
{
    DO_JUMP;
};
#endif
)

DEF_CMD(JB, 10, 1,
#ifndef RUN_MODE
    WRITE_JUMP;
#else
{
    COND_JUMP(DO_POP < DO_POP);
};
#endif
)

DEF_CMD(JBE, 11, 1,
#ifndef RUN_MODE
    WRITE_JUMP;
#else
{
    COND_JUMP(DO_POP <= DO_POP);
};
#endif
)

DEF_CMD(JA, 12, 1,
#ifndef RUN_MODE
    WRITE_JUMP;
#else
{
    COND_JUMP(DO_POP > DO_POP);
};
#endif
)

DEF_CMD(JAE, 13, 1,
#ifndef RUN_MODE
    WRITE_JUMP;
#else
{
    COND_JUMP(DO_POP >= DO_POP);
};
#endif
)

DEF_CMD(JE, 14, 1,
#ifndef RUN_MODE
    WRITE_JUMP;
#else
{
    COND_JUMP(DO_POP == DO_POP);
};
#endif
)

DEF_CMD(JNE, 15, 1,
#ifndef RUN_MODE
    WRITE_JUMP;
#else
{
    COND_JUMP(DO_POP != DO_POP);
};
#endif
)
//directive
DEF_CMD(NAME, 16, 0,
#ifndef RUN_MODE
{
    FuncName(cursor, asm_code);
}
#endif
)
//directive
DEF_CMD(LBL , 17, 0,
#ifndef RUN_MODE
{
    FuncLab(cursor, asm_code);
}
#endif
)

DEF_CMD(DRAW, 18, 0,
#ifndef RUN_MODE
    WRITE_CMD_NUM;
#else
{
    PrintRAM(BIN_FORMAT, cpu, 100); //const, static screen
};
#endif
)

DEF_CMD(CALL, 19, 1,
#ifndef RUN_MODE
    WRITE_JUMP;
#else
{
    REMEMBER_CALL;

    DO_JUMP;
};
#endif
)

DEF_CMD(RET, 20, 0,
#ifndef RUN_MODE
    WRITE_CMD_NUM;
#else
{
    RETURN_TO_CALL;
};
#endif
)

DEF_CMD(COPY, 21, 0,
#ifndef RUN_MODE
    WRITE_CMD_NUM;
#else
{
    double val = DO_POP;

    DO_PUSH(val);
    DO_PUSH(val);
};
#endif
)

DEF_CMD(SQRT, 22, 0,
#ifndef RUN_MODE
    WRITE_CMD_NUM;
#else
{
    double val = DO_POP;
    double sqrt_val = sqrt(val);

    DO_PUSH(sqrt_val);
};
#endif
)

DEF_CMD(CLEAR, 23, 0,
#ifndef RUN_MODE
    WRITE_CMD_NUM;
#else
{
#ifdef TX_NECESSARY
    txClearConsole();
#endif
;
};
#endif
)

//1 file for asm and proc

//=====================================================================================================================================

#endif
