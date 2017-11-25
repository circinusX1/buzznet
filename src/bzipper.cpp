
#include "bzipper.h"


#define CHUNK   16384
//-----------------------------------------------------------------------------
int    Bzipper::Compress(KXCHAR* fileName, BYTE* stream, int length)
{
    int ret, flush;
    unsigned int have;
    z_stream strm;
    char out[CHUNK];

    // allocate deflate state
    strm.zalloc = Z_NULL;
    strm.zfree  = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, 1);
    if (ret != Z_OK)
        return ret;
    
    FILE* pf = fopen(_F("%s~",fileName),"wb");

    if(0==pf)
        return Z_ERRNO;

    printf("Save:%s.z\n",fileName);    
	
    DWORD zipsz     = 0;
    BYTE* pWalk     = stream;
    int   remaining = length;
    /* compress until end of file */
    do {
        strm.avail_in = remaining; // pWalk+=strm.avail_in
        if(strm.avail_in > CHUNK)
        {
            strm.avail_in = CHUNK;
            remaining    -= CHUNK;
        }
        else
        {
            remaining    = 0;
        }

        flush = remaining==0 ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = pWalk;
        pWalk += CHUNK;

        // run deflate() on input until output buffer not full, finish
        //   compression if all of source has been read in 
        do {
            strm.avail_out = CHUNK;
            strm.next_out = (Bytef *)out;
            ret = deflate(&strm, flush);    // no bad return value 
            assert(ret != Z_STREAM_ERROR);  // state not clobbered 
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, pf) != have || ferror(pf)) 
            {
                deflateEnd(&strm);
                return Z_ERRNO;
            }
            zipsz += have;
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     // all input will be used 

        // done when last data in file processed 
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        // stream will be complete 

    // clean up and return 
    deflateEnd(&strm);
    fclose(pf);

    float ratio = (float)length/(float)zipsz;
    //UnCompress(fileName);
    return Z_OK;
}

int Bzipper::UnCompress(KXCHAR* fileName, KXCHAR* destName)
{
    int   rv    = -1;
    FILE* pfz   = fopen(fileName,"rb");
    FILE* pfdec = fopen(destName,"wb");
    if(pfz && pfdec)
    {
        rv = _inflate(pfz, pfdec);
    }
    if(pfz)
        fclose(pfz);
    if(pfdec)
        fclose(pfdec);
    return rv;
}

int Bzipper::_inflate(FILE *source, FILE *dest)
{
    int ret;
    unsigned have;
    z_stream strm;
    char in[CHUNK];
    char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = (uInt)fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = (Bytef *)in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = (Bytef *)out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}
