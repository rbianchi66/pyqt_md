#ifndef SYSDEP_H
#define SYSDEP_H

#ifdef _MSC_VER
    #if _MSC_VER <= 1200
        #pragma warning (disable : 4786)
        #define for if (0); else for
    #endif
#endif

#endif
