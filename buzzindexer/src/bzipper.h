#ifndef __BZIP_H__
#define __BZIP_H__

#include "core.h"
#ifdef _WIN32
    #include "../../zlib/zlib-1.2.3/zlib.h"
#else
    #include "../z_lib/src/zlib.h"
#endif //


class Bzipper
{
public:
    Bzipper(void){};
    ~Bzipper(void){};

    int Compress(KXCHAR* fileName, BYTE* stream, int length);
    int UnCompress(KXCHAR* fileName, KXCHAR* destname);
private:
    int _inflate(FILE *source, FILE *dest);
};


#endif //__BZI_H__
