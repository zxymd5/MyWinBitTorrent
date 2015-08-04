/*
 *  Based on shasum from http://www.netsw.org/crypto/hash/
 *  Majorly hacked up to use Dr Brian Gladman's sha1 code
 *
 *  Copyright (C) 2002 Dr Brian Gladman <brg@gladman.me.uk>, Worcester, UK.
 *  Copyright (C) 2003 Glenn L. McGrath
 *  Copyright (C) 2003 Erik Andersen
 *
 * Licensed under GPLv2 or later, see file LICENSE in busybox tarball for details.
 */

#ifndef SHA_H_
#define SHA_H_

#ifdef _MSC_VER
typedef signed char  int8_t;
typedef unsigned char uint8_t;
typedef __int32 int32_t; 
typedef unsigned __int32 uint32_t; 
typedef __int64 int64_t; 
typedef unsigned __int64 uint64_t;  
#else 
#include <stdint.h> 
#endif 

typedef struct sha1_ctx_t {
	uint32_t count[2];
	uint32_t hash[5];
	uint32_t wbuf[16];
} sha1_ctx_t;
void sha1_begin(sha1_ctx_t *ctx);
void sha1_hash(const void *data, size_t length, sha1_ctx_t *ctx);
void *sha1_end(void *resbuf, sha1_ctx_t *ctx);
void sha1_block(const void* data, size_t length, unsigned char output[20]); 

#endif

