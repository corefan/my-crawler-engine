/* MD5.H - header file for MD5C.C
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

#ifndef PROTOTYPES
#define PROTOTYPES 1
#endif

#pragma warning(disable:4996)

#include <string>

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
returns an empty list.
*/
#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif

/* MD5 context. */
typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

void MD5Init(MD5_CTX *);
void MD5Update(MD5_CTX *, unsigned char *, unsigned int);
void MD5Final(unsigned char [16], MD5_CTX *);

#include <windows.h>
#include <tchar.h>

namespace i8desk
{
	inline void MD5Encode(const void* data, size_t ByteLen, LPTSTR md5)
	{
		MD5_CTX		  context	  = {0};
		unsigned char digest[16]  = {0};

		MD5Init(&context);
		MD5Update(&context, (unsigned char*)data, ByteLen);
		MD5Final(digest,   &context);

		for (size_t idx = 0; idx<16; idx++)
		{   
			_stprintf(&(md5[2 * idx]), TEXT("%02x"), digest[idx]); 
		}
	}

	inline std::basic_string<TCHAR> MDString(const char* pString)
    {
        TCHAR szMd5[33] ={0};
        MD5Encode(pString, strlen(pString), szMd5);
        return std::basic_string<TCHAR>(szMd5);
    }
}