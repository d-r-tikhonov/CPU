#ifndef PROCESSOR_CONFIG_H_
#define PROCESSOR_CONFIG_H_

//=====================================================================================================================================

typedef int elem_t;

//=====================================================================================================================================

extern const char* VersionFile;
extern size_t      VersionRepeat;
extern const char* NameFileASM;

//=====================================================================================================================================

enum ARG_TYPES
{
    ARG_IMMED = (1 << 5),
    ARG_REG   = (1 << 6),
    ARG_RAM   = (1 << 7)
};

//=====================================================================================================================================

#endif
