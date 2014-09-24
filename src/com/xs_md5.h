#ifdef __cplusplus
extern "C"{
#endif

#ifndef XS_MD5_H
#define XS_MD5_H



/* POINTER defines a generic pointer type */
typedef unsigned char * POINTER;

/* UINT2 defines a two byte word */
//typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;


/* MD5 context. */
typedef struct {
    UINT4 state[4];                                   /* state (ABCD) */
    UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

void MD5Init (MD5_CTX *context);
void MD5Update (MD5_CTX *context, const unsigned char *input, unsigned int inputLen);
void MD5UpdaterString(MD5_CTX *context,const char *string);
int MD5FileUpdateFile (MD5_CTX *context,char *filename);
void MD5Final (unsigned char digest[16], MD5_CTX *context);
void MDString (const char *string,unsigned char digest[16]);
int MD5File (char *filename,unsigned char digest[16]);

static inline char* xs_hex2str(const unsigned char* hex, int len, char* str)
{
    int i;
    for(i=0; i<len; i++)
    {
        sprintf(str+i*2, "%02x", (int)hex[i]);
    }
    return str;
}

static inline char* xs_mkmd5_for_string(const char* buf, char* output)
{
    unsigned char dig[16];
    MDString(buf, dig);
    xs_hex2str(dig, 16, output);
    return output;
}

#endif // XS_MD5_H

#ifdef __cplusplus
}
#endif
