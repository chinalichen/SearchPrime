/*	BigInteger.h
 *	create by Phaeton
 *	2013.12.28
 */

#pragma once

typedef short          int16t;
typedef int            int32t;
typedef unsigned short uint16t;
typedef unsigned int   uint32t;
typedef int            bi_size_t;

using std::string;
using std::cerr;
using std::endl;

class BigInteger
{
public:
	BigInteger(void);
	BigInteger(int);
	BigInteger(unsigned int);
	BigInteger(BigInteger &bi);
	~BigInteger(void);

	int biAsInt(bool*);
	int biFromInt(int);
	int biFromUnsignedInt(unsigned int digit);
	unsigned int biAsUnsigendInt(bool *overflow);
	void printHex();
	bool isEven();
	bool isZero();
	uint16t* getDigit();
	bi_size_t getSize();
	BigInteger  operator+(BigInteger&);
	BigInteger& operator+=(int);
	BigInteger& operator++();
	BigInteger  operator-(BigInteger&);
	BigInteger  operator-(int b);
	BigInteger& operator--();
	BigInteger& operator-=(int b);
	BigInteger  operator*(BigInteger&);
	BigInteger  operator*=(int);
	BigInteger  operator*=(BigInteger&);
	BigInteger& operator=(const BigInteger &b);
	BigInteger& operator=(const int &b);
	BigInteger  operator/(BigInteger &b);
	BigInteger  operator%(BigInteger &b);
	BigInteger  operator<<(BigInteger &b);
	BigInteger  operator<<(int b);
	BigInteger& operator<<=(int b);
	BigInteger  operator>>(BigInteger &b);
	BigInteger  operator>>(int b);
	BigInteger& operator>>=(int b);
	bool operator==(int b);
	bool operator==(BigInteger &b);
	BigInteger& fromString(char*);
	string toString();
	static int obj_sum;
	enum
	{
		BIT_LENGTH_MAX          = 2147483647,
		BI_SIZE_T_MAX           = 2147483647/16,
		BIG_INTEGER_SHIFT       = 15,
		BIG_INTEGER_BASE        = (uint16t)1 << BIG_INTEGER_SHIFT,
		BIG_INTEGER_MASK        = (uint16t)(BIG_INTEGER_BASE - 1),
		BI_DECIMAL_BASE         = (uint16t)10000,
		BI_DECIMAL_SHIFT        = 4,
		KARATSUBA_CUTOFF        = 70,
		KARATSUBA_SQUARE_CUTOFF = (2 * KARATSUBA_CUTOFF)
	};
private:
	int biResize(bi_size_t);
	void normalize();
	bool greater(BigInteger &);
	bool smaller(BigInteger &);
	int biCompare(BigInteger *a, BigInteger *b);
	BigInteger* add(BigInteger &);
	BigInteger* sub(BigInteger &);
	BigInteger* mul(BigInteger&);
	static uint16t digitAdd(uint16t *a, bi_size_t as, uint16t *b, bi_size_t bs);
	static uint16t digitSub(uint16t *a, bi_size_t as, uint16t *b, bi_size_t bs);
	static BigInteger* biAbsAdd(BigInteger *a, BigInteger *b);
	static BigInteger* biAbsSub(BigInteger *a, BigInteger *b);
	static BigInteger* biAbsMul(BigInteger*, BigInteger*);
	static BigInteger* biKaratsubaMul(BigInteger*, BigInteger*);
	static BigInteger* biLopsidedMul(BigInteger*, BigInteger*);
	static int biKmulSplit(BigInteger*, bi_size_t, BigInteger **hi, BigInteger **lo);
	static string biAsDecimalString(BigInteger*, int);
	static BigInteger* biFromString(char* str, char** pend, int base);
	static BigInteger*  biFromBinaryBase(char **str, int base);
	static BigInteger* digitBigIntDivRem(BigInteger *a, uint16t n, uint16t *prem);
	static BigInteger* biRem(BigInteger *v1, BigInteger *w1, BigInteger **prem);
	static uint16t digitDivRem(uint16t *pout, uint16t *pin, bi_size_t size, uint16t n);
	static int biDivRem(BigInteger *a, BigInteger *b, BigInteger **pdiv, BigInteger **prem);
	static int biDivMod(BigInteger *v, BigInteger *w, BigInteger **pdiv, BigInteger **pmod);
	static uint16t digitLshift(uint16t *z, uint16t *a, bi_size_t m, int d);
	static uint16t digitRshift(uint16t *z, uint16t *a, bi_size_t m, int d);
	static BigInteger* biRshift(BigInteger *v, BigInteger *w);
	static BigInteger* biRshift(BigInteger *v, bi_size_t w);
	static BigInteger* biLshift(BigInteger *v, BigInteger *w);
	static BigInteger* biLshift(BigInteger *v, bi_size_t w);
	static BigInteger* biInvert(BigInteger *v);

private:
	uint16t *ob_digit;	//由于std::vector的性能不符合要求，我们需要自己构建一个vector.
	/*内存模型介绍：
	 *  uint16t为unsigned short类型，16位，两个字节.
	    但是我们只是用其中的低15位存储数据，即：
	 *  0x00017fff  ==  65535
	 *这样的好处：
	 *  在两个BigInteger相加的时候，最高位即为进位，不需要额外的变量来保存进位.
	 *  同样在减法，乘法，位移的过程中也有相似的优点.
	 *坏处：
	 *  有1/16的内存空间被浪费，但是这是牺牲空间来换取时间的典型做法.
	 */
	bi_size_t ob_digit_size;
	bool negative;
};

