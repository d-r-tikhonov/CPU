#ifndef PARSE_H
#define PARSE_H

//=====================================================================================================================================

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
//=====================================================================================================================================

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "errors.h"

//=====================================================================================================================================

struct Lines
{
    char* lineStart = nullptr;

    size_t length   = 0;
    size_t lineIndex = 0;
};

struct Text
{
    char* buffer = nullptr;

    Lines* lines = nullptr;

    size_t nChar  = 0;
    size_t nLines = 0;
};

//=====================================================================================================================================

void        TextCtor        (Text * text, FILE* const source);
size_t      FileSize        (FILE* const stream);
void        CountInText     (char* buf, char search, size_t* count);
void        ReadInBuffer    (Text* text, FILE* source);
void        MatchLines      (Text* text);
void        TextDtor        (Text* text); 

//=====================================================================================================================================

#endif