/*	NumberHelper.cpp
 *	create by Phaeton
 *	2013.12.28
 */

#include "StdAfx.h"
#include "NumberHelper.h"


NumberHelper::NumberHelper(void)
{

}


NumberHelper::~NumberHelper(void)
{

}

int NumberHelper::BiAbs(int n)
{
	return n<0?-n:n;
}

int NumberHelper::bits_in_digit(uint16t d)
{
    int d_bits = 0;
    while (d >= 32) 
	{
        d_bits += 6;
        d >>= 6;
    }
    d_bits += (int)BitLengthTable[d];
    return d_bits;
}