//=====================================================================================================================================

#include "assembler.h"

//=====================================================================================================================================

const char*  VersionFile      = "version.txt";
size_t       VersionRepeat    = 1024;
const size_t RegNameMaxLen    = 5;
const char*  NameFileASM      = "asm.bin";
const char*  Signature        = "SGN";
const char*  InputFileName    = "quadraticSolver.asm";

//=====================================================================================================================================

#define DEF_CMD(name, num, arg, ...) \
    else if (!stricmp(cmd, #name))   \
        {                            \
            cmd_code = num;          \
            __VA_ARGS__              \
        }                            \

//=====================================================================================================================================

int Assemble (int argc, char** argv)
{
    HandleCmdArgs (argc, argv);

    type_buf_char      user_code   = {nullptr, 0, 0};
    type_buf_structs   arr_structs = {nullptr, 0   };

    label_field labels[MaxLen] = {};

    asm_t asm_code = {nullptr, 0, 0, labels};

    ReadFile (InputFileName, &user_code, &arr_structs);

    asm_code.ptr = (int*) calloc (user_code.Num_lines * 3, sizeof (int));
    ASSERT (asm_code.ptr != nullptr);

    UserCodeToASM (&user_code, &arr_structs, &asm_code);
    UserCodeToASM (&user_code, &arr_structs, &asm_code);

    WriteASM (asm_code.ptr, NameFileASM, asm_code.size);

    ASSERT (asm_code.ptr != nullptr);
    free (asm_code.ptr);

    return 0;
}

//=====================================================================================================================================

int HandleCmdArgs (int argc, char** argv)
{
    for (int args_count = 1; args_count < argc; args_count++)
    {
        InputFileName = argv[args_count];
    }

    if (argc == 1)
    {
        printf ("Name of code file has not been chosen.\n"
                "The program is going to try to open default file.\n");
    }

    return 1;
}

//=====================================================================================================================================

int UserCodeToASM (type_buf_char* ptr_user_code, type_buf_structs* ptr_arr_structs, asm_t* asm_code)
{
    asm_code->ip = 0;

    char   cmd[MaxLen]      = "";
    size_t cmd_code         = 0;
    char*  cursor           = nullptr;

    for (size_t i = 0; i < ptr_user_code->Num_lines; i++)
    {
        cursor = (ptr_arr_structs->ptr)[i].Loc;
        SkipSpace (&cursor);
        sscanf (cursor, "%s", cmd);
        cursor = cursor + strlen ((const char*) cmd);

        if (false)
        {
            printf ("FALSE in function: %s.", __func__);
        }

        #include "architecture.h"

        else
        {
            printf ("Error in function: %s. Unknown command!\n", __func__);
            return -1;
        }

        #undef DEF_CMD
    }

    asm_code->size = asm_code->ip;

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
    for (size_t i = 0; i < MaxLen; i++)
    {
        if (strcmp (labels[i].name, name) == 0)
        {
            return i;
        }
    }

    printf ("Error in function: %s. Label not found!\n", __func__);

    return -1;
}

//=====================================================================================================================================

int FuncLab (char* argPtr, asm_t* asm_code)
{
    size_t num_label = IdentifyNumLabel (argPtr, asm_code);

    asm_code->labels[num_label].value = asm_code->ip + 1;

    return 0;
}

//=====================================================================================================================================

int FuncName (char* argPtr, asm_t* asm_code)
{
    SkipSpace (&argPtr);

    size_t num_label = -1;

    char label_name[MaxLen] = {};

    if (sscanf (argPtr, "%s", label_name) == 0)
    {
        printf ("Error in function: %s. Label ERROR!\n", __func__);
        return -1;
    }

    SkipSpace (&argPtr);

    argPtr += strlen ((const char*)label_name);

    if (sscanf (argPtr, "%llu", &num_label) == 0)
    {
        printf ("Error in function: %s. Label ERROR!\n", __func__);        
        return -1;
    }

    strcpy (asm_code->labels[num_label].name, label_name);

    return 0;
}

//=====================================================================================================================================

int FuncJump (char* argPtr, asm_t* asm_code)
{
    size_t num_label = IdentifyNumLabel (argPtr, asm_code);

    (asm_code->ptr)[(asm_code->ip)++] = asm_code->labels[num_label].value;

    return 0;
}

//=====================================================================================================================================

size_t IdentifyNumLabel (char* argPtr, asm_t* asm_code)
{
    SkipSpace (&argPtr);

    int numLabel = -1;

    if (isdigit (*argPtr) == 0)
    {
        char label_name[MaxLen] = {};

        if (sscanf (argPtr, "%s", label_name) == 0)
        {
            printf ("Error in function: %s. Label ERROR!\n", __func__);        
            return -1;
        }

        numLabel = SearchLabelByName (asm_code->labels, label_name);
    }
    else
    {
        if (sscanf (argPtr, "%d", &numLabel) == 0)
        {
            printf ("Error in function: %s. Label ERROR!\n", __func__);        
            return -1;
        }
    }

    ASSERT (numLabel != -1);

    if (numLabel == -1)
    {
        return -1;
    }

    return numLabel;
}

//=====================================================================================================================================

int HandleRegAndNum (asm_t* asm_code, size_t cmd_code, char* argPtr, const char* mask1, const char* mask2, 
                    const char* ram_mask1, const char* ram_mask2)
{
    int  arg      = 0;
    int  read_res = 0;

    char reg_name[RegNameMaxLen] = {};

    if ((read_res = sscanf (argPtr, ram_mask1, &arg, reg_name)) == 1)
    {
        cmd_code |= ARG_RAM;
    }

    if (read_res < 2)
    {
        if ((read_res = sscanf (argPtr, ram_mask2, reg_name, &arg)) == 1)    
        {
            cmd_code |= ARG_RAM;
        }
    }

    if (read_res < 2)
    {
        read_res = sscanf (argPtr, mask1, &arg, reg_name);
    }
    else if (read_res == 2)
    {
        (asm_code->ptr)[(asm_code->ip)++] = cmd_code | ARG_IMMED | ARG_REG;

        HandleRegname (asm_code, reg_name);

        asm_code->ptr[(asm_code->ip)++] = arg;

        return 0;
    }
    else
    {
        read_res = sscanf (argPtr, mask2, reg_name, &arg);
    }

    return 1;
}

//=====================================================================================================================================

int HandleNum (asm_t* asm_code, size_t cmd_code, char* argPtr, const char* mask1, const char* ram_mask1)
{
    int  arg        = 0;
    int  readResult = 0;

    if ((readResult = sscanf (argPtr, ram_mask1, &arg)) == 1)        
    {
        cmd_code |= ARG_RAM;
    }

    if (readResult < 1)
    {
        readResult = sscanf (argPtr, mask1, &arg);
    }

    if (readResult == 1)
    {
        (asm_code->ptr)[(asm_code->ip)++] = cmd_code | ARG_IMMED;
        (asm_code->ptr)[(asm_code->ip)++] = arg;

        return 0;
    }

    return 1;
}

//=====================================================================================================================================

int HandleReg (asm_t* asm_code, size_t cmd_code, char* argPtr, const char* mask1, const char* ram_mask1)
{
    int  readResult = 0;

    char reg_name[RegNameMaxLen] = {};

    if ((readResult = sscanf (argPtr, ram_mask1, reg_name)) == 1)        
    {
        cmd_code |= ARG_RAM;
    }

    if (readResult < 1)
    {
        readResult = sscanf (argPtr, mask1, reg_name);
    }

    if (readResult == 1)
    {
        (asm_code->ptr)[(asm_code->ip)++] = cmd_code | ARG_REG;

        HandleRegname (asm_code, reg_name);

        return 0;
    }

    return 1;
}

//=====================================================================================================================================

elem_t PutArg (size_t cmd_code, char* argPtr, asm_t* asm_code)
{
    ASSERT (argPtr != nullptr);

    int  readResult = 0;

    SkipSpace (&argPtr);

    readResult = HandleRegAndNum (asm_code, cmd_code, argPtr, "%d+%[a-z]", "%[a-z]+%d", "[%d+%[a-z]]", "[%[a-z]+%d]");

    if (!readResult)
    {
        return 0;
    }

    readResult = HandleNum (asm_code, cmd_code, argPtr, "%d", "[%d]");

    if (!readResult)
    {
        return 0;
    }

    readResult = HandleReg (asm_code, cmd_code, argPtr, "%[a-z]", "[%[a-z]]");
    if (!readResult) 
    {
        return 0;
    }

    return -1;
}

//=====================================================================================================================================

int HandleRegname (asm_t* asm_code, char* regName)
{
    if (regName[0] != 'r' || regName[2] != 'x')
    {
        printf ("Error in function: %s. Syntax error: register is incorrect!\n", __func__);
    }

    int reg_num = regName[1] - 'a';
    (asm_code->ptr)[(asm_code->ip)++] = reg_num;

    return 0;
}

//=====================================================================================================================================

int WriteASM (int* asmPtr, const char* fileName, size_t bufferSize)
{
    FILE* stream = OpenWFile (fileName);
    ASSERT (stream != nullptr);

    WriteHead (stream, bufferSize);
    
    PutBuffer (stream, asmPtr, bufferSize);

    if (fclose (stream) != 0)
    {
        printf ("Error in function: %s. Error closing file!\n", __func__);
        return errno;
    }

    return 0;
}

//=====================================================================================================================================

int WriteHead (FILE* file, size_t bufferSize)
{
    fwrite (Signature, sizeof (char), strlen (Signature), file);

    size_t version = ReadVersion (VersionFile);

    UpdateVersion (VersionFile, &version);

    WriteVersion (file, version);

    fwrite (&bufferSize, sizeof (int), 1, file);

    return 0;
}

//=====================================================================================================================================

size_t ReadVersion (const char* fileName)
{
    FILE* versionFile = OpenReadingFile (fileName);

    if (versionFile == nullptr)
    {
        printf ("Error in function: %s. Error opening versionFile!\n", __func__);
        return -1;
    }

    size_t version = 0;

    fread (&version, sizeof (int), 1, versionFile);

    if (fclose (versionFile) != 0)
    {
        printf ("Error in function: %s. Error closing versionFile!\n", __func__);
        return errno;
    }

    return version;
}

//=====================================================================================================================================

int UpdateVersion (const char* fileName, size_t* versionPtr)
{
    *versionPtr = (*versionPtr + 1) % VersionRepeat;

    FILE* versionFile = OpenWFile (fileName);
    ASSERT (versionFile != nullptr);

    fwrite (versionPtr, sizeof (int), 1, versionFile);

    if (fclose (versionFile) != 0)
    {
        printf ("Error in function: %s. Error closing versionFile!\n", __func__);
        return errno;
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
    FILE* stream = OpenReadingFile (filename);

    if (stream == nullptr)
    {
        return -1;
    }

    ptr_text_buf->size = FileSize (stream) + 1;

    ptr_text_buf->ptr = (char*) calloc (ptr_text_buf->size, sizeof (char));

    ReadInBuffer (stream, ptr_text_buf);

    if (fclose (stream) != 0)
    {
        printf ("Error in function: %s.\n", __func__);
        return errno;
    }

    MakePointersToLines (ptr_text_buf, ptr_arr_structs);

    return 0;
}

//=====================================================================================================================================

int ReadInBuffer (FILE* file, type_buf_char* ptr_text_buf)
{
    size_t num_read_sym = fread (ptr_text_buf->ptr, sizeof (char), ptr_text_buf->size - 1, file);
    ASSERT (num_read_sym != 0);

    if (ptr_text_buf->size - 1 != num_read_sym)
    {
        printf ("Error in function: %s.\n", __func__);
        return -1;
    }

    return 0;
}

//=====================================================================================================================================

FILE* OpenReadingFile (const char* filename)
{
    FILE* stream = fopen (filename, "rb");
    
    if (stream == nullptr)
    {
        printf ("Error in function: %s. Error opening file!\n", __func__);
        return nullptr;
    }

    return stream;
}

//=====================================================================================================================================

int MakePointersToLines (type_buf_char* ptr_text_buf, type_buf_structs* ptr_arr_structs)
{
    ptr_text_buf->Num_lines = GetAmountsOfLines (ptr_text_buf);

    ptr_arr_structs->ptr  = (type_prop_line*) calloc (ptr_text_buf->Num_lines, sizeof (type_prop_line));
    ptr_arr_structs->size = ptr_text_buf->Num_lines;

    CreateArrayStructs (ptr_text_buf, ptr_arr_structs);

    return 0;
}

//=====================================================================================================================================

void CreateArrayStructs (type_buf_char* ptr_text_buf, type_buf_structs* ptr_arr_structs)
{
    size_t index_line = 0;

    char* ptr_prev_line = ptr_text_buf->ptr;

    for (size_t i = 0; i < ptr_text_buf->size; i++)
    {
        if (isEndOfFile (ptr_text_buf->ptr[i]) == 1)
        {
            if (isLineEmpty (ptr_prev_line) == 0)
            {
                ptr_arr_structs->ptr[index_line] = {ptr_prev_line, (ptr_text_buf->ptr) + i - ptr_prev_line};

                index_line++;
            }

            ptr_prev_line = (ptr_text_buf->ptr) + i + 1;
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

    char* ptr_prev_line = ptr_text_buf->ptr;

    for (size_t i = 0; i < ptr_text_buf->size; i++)
    {
        if (*(ptr_text_buf->ptr + i) == '\n')
        {
            if (!isLineEmpty(ptr_prev_line))
            {
                num_lines++;
            }

            ptr_prev_line = ptr_text_buf->ptr + i + 1;
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

bool isLetter (char symbol)
{
    if (('a' <= symbol && symbol <= 'z') || ('A' <= symbol && symbol <= 'Z'))
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
        printf ("Error in function: %s. Error opening file!\n", __func__);
        return nullptr;
    }

    return stream;
}

//=====================================================================================================================================

int PutBuffer (FILE* stream, int* asmPtr, size_t bufferSize)
{
    size_t countSymbols = fwrite (asmPtr, sizeof (int), bufferSize, stream);

    if (countSymbols != bufferSize)
    {
        return -1;
    }

    return 0;
}

//=====================================================================================================================================

bool isLineEmpty (char* ptr_line)
{
    while (isEndOfFile (*ptr_line) == 0)
    {
        if (isLetter (*ptr_line))
        {
            return false;
        }

        ptr_line++;
    }

    return true;
}

//=====================================================================================================================================

bool isEndOfFile (char symbol)
{
    return (symbol == '\0' || symbol == '\n' || symbol == '\r') ? true : false;
}

//=====================================================================================================================================
