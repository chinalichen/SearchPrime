/*	MemHelper.cpp
 *	create by Phaeton
 *	2013.12.28
 */

#include "StdAfx.h"
#include "MemHelper.h"
#include "BigInteger.h"

MemHelper::MemHelper(void)
{
}


MemHelper::~MemHelper(void)
{
}

/* BiMalloc(0) means malloc(1). Some systems would return NULL
   for malloc(0), which would be treated as an error. Some platforms
   would return a pointer with no memory behind it, which would break
   BiInteger. To solve these problems, allocate an extra byte. */
void* MemHelper::BiMalloc(bi_size_t n)
{
	if((size_t)n > (size_t)BigInteger::BI_SIZE_T_MAX)
		return NULL;
	return malloc(n?n:1);
}


void MemHelper::BiFree(void* p)
{
	free(p);
}

void* MemHelper::BiRealloc(void* memblock, bi_size_t n)
{
	if((size_t)n > (size_t)BigInteger::BI_SIZE_T_MAX)
		return NULL;
	return realloc(memblock, n?n:1);
}