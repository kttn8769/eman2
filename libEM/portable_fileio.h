#ifndef __portable_fileio_h__
#define __portable_fileio_h__

#include <stdio.h>
#include <sys/types.h>


inline int portable_fseek(FILE *fp, off_t offset, int whence)
{
#if defined(HAVE_FSEEKO) && SIZEOF_OFF_T >= 8
    return fseeko(fp, offset, whence);
#elif defined(HAVE_FSEEK64)
    return fseek64(fp, offset, whence);
#elif defined(__BEOS__)
    return _fseek(fp, offset, whence);
#else
    return fseek(fp, offset, whence);
#endif
}

inline off_t portable_ftell(FILE* fp)
{
#if defined(HAVE_FTELLO) && SIZEOF_OFF_T >= 8
    return ftello(fp);
#elif defined(HAVE_FTELL64)
    return ftell64(fp);
#else
    return ftell(fp);
#endif
}



#endif
