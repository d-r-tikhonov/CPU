//=====================================================================================================================================

#include "assembler.h"

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
const char*  Signature        = "DRT";
const char*  InputFileName    = "solveQE.txt";

//=====================================================================================================================================

int Assemble (int argc, char** argv)
{
    HandleCmdArgs (argc, argv);

    type_buf_char      user_code   = {NULL, 0, 0};
    type_buf_structs   arr_structs = {NULL, 0   };

    label_field labels[LabelsMaxNum] = {};

    asm_t asm_code = {NULL, 0, 0, labels};

    ReadFile (InputFileName, &user_code, &arr_structs);

    asm_code.Ptr = (int*) calloc (user_code.Num_lines * 3, sizeof (int));
    ASSERT (asm_code.Ptr != nullptr);

    UserCodeToASM (&user_code, &arr_structs, &asm_code);
    UserCodeToASM (&user_code, &arr_structs, &asm_code);

    WriteASM (asm_code.Ptr, NameFileASM, asm_code.Size);

    ASSERT (asm_code.Ptr != nullptr);
    free (asm_code.Ptr);

    return 0;
}

//=====================================================================================================================================

int HandleCmdArgs (int argc, char** argv)
{
    for(int args_count = 1; args_count < argc; args_count++)
    {
        InputFileName = argv[args_count];
    }

    if(argc == 1)
    {
        printf ("Name of code file has not been chosen.\n"
                "The program is going to try to open default file.\n");
    }

    return 1;
}

//=====================================================================================================================================

int UserCodeToASM (type_buf_char* ptr_user_code, type_buf_structs* ptr_arr_structs, asm_t* asm_code)
{
    asm_code->Ip = 0;

    char   cmd[CmdMaxLen]   = "";
    size_t cmd_code         = 0;
    char*  cursor           = nullptr;

    for (size_t i = 0; i < ptr_user_code->Num_lines; i++)
    {
        cursor = (ptr_arr_structs->Ptr)[i].Loc;
        SkipSpace (&cursor);
        sscanf (cursor, "%s", cmd);
        cursor = cursor + strlen ((const char*) cmd);

        if (false)
        {
            printf ("FALSE in function: %s.", __func__);
        }

        #include "strcmp_for_asm.h"

        else
        {
            printf ("Error in function: %s. Unknown command!\n", __func__);
            return -1;
        }

        #undef DEF_CMD
    }

    asm_code->Size = asm_code->Ip;

    return 0;
}

//=====================================================================================================================================

int SkipSpace (char** cursor)
{
    while (**cursor == ' ') 
    {
        (*cursor)++;
    }

    return 0;
}

//=====================================================================================================================================

int SearchLabelByName (label_field* labels, char* name)
{
    size_t i = 0;

    while (i < LabelsMaxNum)
    {
        if (strcmp (labels[i].Name, name) == 0)
        {
            return i;
        }

        i++;
    }

    printf ("Error in function: %s. Label not found!\n", __func__);

    return -1;
}

//=====================================================================================================================================

int FuncLab (char* ptr_arg, asm_t* asm_code)
{
    size_t num_label = IdentifyNumLabel (ptr_arg, asm_code);

    asm_code->Labels[num_label].Value = (asm_code->Ip) + 1;

    return 0;
}

//=====================================================================================================================================

int FuncName (char* ptr_arg, asm_t* asm_code)
{
    SkipSpace (&ptr_arg);

    size_t num_label = -1;

    char label_name[MAX_LEN_LABEL_NAME] = {};

    if (sscanf (ptr_arg, "%s", label_name) == 0)
    {
        printf ("Error in function: %s. Label ERROR!\n", __func__);
        return -1;
    }

    SkipSpace (&ptr_arg);

    ptr_arg += strlen ((const char*)label_name);

    if (sscanf (ptr_arg, "%llu", &num_label) == 0)
    {
        printf ("Error in function: %s. Label ERROR!\n", __func__);        
        return -1;
    }

    strcpy (asm_code->Labels[num_label].Name, label_name);

    return 0;
}

