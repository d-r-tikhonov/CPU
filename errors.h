#ifndef ERRORS_H
#define ERRORS_H

//=====================================================================================================================================

#include <stdio.h>

//=====================================================================================================================================

#ifndef NDEBUG
    #define ASSERT(condition, returnValue)                                                      \
        if (!(condition))                                                                       \
        {                                                                                       \
            printf ("Assertion FAILED! Error in %s. FILE: %s. LINE: %d. FUNCTION: %s\n",        \
                    #condition, __FILE__, __LINE__, __PRETTY_FUNCTION__);                       \
                                                                                                \
            return returnValue;                                                                 \
        }
#else
    #define ASSERT(condition) ;
#endif

//=====================================================================================================================================

#endif
