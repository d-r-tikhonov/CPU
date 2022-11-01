#include "assembler.h"
#include "config.h"

//=====================================================================================================================================

#define DEF_CMD(name, num, arg, ...) \
    else if (!stricmp(cmd, #name))   \
        {                            \
            cmd_code = num;          \
            __VA_ARGS__              \
        }                            \

//=====================================================================================================================================

const char*  VersionFile      = "version.txt";
size_t       VersionRepeat    = 1024;
const size_t CmdMaxLen        = 30;
const size_t LabelsMaxNum     = 30;
const size_t RegNameMaxLen    = 5;
const char*  NameFileASM      = "ASM.txt";
const char*  Signature        = "MDA";
char*        InputFileName    = "factorial.txt";

//=====================================================================================================================================

int assemble (int argc, char** argv)
{
    handle_cmd_args (argc, argv);

    type_buf_char      user_code         = {NULL, 0, 0};
    type_buf_structs   arr_structs       = {NULL, 0   };

    label_field labels[LabelsMaxNum] = {};

    asm_t asm_code = {NULL, 0, 0, labels};

    read_file(InputFileName, &user_code, &arr_structs);

    asm_code.Ptr = allocate_array(int, user_code.Num_lines * 3);

    Assert(asm_code.Ptr == NULL);

    UserCodeToASM(&user_code, &arr_structs, &asm_code);
    UserCodeToASM(&user_code, &arr_structs, &asm_code);

    WriteASM(asm_code.Ptr, NameFileASM`, asm_code.Size);

    FREE(asm_code.Ptr);

    return 0;
}

//=====================================================================================================================================

int handle_cmd_args(int argc, char** argv)
{
    for(int args_count = 1; args_count < argc; args_count++)
    {
        InputFileName = argv[args_count];
    }

    if(argc == 1)
    {
        printf("Name of code file has not been chosen.\n"
                "The program is going to try to open default file.\n");

        log("START IN DEFAULT MODE\n");
    }

    return 1;
}

//=====================================================================================================================================

int UserCodeToASM (type_buf_char* ptr_user_code, type_buf_structs* ptr_arr_structs, asm_t* asm_code)
{
    asm_code->Ip = 0;

    char   cmd[CmdMaxLen] = "";

    size_t cmd_code         = 0;

    char*  cursor           = NULL;

    for (size_t i = 0; i < ptr_user_code->Num_lines; i++)
    {
        cursor = (ptr_arr_structs->Ptr)[i].Loc;

        SkipSpace(&cursor);

        sscanf(cursor, "%s", cmd);

        cursor += strlen((const char*)cmd);

        log("Command found: %s\n", cmd);

        if (false)
        {
            log("Never gonna give you up");
        }

        #include "strcmp_for_asm.h"

        else
        {
            print_log(FRAMED, "COMMAND ERROR: Unexpected command while assembling");

            return -1;
        }

        #undef DEF_CMD
    }

    asm_code->Size = asm_code->Ip;

    PrintASM(asm_code);

    return 0;
}

//=====================================================================================================================================

int PrintASM (asm_t* asm_code)
{
    log("\n\n^^^^^^^ START ASM CODE ^^^^^^^\n\n");

    size_t numbers_in_size = (size_t) ceil(log10 ((double) asm_code->Size));

    for (size_t i = 0; i < asm_code->Ip; i++)
    {
        log("ASM[%0*d]: %d\n", numbers_in_size, i, (asm_code->Ptr)[i]);
    }

    log("\n=======  END ASM CODE =======\n\n");

    return 0;
}

//=====================================================================================================================================

int SkipSpace (char** cursor)
{
    while (**cursor == ' ') {(*cursor)++;}

    return 0;
}

//=====================================================================================================================================

int SearchLabelByName (label_field* labels, char* name)
{
    size_t i = 0;

    while (i < LabelsMaxNum)
    {
        if (!strcmp(labels[i].Name, name))     return i;

        i++;
    }

    print_log(FRAMED, "LABEL ERROR: No such label found");

    log("Expected to find label: %s\n", name);

    log("List labels:\n");

    for (i = 0; i < LabelsMaxNum; i++)
    {
        log("Name: %*s,      value: %d\n", MAX_LEN_LABEL_NAME, labels[i].Name, labels[i].Value);
    }

    return -1;
}

//=====================================================================================================================================

int FuncLab (char* ptr_arg, asm_t*       asm_code)
{
    size_t num_label = IdentifyNumLabel(ptr_arg, asm_code);

    asm_code->Labels[num_label].Value = (asm_code->Ip) + 1;

    return 0;
}

//=====================================================================================================================================

int FuncName (char* ptr_arg, asm_t* asm_code)
{
    log("case name\n");

    SkipSpace(&ptr_arg);

    size_t num_label = -1;

    char label_name[MAX_LEN_LABEL_NAME] = {};

    if (!sscanf(ptr_arg, "%s", label_name))
    {
        print_log(FRAMED, "LABEL ERROR: Unreadable name");

        return -1;
    }

    SkipSpace(&ptr_arg);

    ptr_arg += strlen((const char*)label_name);

    if (!sscanf(ptr_arg, "%d", &num_label))
    {
        print_log(FRAMED, "LABEL ERROR: Unreadable name");

        return -1;
    }

    strcpy(asm_code->Labels[num_label].Name, label_name);

    return 0;
}

//=====================================================================================================================================

int FuncJump (char* ptr_arg, asm_t* asm_code)
{
    log ("case jump\n");

    size_t num_label = IdentifyNumLabel(ptr_arg, asm_code);

    (asm_code->Ptr)[(asm_code->Ip)++] = asm_code->Labels[num_label].Value;

    return 0;
}

//=====================================================================================================================================

size_t IdentifyNumLabel (char* ptr_arg, asm_t* asm_code)
{
    SkipSpace(&ptr_arg);

    int num_label = -1;

    if (!isdigit(*ptr_arg))
    {
        char label_name[MAX_LEN_LABEL_NAME] = {};

        if (!sscanf(ptr_arg, "%s", label_name))
        {
            print_log(FRAMED, "LABEL ERROR: Unreadable name");

            return -1;
        }

        num_label = SearchLabelByName(asm_code->Labels, label_name);
    }
    else
    {
        if (!sscanf(ptr_arg, "%d", &num_label))
        {
            print_log(FRAMED, "LABEL ERROR: Unreadable digit");

            return -1;
        }
    }

    Assert(num_label == -1);

    if (num_label == -1)
    {
        return -1;
    }

    return num_label;
}

//=====================================================================================================================================

int HandleRegAndNum (asm_t* asm_code, size_t cmd_code, char* ptr_arg, const char* mask1, const char* mask2, 
                    const char* ram_mask1, const char* ram_mask2)
{
    int  arg         = 0;

    int  read_res    = 0;

    char reg_name[RegNameMaxLen] = {};

    if (read_res = sscanf(ptr_arg, ram_mask1, &arg, reg_name))         cmd_code |= ARG_RAM;

    if (read_res < 2)
        if (read_res = sscanf(ptr_arg, ram_mask2, reg_name, &arg))    cmd_code |= ARG_RAM;

    if (read_res < 2)
        read_res = sscanf(ptr_arg, mask1, &arg, reg_name);

    if (!read_res < 2)
        read_res = sscanf(ptr_arg, mask2, reg_name, &arg);

    if (read_res == 2)
    {
        log("case [d+rcx]\n");

        (asm_code->Ptr)[(asm_code->Ip)++] = cmd_code | ARG_IMMED | ARG_REG;

        HandleRegname(asm_code, reg_name);

        asm_code->Ptr[(asm_code->Ip)++] = arg;

        return 0;
    }

    return 1;
}

//=====================================================================================================================================

int HandleNum (asm_t* asm_code, size_t cmd_code, char* ptr_arg, const char* mask1, const char* ram_mask1)
{
    int  arg         = 0;

    int  read_res    = 0;

    if (read_res = sscanf(ptr_arg, ram_mask1, &arg))        cmd_code |= ARG_RAM;

    if (read_res < 1)
        read_res = sscanf(ptr_arg, mask1, &arg);

    if (read_res == 1)
    {
        log ("case d\n");

        (asm_code->Ptr)[(asm_code->Ip)++] = cmd_code | ARG_IMMED;

        (asm_code->Ptr)[(asm_code->Ip)++] = arg;

        return 0;
    }

    return 1;
}

//=====================================================================================================================================

int HandleReg (asm_t* asm_code, size_t cmd_code, char* ptr_arg, const char* mask1, const char* ram_mask1)
{
    int  read_res    = 0;

    char reg_name[RegNameMaxLen] = {};

    if (read_res = sscanf(ptr_arg, ram_mask1, reg_name))        cmd_code |= ARG_RAM;

    if (read_res < 1)
        read_res = sscanf(ptr_arg, mask1, reg_name);

    if (read_res == 1)
    {
        log("case rcx\n");

        (asm_code->Ptr)[(asm_code->Ip)++] = cmd_code | ARG_REG;

        HandleRegname(asm_code, reg_name);

        return 0;
    }

    return 1;
}

//=====================================================================================================================================

elem_t PutArg (size_t cmd_code, char* ptr_arg, asm_t* asm_code)
{
    Assert(ptr_arg == NULL);

    log("cmd_code: %d\n", cmd_code);

    int  read_res    = 0;

    SkipSpace(&ptr_arg);

    log("ptr_arg: %p, first sym to scan: %c\n", ptr_arg, *ptr_arg);

    read_res = HandleRegAndNum(asm_code, cmd_code, ptr_arg, "%d+%[a-z]",
                                                            "%[a-z]+%d",
                                                            "[%d+%[a-z]]",
                                                            "[%[a-z]+%d]");
    if (!read_res)       return 0;

    read_res = HandleNum (asm_code, cmd_code, ptr_arg, "%d",
                                                       "[%d]");
    if (!read_res)       return 0;

    read_res = HandleReg (asm_code, cmd_code, ptr_arg, "%[a-z]",
                                                       "[%[a-z]]");
    if (!read_res)       return 0;

    log("No argument found\n");

    return -1;
}

//=====================================================================================================================================

int HandleRegname (asm_t* asm_code, char* reg_name)
{
    if (reg_name[0] != 'r' || reg_name[2] != 'x')
    {
        print_log(FRAMED, "Syntax Error: name of register is invalid");
    }

    int reg_num = reg_name[1] - 'a';

    log("reg_num: %d\n\n", reg_num);

    (asm_code->Ptr)[(asm_code->Ip)++] = reg_num;

    return 0;
}

//=====================================================================================================================================

int read_file (const char* filename, type_buf_char* ptr_text_buf, type_buf_structs* ptr_arr_structs)
{
    FILE* text_file = open_file_rmode(filename);

    if (!text_file)    return -1;

    ptr_text_buf->Size = safe_def_int(get_file_size(text_file) + 1, 1);

    log("Size of file is: %d bytes.\n", ptr_text_buf->Size);

    ptr_text_buf->Ptr = allocate_array(char, ptr_text_buf->Size);

    text_to_buffer(text_file, ptr_text_buf);

    fclose(text_file);

    make_pointers_to_lines(ptr_text_buf, ptr_arr_structs);

    return 0;
}

//=====================================================================================================================================

int make_pointers_to_lines (type_buf_char* ptr_text_buf, type_buf_structs* ptr_arr_structs)
{
    ptr_text_buf   ->Num_lines = count_lines(ptr_text_buf);

    ptr_arr_structs->Ptr       = allocate_array(type_prop_line , ptr_text_buf->Num_lines);

    ptr_arr_structs->Size      = ptr_text_buf->Num_lines;

    create_array_structs(ptr_text_buf, ptr_arr_structs);

    return 0;
}

//=====================================================================================================================================

FILE* open_file_rmode (const char* filename)
{
    FILE* r_file = fopen(filename, "rb");

    if(r_file == NULL)
    {
        print_log(FRAMED, "File not found");

        log("File %s not found\n", filename);

        return NULL;
    }

    return r_file;
}

//=====================================================================================================================================

int text_to_buffer (FILE* file, type_buf_char* ptr_text_buf)
{
    size_t num_read_sym = fread(ptr_text_buf->Ptr,
                                sizeof(char),
                                ptr_text_buf->Size - 1,
                                file);

    Assert(num_read_sym == 0);

    log("buffer size: %d\n"
        "fread number of symbols: %d\n", ptr_text_buf->Size, num_read_sym);

    if (ptr_text_buf->Size - 1 != num_read_sym)
    {
        printf("Program dies from cringe encoding\n");

        return -1;
    }

    return 0;
}

//=====================================================================================================================================

int create_array_structs (type_buf_char* ptr_text_buf, type_buf_structs* ptr_arr_structs)
{
    unsigned long int index_line = 0;

    char* ptr_prev_line = ptr_text_buf->Ptr;

    for (size_t i = 0; i < ptr_text_buf->Size; i++)
    {
        if (end_of_line((ptr_text_buf->Ptr)[i]))
        {
            if (!is_line_empty(ptr_prev_line))
            {
                (ptr_arr_structs->Ptr)[index_line] = {ptr_prev_line,
                                                     (ptr_text_buf->Ptr) + i - ptr_prev_line};

                index_line++;
            }

            ptr_prev_line = (ptr_text_buf->Ptr) + i + 1;
        }
    }

    return 1;
}

//=====================================================================================================================================

int count_lines (type_buf_char* ptr_text_buf)
{
    Assert(ptr_text_buf == NULL);

    size_t num_lines = 0;

    char* ptr_prev_line = ptr_text_buf->Ptr;

    for(size_t i = 0; i < ptr_text_buf->Size; i++)
    {
        if (*(ptr_text_buf->Ptr + i) == '\n')
        {
            if (!is_line_empty(ptr_prev_line))
            {
                num_lines++;
            }
            ptr_prev_line = ptr_text_buf->Ptr + i + 1;
        }
    }

    if (!is_line_empty(ptr_prev_line))
    {
        num_lines++;
    }

    ptr_text_buf->Num_lines = num_lines;

    log("Num lines: %d\n", num_lines);

    return num_lines;
}

//=====================================================================================================================================

int is_line_empty (char* ptr_line)
{
    while (!end_of_line(*ptr_line))
    {
        if (isletter(*ptr_line))
        {
            return 0;
        }
        ptr_line++;
    }
    return 1;
}

//=====================================================================================================================================

int get_file_size (FILE* file)
{
    Assert(file == NULL);

    struct stat buf;

    int errcode = fstat(fileno(file), &buf);

    Assert(errcode != 0);

    printf("File size = %d\n", (int)buf.st_size);

    return buf.st_size;
}

//=====================================================================================================================================

bool isletter (char sym)
{
    if (('a' <= sym && sym <= 'z') ||
        ('A' <= sym && sym <= 'Z') ||
        ('�' <= sym && sym <= '�') ||
        ('�' <= sym && sym <= '�'))
    {
        return true;
    }

    return false;
}

//=====================================================================================================================================

int end_of_line (char sym)
{
    return (sym == '\0' || sym == '\n' || sym == '\r') ? true : false;
}

//=====================================================================================================================================

int WriteASM (int* ptr_asm, const char* filename, size_t buf_size)
{
    FILE* file = open_Wfile (filename);

    WriteHead (file, buf_size);

    put_buffer (file, ptr_asm, buf_size);

    fclose (file);

    return 0;
}

//=====================================================================================================================================

int WriteHead (FILE* file, size_t buf_size)
{
    log ("length signature: %d\n", strlen (Signature));

    fwrite (Signature, sizeof (char), strlen (Signature), file);

    size_t version = ReadVersion (VersionFile);

    UpdateVersion (VersionFile, &version);

    WriteVersion (file, version);

    log ("written size: %d\n", buf_size);

    fwrite (&buf_size, sizeof(int), 1, file);

    return 0;
}

//=====================================================================================================================================

FILE* open_Wfile (const char* filename)
{
    FILE* w_file = fopen(filename, "wb");

    if(w_file == NULL)
    {
        log("Opening file to write %s failed\n", filename);

        return NULL;
    }

    return w_file;
}

//=====================================================================================================================================

int put_buffer (FILE* w_file, int* ptr_asm, size_t buf_size)
{
    size_t num_written_sym = fwrite (ptr_asm, sizeof (int), buf_size, w_file);

    if (num_written_sym != buf_size)
    {
        print_log(FRAMED, "Error in writing to file\n");

        return -1;
    }

    log ("File is written successfully\n");

    return 0;
}

//=====================================================================================================================================

size_t ReadVersion (const char* filename)
{
    FILE* vers_file = open_file_rmode(filename);

    if (vers_file == NULL) 
        return -1;

    size_t version = 0;

    fread (&version, sizeof(int), 1, vers_file);

    fclose (vers_file);

    return version;
}

//=====================================================================================================================================

int UpdateVersion(const char* filename, size_t* ptr_version)
{
    *ptr_version = (*ptr_version + 1) % VersionRepeat;

    FILE* vers_file = open_Wfile (filename);

    fwrite (ptr_version, sizeof(int), 1, vers_file);

    fclose (vers_file);

    return 0;
}

//=====================================================================================================================================

int WriteVersion (FILE* file, size_t version)
{
    fwrite (&version, sizeof(int), 1, file);

    return 0;
}

//=====================================================================================================================================
