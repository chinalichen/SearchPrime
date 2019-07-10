#include "StdAfx.h"
#include "SpecialInt.h"


SpecialInt::SpecialInt(void)
{
}


SpecialInt::~SpecialInt(void)
{
}

bool SpecialInt::isPrime()
{
	if(getDigit()[0]&1)
		return true;
	return false;
}