//=====================================================================================================================================

int FuncJump (char* ptr_arg, asm_t* asm_code)
{
    size_t num_label = IdentifyNumLabel (ptr_arg, asm_code);

    (asm_code->Ptr)[(asm_code->Ip)++] = asm_code->Labels[num_label].Value;

    return 0;
}

//=====================================================================================================================================

size_t IdentifyNumLabel (char* ptr_arg, asm_t* asm_code)
{
    SkipSpace (&ptr_arg);

    int num_label = -1;

    if (!(isdigit (*ptr_arg)))
    {
        char label_name[MAX_LEN_LABEL_NAME] = {};

        if (!(sscanf (ptr_arg, "%s", label_name)))
        {
            printf ("Error in function: %s. Label ERROR!\n", __func__);        
            return -1;
        }

        num_label = SearchLabelByName(asm_code->Labels, label_name);
    }
    else
    {
        if (!(sscanf(ptr_arg, "%d", &num_label)))
        {
            printf ("Error in function: %s. Label ERROR!\n", __func__);        
            return -1;
        }
    }

    ASSERT (num_label != -1);

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
    int  arg      = 0;
    int  read_res = 0;

    char reg_name[RegNameMaxLen] = {};

    if ((read_res = sscanf (ptr_arg, ram_mask1, &arg, reg_name)) == 1)
    {
        cmd_code |= ARG_RAM;
    }

    if (read_res < 2)
    {
        if ((read_res = sscanf (ptr_arg, ram_mask2, reg_name, &arg)) == 1)    
        {
            cmd_code |= ARG_RAM;
        }
    }

    if (read_res < 2)
    {
        read_res = sscanf (ptr_arg, mask1, &arg, reg_name);
    }
    else if (read_res == 2)
    {
        (asm_code->Ptr)[(asm_code->Ip)++] = cmd_code | ARG_IMMED | ARG_REG;

        HandleRegname (asm_code, reg_name);

        asm_code->Ptr[(asm_code->Ip)++] = arg;

        return 0;
    }
    else
    {
        read_res = sscanf (ptr_arg, mask2, reg_name, &arg);
    }

    return 1;
}

//=====================================================================================================================================

int HandleNum (asm_t* asm_code, size_t cmd_code, char* ptr_arg, const char* mask1, const char* ram_mask1)
{
    int  arg      = 0;
    int  read_res = 0;

    if ((read_res = sscanf (ptr_arg, ram_mask1, &arg)) == 1)        
    {
        cmd_code |= ARG_RAM;
    }

    if (read_res < 1)
    {
        read_res = sscanf (ptr_arg, mask1, &arg);
    }

    if (read_res == 1)
    {
        (asm_code->Ptr)[(asm_code->Ip)++] = cmd_code | ARG_IMMED;
        (asm_code->Ptr)[(asm_code->Ip)++] = arg;

        return 0;
    }

    return 1;
}

//=====================================================================================================================================

int HandleReg (asm_t* asm_code, size_t cmd_code, char* ptr_arg, const char* mask1, const char* ram_mask1)
{
    int  read_res = 0;

    char reg_name[RegNameMaxLen] = {};

    if ((read_res = sscanf (ptr_arg, ram_mask1, reg_name)) == 1)        
    {
        cmd_code |= ARG_RAM;
    }

    if (read_res < 1)
    {
        read_res = sscanf (ptr_arg, mask1, reg_name);
    }

    if (read_res == 1)
    {
        (asm_code->Ptr)[(asm_code->Ip)++] = cmd_code | ARG_REG;

        HandleRegname (asm_code, reg_name);

        return 0;
    }

    return 1;
}

//=====================================================================================================================================

