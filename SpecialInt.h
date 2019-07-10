#pragma once
#include "biginteger.h"

class SpecialInt :
	public BigInteger
{
public:
	SpecialInt(void);
	~SpecialInt(void);
	bool isPrime();
};

