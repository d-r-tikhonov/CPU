//=====================================================================================================================================

#include "parser.h"

//=====================================================================================================================================

void TextCtor (Text* text, FILE* const source)
{
    ASSERT (source != nullptr, (void) -1);
    ASSERT (text   != nullptr, (void) -1);

    ReadInBuffer (text, source);

    MatchLines (text);
}

//=====================================================================================================================================

void ReadInBuffer (Text* text, FILE* source)
{
    ASSERT (text   != nullptr, (void) -1);
    ASSERT (source != nullptr, (void) -1);
    
    text->nChar = FileSize (source);

    text->buffer = (char*) calloc (text->nChar + 1, sizeof (char));
    
    fread (text->buffer, sizeof(char), text->nChar, source);
}

//=====================================================================================================================================

size_t FileSize (FILE* stream)
{
    ASSERT (stream != nullptr, 0);

	size_t curPos = ftell (stream);

	fseek (stream, 0L, SEEK_END);

	if (ftell (stream) == -1)
    {
        printf ("Error in function: %s. Error size = -1!\n", __func__);
        return errno;
    }

    size_t size = ftell (stream);

	fseek (stream, curPos, SEEK_SET);

	return size;
}

//=====================================================================================================================================

void MatchLines (Text* text)
{
    size_t   line        = 0;
    size_t   lineStart   = 0;
    
    CountInText (text->buffer, '\n', &(text->nLines));
    text->lines = (Lines*) calloc (text->nLines + 1, sizeof(Lines));
    
    for (size_t index = 1; index < text->nChar; index++)
    {
        if (*(text->buffer + index) == '\n')
        {
            (*(text->buffer + index)= '\0');

            (text->lines + line)->lineStart = text->buffer + lineStart;
            (text->lines + line)->length    = index - lineStart;
            (text->lines + line)->lineIndex = line;

            lineStart = index + 1;

            line = line + 1;
        }

        if (lineStart > text->nChar)
        {
            break;
        }
    }

}

//=====================================================================================================================================
     
void CountInText (char* text, char search, size_t* count)
{
    size_t index = 0;

    while (*(text + index))  
    {
        if (*(text + index) == search)
        {
            ++(*count);
        }
        ++index;
    }
}

//=====================================================================================================================================

void TextDtor(Text* text)
{
    ASSERT (text != nullptr, (void) -1);

    free (text->buffer); 
    free (text->lines);
}

//=====================================================================================================================================