elem_t PutArg (size_t cmd_code, char* ptr_arg, asm_t* asm_code)
{
    ASSERT (ptr_arg != nullptr);

    int  read_res = 0;

    SkipSpace (&ptr_arg);

    read_res = HandleRegAndNum (asm_code, cmd_code, ptr_arg, "%d+%[a-z]", "%[a-z]+%d", "[%d+%[a-z]]", "[%[a-z]+%d]");

    if (!read_res)
    {
        return 0;
    }

    read_res = HandleNum (asm_code, cmd_code, ptr_arg, "%d", "[%d]");

    if (!read_res)
    {
        return 0;
    }

    read_res = HandleReg (asm_code, cmd_code, ptr_arg, "%[a-z]", "[%[a-z]]");
    if (!read_res) 
    {
        return 0;
    }

    return -1;
}

//=====================================================================================================================================

int HandleRegname (asm_t* asm_code, char* reg_name)
{
    if (reg_name[0] != 'r' || reg_name[2] != 'x')
    {
        printf ("Error in function: %s. Syntax error: register is incorrect!\n", __func__);
    }

    int reg_num = reg_name[1] - 'a';
    (asm_code->Ptr)[(asm_code->Ip)++] = reg_num;

    return 0;
}

//=====================================================================================================================================

int WriteASM (int* ptr_asm, const char* filename, size_t buf_size)
{
    FILE* file = OpenWFile (filename);
    ASSERT (file != nullptr);

    WriteHead (file, buf_size);
    
    PutBuffer (file, ptr_asm, buf_size);

    fclose (file);

    return 0;
}

//=====================================================================================================================================

int WriteHead (FILE* file, size_t buf_size)
{
    fwrite (Signature, sizeof (char), strlen (Signature), file);

    size_t version = ReadVersion (VersionFile);

    UpdateVersion (VersionFile, &version);

    WriteVersion (file, version);

    fwrite (&buf_size, sizeof (int), 1, file);

    return 0;
}

//=====================================================================================================================================

size_t ReadVersion (const char* filename)
{
    FILE* versionFile = OpenReadingFile (filename);

    if (versionFile == nullptr)
    {
        printf ("Error in function: %s. Error opening versionFile!\n", __func__);
        return -1;
    }

    size_t version = 0;

    fread (&version, sizeof (int), 1, versionFile);

    if ((fclose (versionFile)) != 0)
    {
        printf ("Error in function: %s. Error closing versionFile!\n", __func__);
    }

    return version;
}

//=====================================================================================================================================

int UpdateVersion (const char* filename, size_t* ptr_version)
{
    *ptr_version = (*ptr_version + 1) % VersionRepeat;

    FILE* versionFile = OpenWFile (filename);
    ASSERT (versionFile != nullptr);

    fwrite (ptr_version, sizeof (int), 1, versionFile);

    if ((fclose (versionFile)) != 0)
    {
        printf ("Error in function: %s. Error closing versionFile!\n", __func__);
    }

    return 0;
}

//=====================================================================================================================================

int WriteVersion (FILE* file, size_t version)
{
    fwrite (&version, sizeof (int), 1, file);
    
    return 0;
}

//=====================================================================================================================================

int ReadFile (const char* filename, type_buf_char* ptr_text_buf, type_buf_structs* ptr_arr_structs)
{
    FILE* text_file = OpenReadingFile (filename);

    if (!text_file)
    {
        return -1;
    }

    ptr_text_buf->Size = FileSize (text_file) + 1;

    ptr_text_buf->Ptr = (char*) calloc (ptr_text_buf->Size, sizeof (char));

    ReadInBuffer (text_file, ptr_text_buf);

    fclose (text_file);

    MakePointersToLines (ptr_text_buf, ptr_arr_structs);

    return 0;
}

//=====================================================================================================================================

int ReadInBuffer (FILE* file, type_buf_char* ptr_text_buf)
{
    size_t num_read_sym = fread (ptr_text_buf->Ptr, sizeof (char), ptr_text_buf->Size - 1, file);
    ASSERT (num_read_sym != 0);

    if (ptr_text_buf->Size - 1 != num_read_sym)
    {
        printf ("Error in function: %s.\n", __func__);
        return -1;
    }

    return 0;
}

