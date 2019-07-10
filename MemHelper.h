/*	MemHelper.h
 *	create by Phaeton
 *	2013.12.28
 */

#pragma once

typedef int            bi_size_t;

class MemHelper
{
public:
	MemHelper(void);
	~MemHelper(void);
	static void* BiMalloc(bi_size_t);
	static void BiFree(void*);
	static void* BiRealloc(void*, bi_size_t);
};