//=====================================================================================================================================

FILE* OpenReadingFile (const char* filename)
{
    FILE* readableFile = fopen (filename, "rb");

    if (readableFile == nullptr)
    {
        printf ("Error in function: %s. Error opening file!\n", __func__);
        return nullptr;
    }

    return readableFile;
}

//=====================================================================================================================================

int MakePointersToLines (type_buf_char* ptr_text_buf, type_buf_structs* ptr_arr_structs)
{
    ptr_text_buf->Num_lines = GetAmountsOfLines (ptr_text_buf);

    ptr_arr_structs->Ptr  = (type_prop_line*) calloc (ptr_text_buf->Num_lines, sizeof (type_prop_line));
    ptr_arr_structs->Size = ptr_text_buf->Num_lines;

    CreateArrayStructs (ptr_text_buf, ptr_arr_structs);

    return 0;
}

//=====================================================================================================================================

void CreateArrayStructs (type_buf_char* ptr_text_buf, type_buf_structs* ptr_arr_structs)
{
    unsigned long int index_line = 0;

    char* ptr_prev_line = ptr_text_buf->Ptr;

    for (size_t i = 0; i < ptr_text_buf->Size; i++)
    {
        if (isEndOfFile ((ptr_text_buf->Ptr)[i]) == 1)
        {
            if (isLineEmpty (ptr_prev_line) == 0)
            {
                ptr_arr_structs->Ptr[index_line] = {ptr_prev_line, (ptr_text_buf->Ptr) + i - ptr_prev_line};

                index_line++;
            }

            ptr_prev_line = (ptr_text_buf->Ptr) + i + 1;
        }
    }
}

//=====================================================================================================================================

int FileSize (FILE* file)
{
    ASSERT (file != nullptr);

    struct stat buf;

    int errcode = fstat (fileno (file), &buf);
    ASSERT (errcode == 0);

    return buf.st_size;
}

//=====================================================================================================================================

int GetAmountsOfLines (type_buf_char* ptr_text_buf)
{
    ASSERT (ptr_text_buf != nullptr);

    size_t num_lines = 0;

    char* ptr_prev_line = ptr_text_buf->Ptr;

    for (size_t i = 0; i < ptr_text_buf->Size; i++)
    {
        if (*(ptr_text_buf->Ptr + i) == '\n')
        {
            if (!isLineEmpty(ptr_prev_line))
            {
                num_lines++;
            }

            ptr_prev_line = ptr_text_buf->Ptr + i + 1;
        }
    }

    if (!(isLineEmpty (ptr_prev_line)))
    {
        num_lines++;
    }

    ptr_text_buf->Num_lines = num_lines;

    return num_lines;
}

//=====================================================================================================================================

bool isLetter (char sym)
{
    if (('a' <= sym && sym <= 'z') || ('A' <= sym && sym <= 'Z'))
    {
        return true;
    }

    return false;
}

//=====================================================================================================================================

FILE* OpenWFile (const char* filename)
{
    FILE* stream = fopen (filename, "wb");

    if (stream == nullptr)
    {
        return nullptr;
    }

    return stream;
}

//=====================================================================================================================================

int PutBuffer (FILE* w_file, int* ptr_asm, size_t buf_size)
{
    size_t num_written_sym = fwrite (ptr_asm, sizeof (int), buf_size, w_file);

    if (num_written_sym != buf_size)
    {
        return -1;
    }

    return 0;
}

//=====================================================================================================================================

int isLineEmpty (char* ptr_line)
{
    while (!(isEndOfFile (*ptr_line)))
    {
        if (isLetter (*ptr_line))
        {
            return 0;
        }

        ptr_line++;
    }

    return 1;
}

//=====================================================================================================================================

bool isEndOfFile (char sym)
{
    return (sym == '\0' || sym == '\n' || sym == '\r') ? true : false;
}

//=====================================================================================================================================
