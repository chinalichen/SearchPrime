/*	BigInteger.cpp
 *	create by Phaeton
 *	2013.12.28
 */

#include "StdAfx.h"
#include "BigInteger.h"
#include "MemHelper.h"
#include "NumberHelper.h"

BigInteger::BigInteger(void)
{
	ob_digit = NULL;
	ob_digit_size = 0;
	negative = false;
	obj_sum++;
}

BigInteger::BigInteger(BigInteger &bi)
{
	negative = bi.negative;
	ob_digit_size = bi.ob_digit_size;
	ob_digit = (uint16t*)MemHelper::BiMalloc(ob_digit_size*sizeof(uint16t));
	memcpy(ob_digit, bi.ob_digit, ob_digit_size * sizeof(uint16t));
	obj_sum++;
}

BigInteger::BigInteger(int value)
{
	biFromInt(value);
	obj_sum++;
}

BigInteger::BigInteger(unsigned int value)
{
	biFromUnsignedInt(value);
	obj_sum++;
}

BigInteger::~BigInteger(void)
{
	MemHelper::BiFree(ob_digit);
	obj_sum--;
}

uint16t* BigInteger::getDigit()
{
	return ob_digit;
}

bi_size_t BigInteger::getSize()
{
	return ob_digit_size;
}

/* Normalize (remove leading zeros from) a BigInteger object.
   Doesn't attempt to free the storage--in most cases, due to the nature
   of the algorithms used, this could save at most be one word anyway. */
void BigInteger::normalize()
{
	bi_size_t i;
	for(i=ob_digit_size; i>=0; i--)
	{
		if(ob_digit[i-1]!=0)
			break;
	}
	ob_digit_size = i;
}

/*从int转换为BigInteger类型，因为unsigned int的
范围比int大，故使用了unsigned int转换的方法*/
int BigInteger::biFromInt(int digit)
{
	int ret = biFromUnsignedInt(digit<0?(-digit):digit);
	if(digit<0)
		negative = true;
	return ret;
}

/*从unsigned int转换为BigInteger类型
 内存模型在BigInteger.h中介绍*/
int BigInteger::biFromUnsignedInt(unsigned int digit)
{
	uint16t *p;
	uint32t t = digit;
	bi_size_t size=0;
	negative = false;
	while(t)
	{
		size++;
		t>>=BIG_INTEGER_SHIFT;
	}
	ob_digit = (uint16t*)MemHelper::BiMalloc(sizeof(uint16t)*size);
	ob_digit_size = size;
	if(ob_digit == NULL)
		return -1;
	t = digit;
	p = ob_digit;
	while(t)
	{
		*p++ = (uint16t)(t&BIG_INTEGER_MASK);
		t>>=BIG_INTEGER_SHIFT;
	}
	return 0;
}

void BigInteger::printHex()
{
	std::cout<<"0x";
	for(int i=0;i!=ob_digit_size;i++)
		std::cout<<std::hex<<std::setw(4)<<std::setfill('0')<<ob_digit[i];
}

int BigInteger::biAsInt(bool *overflow)
{
	int ret = 0, prev;
	bi_size_t size = ob_digit_size;

	while(--size >= 0)
	{
		prev = ret;
		ret = (ret<<BIG_INTEGER_SHIFT) | ob_digit[size];
		if((ret>>BIG_INTEGER_SHIFT) != prev)
		{
			cerr<<"convert to unsigned int, overflow."<<endl;
			*overflow = true;
			return -1;
		}
	}
	*overflow = false;
	return negative?-ret:ret;
}

unsigned int BigInteger::biAsUnsigendInt(bool *overflow)
{
	uint32t ret = 0, prev;
	bi_size_t size = ob_digit_size;

	if(negative)
	{
		cerr<<"can't convert negative value to unsigned int"<<endl;
		*overflow = true;
		return -1;
	}
	while(--size >= 0)
	{
		prev = ret;
		ret = (ret<<BIG_INTEGER_SHIFT) | ob_digit[size];
		if((ret>>BIG_INTEGER_SHIFT) != prev)
		{
			cerr<<"convert to unsigned int, overflow."<<endl;
			*overflow = true;
			return -1;
		}
	}
	*overflow = false;
	return ret;
}

bool BigInteger::greater(BigInteger &b)
{
	normalize();
	b.normalize();
	if(ob_digit_size > b.ob_digit_size)
		return true;
	if(ob_digit_size < b.ob_digit_size)
		return false;
	return ob_digit[ob_digit_size-1] > b.ob_digit[b.ob_digit_size-1];
}

bool BigInteger::smaller(BigInteger &b)
{
	normalize();
	b.normalize();
	if(ob_digit_size < b.ob_digit_size)
		return true;
	if(ob_digit_size > b.ob_digit_size)
		return false;
	return ob_digit[ob_digit_size-1] < b.ob_digit[b.ob_digit_size-1];
}

// 将a的绝对值与b的绝对值相加（a+b）,结果存放到a中，返回值为进位，都为非负数
uint16t BigInteger::digitAdd(uint16t *a, bi_size_t as, uint16t *b, bi_size_t bs)
{
	uint16t carry=0;
	bi_size_t i;
	assert(as>=bs);
	for(i=0;i<bs;i++)
	{
		carry += a[i] + b[i];
		a[i] = carry & BIG_INTEGER_MASK;
		carry >>= BIG_INTEGER_SHIFT;
	}
	for(;carry && i<as;i++)
	{
		carry += a[i];
		a[i] = carry & BIG_INTEGER_MASK;
		carry >>= BIG_INTEGER_SHIFT;
	}
	return carry;
}

// 将a的绝对值与b的绝对值相减（a-b）,结果存放到a中，返回值为进位，都为非负数
uint16t BigInteger::digitSub(uint16t *a, bi_size_t as, uint16t *b, bi_size_t bs)
{
	uint16t borrow=0;
	bi_size_t i;

	for(i=0;i<bs;i++)
	{
		borrow = a[i] - b[i] - borrow;
		a[i] = borrow & BIG_INTEGER_MASK;
		borrow >>= BIG_INTEGER_SHIFT;
		borrow &= 1;
	}
	for(;borrow && i<as;i++)
	{
		borrow = a[i] - borrow;
		a[i] = borrow & BIG_INTEGER_MASK;
		borrow >>= BIG_INTEGER_SHIFT;
		borrow &= 1;
	}
	return borrow;
}

BigInteger* BigInteger::biAbsAdd(BigInteger *a, BigInteger *b)
{
	bi_size_t size_a = a->ob_digit_size, size_b = b->ob_digit_size;
    BigInteger *z;
    bi_size_t i;
    uint16t carry = 0;

    // 保证a是两者之中的大数
    if (size_a < size_b) 
	{
        BigInteger *temp = a;
		a = b;
		b = temp;
        bi_size_t size_temp = size_a;
        size_a = size_b;
        size_b = size_temp; 
    }
    z = new BigInteger();
    if (z == NULL)
        return NULL;
	z->biResize(size_a+1);
	for (i = 0; i < size_b; ++i) 
	{
        carry += a->ob_digit[i] + b->ob_digit[i];
        z->ob_digit[i] = carry & BIG_INTEGER_MASK;
        carry >>= BIG_INTEGER_SHIFT;
    }
    for (; i < size_a; ++i) {
        carry += a->ob_digit[i];
        z->ob_digit[i] = carry & BIG_INTEGER_MASK;
        carry >>= BIG_INTEGER_SHIFT;
    }
    z->ob_digit[i] = carry;
	z->normalize();
    return z;
}

BigInteger* BigInteger::biAbsSub(BigInteger *a, BigInteger *b)
{
    bi_size_t size_a = a->ob_digit_size, size_b = b->ob_digit_size;
    BigInteger *z;
    bi_size_t i;
    int sign = 1;
    uint16t borrow = 0;

    // 保证a是两者之中的大数
    if (size_a < size_b) 
	{
        sign = -1;
        BigInteger *temp = a; 
		a = b; 
		b = temp;
        bi_size_t size_temp = size_a;
        size_a = size_b;
        size_b = size_temp;
    }
    else if (size_a == size_b) 
	{
        // Find highest digit where a and b differ:
        i = size_a;
        while (--i >= 0 && a->ob_digit[i] == b->ob_digit[i])
            ;
        if (i < 0)
            return new BigInteger();
        if (a->ob_digit[i] < b->ob_digit[i]) 
		{
            sign = -1;
            BigInteger *temp = a; 
			a = b; 
			b = temp;
        }
        size_a = size_b = i+1;
    }
	z = new BigInteger();
    if (z == NULL)
        return NULL;
	z->biResize(size_a);
    for (i = 0; i < size_b; ++i) 
	{
        // The following assumes unsigned arithmetic
        // works module 2**N for some N>BIG_INTEGER_SHIFT.
        borrow = a->ob_digit[i] - b->ob_digit[i] - borrow;
        z->ob_digit[i] = borrow & BIG_INTEGER_MASK;
        borrow >>= BIG_INTEGER_SHIFT;
        borrow &= 1; // Keep only one sign bit
    }
    for (; i < size_a; ++i) 
	{
        borrow = a->ob_digit[i] - borrow;
        z->ob_digit[i] = borrow & BIG_INTEGER_MASK;
        borrow >>= BIG_INTEGER_SHIFT;
        borrow &= 1; // Keep only one sign bit
    }
    assert(borrow == 0);
    if (sign < 0)
		z->negative = true;
	z->normalize();
    return z;
}

BigInteger* BigInteger::add(BigInteger &b)
{
	BigInteger* z;
    if (negative)
	{
		if (b.negative)
		{
            z = biAbsAdd(this, &b);
            if (z != NULL && z->ob_digit_size != 0)
				z->negative = z->negative?false:true;
        }
        else
            z = biAbsSub(&b, this);
    }
    else 
	{
        if (b.negative)
            z = biAbsSub(this, &b);
        else
            z = biAbsAdd(this, &b);
    }

    return z;
}

BigInteger* BigInteger::sub(BigInteger &b)
{
	BigInteger *z;

    if (negative) 
	{
        if (b.negative)
            z = biAbsSub(this, &b);
        else
            z = biAbsAdd(this, &b);
        if (z != NULL && z->ob_digit_size != 0)
			z->negative = z->negative?false:true;
    }
    else {
        if (b.negative)
            z = biAbsAdd(this, &b);
        else
            z = biAbsSub(this, &b);
    }

    return z;
}

bi_size_t BigInteger::biResize(bi_size_t size)
{
	if(size < ob_digit_size)
		return ob_digit_size;
	if(ob_digit == NULL)
	{
		ob_digit = (uint16t*)MemHelper::BiMalloc(size * sizeof(uint16t));
	}
	else
	{
		uint16t *ob_digit_re = (uint16t*)MemHelper::BiRealloc(ob_digit, size*sizeof(uint16t));
		if(ob_digit_re != NULL)
		{
			ob_digit = ob_digit_re;
			memset(ob_digit, 0, (size - ob_digit_size)*sizeof(uint16t));
		}
		else
		{
			cerr<<"BiRealloc Error."<<endl;
		}
	}
	return ob_digit_size = size;
}

BigInteger* BigInteger::biAbsMul(BigInteger *a, BigInteger *b)
{
	BigInteger *z;
    bi_size_t size_a = a->ob_digit_size;
	bi_size_t size_b = b->ob_digit_size;
    bi_size_t i;

	z = new BigInteger();
	z->biResize(size_a + size_b);
	memset(z->ob_digit, 0, z->ob_digit_size * sizeof(uint16t));
    if (a == b) 
	{
        // Efficient squaring per HAC, Algorithm 14.16
        // Gives slightly less than a 2x speedup when a == b,
        // via exploiting that each entry in the multiplication
        // pyramid appears twice (except for the size_a squares).
        for (i = 0; i < size_a; ++i) 
		{
            uint32t carry;
            uint32t f = a->ob_digit[i];
            uint16t *pz = z->ob_digit + (i << 1);
            uint16t *pa = a->ob_digit + i + 1;
            uint16t *paend = a->ob_digit + size_a;
            carry = *pz + f * f;
            *pz++ = (uint16t)(carry & BIG_INTEGER_MASK);
            carry >>= BIG_INTEGER_SHIFT;
            assert(carry <= BIG_INTEGER_MASK);

            // Now f is added in twice in each column of the
            // pyramid it appears.  Same as adding f<<1 once.
            f <<= 1;
            while (pa < paend) 
			{
                carry += *pz + *pa++ * f;
                *pz++ = (uint16t)(carry & BIG_INTEGER_MASK);
                carry >>= BIG_INTEGER_SHIFT;
                assert(carry <= (BIG_INTEGER_MASK << 1));
            }
            if (carry) {
                carry += *pz;
                *pz++ = (uint16t)(carry & BIG_INTEGER_MASK);
                carry >>= BIG_INTEGER_SHIFT;
            }
            if (carry)
                *pz += (uint16t)(carry & BIG_INTEGER_MASK);
            assert((carry >> BIG_INTEGER_SHIFT) == 0);
        }
    }
    else 
	{      // a is not the same as b -- gradeschool long mult
        for (i = 0; i < size_a; ++i) 
		{
            uint32t carry = 0;
            uint32t f = a->ob_digit[i];
            uint16t *pz = z->ob_digit + i;
            uint16t *pb = b->ob_digit;
            uint16t *pbend = b->ob_digit + size_b;
            while (pb < pbend) 
			{
                carry += *pz + *pb++ * f;
                *pz++ = (uint16t)(carry & BIG_INTEGER_MASK);
                carry >>= BIG_INTEGER_SHIFT;
                assert(carry <= BIG_INTEGER_MASK);
            }
            if (carry)
                *pz += (uint16t)(carry & BIG_INTEGER_MASK);
            assert((carry >> BIG_INTEGER_SHIFT) == 0);
        }
    }
	z->normalize();
    return z;
}

/*
Karatsuba算法主要应用于两个大数的相乘，原理是将大数分成两段后变成较小的数位，然后做3次乘法，并附带少量的加法操作和移位操作。
现有两个大数，x，y。首先将x，y分别拆开成为两部分，可得x1，x0，y1，y0。他们的关系如下：
x = x1 * 10m + x0；
y = y1 * 10m + y0。
其中m为正整数，m < n，且x0，y0 小于 10m。
那么:
xy = (x1 * 10m + x0)(y1 * 10m + y0)
   = z2 * 102m + z1 * 10m + z0
其中：
z2 = x1 * y1；
z1 = x1 * y0 + x0 * y1；
z0 = x0 * y0。
此步骤共需4次乘法，但是由Karatsuba改进以后仅需要3次乘法。因为：
z1 = x1 * y0+ x0 * y1
z1 = (x1 + x0) * (y1 + y0) - x1 * y1 - x0 * y0，
故x0 * y0 便可以由加减法得到。
*/
BigInteger* BigInteger::biKaratsubaMul(BigInteger *a, BigInteger *b)
{
	bi_size_t asize = a->ob_digit_size;
    bi_size_t bsize = b->ob_digit_size;
    BigInteger *ah = NULL;
    BigInteger *al = NULL;
    BigInteger *bh = NULL;
    BigInteger *bl = NULL;
    BigInteger *ret = NULL;
    BigInteger *t1, *t2, *t3;
    bi_size_t shift;           // 拆分的位数 
    bi_size_t i;

    // (ah*X+al)(bh*X+bl) = ah*bh*X*X + (ah*bl + al*bh)*X + al*bl
    // Let k = (ah+al)*(bh+bl) = ah*bl + al*bh  + ah*bh + al*bl
    // 原表达式可以表示为：
    //     ah*bh*X*X + (k - ah*bh - al*bl)*X + al*bl
    // X是 2 power , 当 "*X" 时，可以直接移位.

    // 因为需要按照较大的数进行拆分，所以保持b始终的较大的数
    if (asize > bsize) 
	{
        t1 = a;
        a = b;
        b = t1;

        i = asize;
        asize = bsize;
        bsize = i;
    }

    // 当数比较小的时候，使用列式乘法 112×12的计算方法：
	//      112
	//	×    12
	//----------
	//      224
	// +   112
	//----------
	//     1344

    i = a == b ? KARATSUBA_SQUARE_CUTOFF : KARATSUBA_CUTOFF;
    if (asize <= i) 
	{
        if (asize == 0)
            return new BigInteger(0);
        else
            return biAbsMul(a, b);
    }

    // 当 a 的值远小于 b 时， 将b拆分后会使ah == 0， 这时， Karatsuba乘法的性能不一定比「简单乘法」高.
	//但是我们可以把 b 看成一串长度为 a->ob_digit_size 的大数.这样，算法复杂度还是比「简单乘法」低很多.
    if (2 * asize <= bsize)
        return biLopsidedMul(a, b);

    // 将a和b拆分成 高位：hi 和 低位：lo.
    shift = bsize >> 1;
    if (biKmulSplit(a, shift, &ah, &al) < 0) 
		goto fail;
	assert(ah->ob_digit_size > 0);            // the split isn't degenerate

    if (a == b) 
	{
        bh = ah;
        bl = al;
    }
    else if (biKmulSplit(b, shift, &bh, &bl) < 0) 
		goto fail;

	//步骤：
	//1. 开辟存放结果的内存空间，有asize + bsize个uint16t大小（可以保证任何时候都能存储a*b的结果）.
	//2. 计算 ah*bh, 并将结果移动 2*shift.
	//3. 计算 al*bl, 结果不移动 0.  此处注意不能与步骤2的结果重叠.
	//4. 从结果中减去 al*bl , 这个步骤有可能会产生借位，不过可以直接忽略：
	//   我们的操作都是无符号的 mod BIG_INTEGER_BASE**(sizea + sizeb),
	//   并且只要 *final* 结果是正确的的,高位产生的进位和借位都可以被忽略掉.
	//5. 减去 ah*bh 
	//6. 计算 (ah+al)*(bh+bl), 并且将其移位后添加到结果中

    // 1. 给结果开辟空间.
	ret = new BigInteger();
	ret->biResize(asize + bsize);
    if (ret == NULL) 
		goto fail;

    // 2. t1 = ah*bh, 并将其拷贝到结果的高位中. 
    if ((t1 = biKaratsubaMul(ah, bh)) == NULL) 
		goto fail;
	assert(t1->ob_digit_size >= 0);
	assert(2*shift + t1->ob_digit_size <= ret->ob_digit_size);
	memcpy(ret->ob_digit + 2*shift, t1->ob_digit,t1->ob_digit_size * sizeof(uint16t));

    // 高于 ah*bh 的部分置 0.
	i = ret->ob_digit_size - 2*shift - t1->ob_digit_size;
    if (i)
		memset(ret->ob_digit + 2*shift + t1->ob_digit_size, 0, i * sizeof(uint16t));

    // 3. t2 = al*bl, 并将其拷贝到结果的低位中.
    if ((t2 = biKaratsubaMul(al, bl)) == NULL) 
	{
		delete(t1);
        //Py_DECREF(t1);
        goto fail;
    }
	assert(t2->ob_digit_size >= 0);
    assert(t2->ob_digit_size <= 2*shift); // 保证高位空间和低位空间没有重叠
    memcpy(ret->ob_digit, t2->ob_digit, t2->ob_digit_size * sizeof(uint16t));

    // 将剩余空间 置0
    i = 2*shift - t2->ob_digit_size;          // 没有初始化的空间
    if (i)
        memset(ret->ob_digit + t2->ob_digit_size, 0, i * sizeof(uint16t));

    // 4 & 5. 减去 ah*bh (t1) 和 al*bl (t2). 
	// 首先减去 al*bl,因为这两个数应该还在cache中.
	i = ret->ob_digit_size - shift;  // # digits after shift
    (void)digitSub(ret->ob_digit + shift, i, t2->ob_digit, t2->ob_digit_size);
    delete t2;

    (void)digitSub(ret->ob_digit + shift, i, t1->ob_digit, t1->ob_digit_size);
    delete t1;

    // 6. t3 = (ah+al)(bh+bl), 添加到结果中.
    if ((t1 = biAbsAdd(ah, al)) == NULL) goto fail;
    delete ah;
	delete al;
    ah = al = NULL;

    if (a == b) 
	{
        t2 = t1;
    }
    else if ((t2 = biAbsAdd(bh, bl)) == NULL) 
	{
        delete t1;
        goto fail;
    }
	delete bh;
	delete bl;
    bh = bl = NULL;

    t3 = biKaratsubaMul(t1, t2);
	delete t1;
	delete t2;
    if (t3 == NULL) goto fail;
	assert(t3->ob_digit_size >= 0);

    // 加 t3. 
	(void)digitAdd(ret->ob_digit + shift, i, t3->ob_digit, t3->ob_digit_size);
	delete t3;

	ret->normalize();
    return ret;

  fail:
    delete ret;
    delete ah;
    delete al;
    delete bh;
    delete bl;
    return NULL;
}

BigInteger* BigInteger::biLopsidedMul(BigInteger *a, BigInteger *b)
{
	const bi_size_t asize = a->ob_digit_size;
    bi_size_t bsize = b->ob_digit_size;
    bi_size_t nbdone;          // # of b digits already multiplied
    BigInteger *ret;
    BigInteger *bslice = NULL;

    assert(asize > KARATSUBA_CUTOFF);
    assert(2 * asize <= bsize);

    // Allocate result space, and zero it out.
	ret = new BigInteger();
	ret->biResize(asize + bsize);
    if (ret == NULL)
        return NULL;
	memset(ret->ob_digit, 0, ret->ob_digit_size * sizeof(uint16t));

    // Successive slices of b are copied into bslice.
    bslice = new BigInteger();
	bslice->biResize(asize);
    if (bslice == NULL)
	{
		//error occured
		delete ret;
		return NULL;
	}

    nbdone = 0;
    while (bsize > 0) 
	{
        BigInteger *product;
        const bi_size_t nbtouse = MIN(bsize, asize);

        // Multiply the next slice of b by a.
        memcpy(bslice->ob_digit, b->ob_digit + nbdone,
               nbtouse * sizeof(uint16t));
		bslice->ob_digit_size = nbtouse;
        product = biKaratsubaMul(a, bslice);
        if (product == NULL)
		{
			delete ret;
			delete bslice;
			return NULL;
		}

        // Add into result.
		(void)digitAdd(ret->ob_digit + nbdone, ret->ob_digit_size - nbdone,
			product->ob_digit, product->ob_digit_size);
        delete product;

        bsize -= nbtouse;
        nbdone += nbtouse;
    }

    delete bslice;
	ret->normalize();
    return ret;
}

int BigInteger::biKmulSplit(BigInteger *n, bi_size_t size, BigInteger **high, BigInteger **low)
{
    BigInteger *hi, *lo;
    bi_size_t size_lo, size_hi;
	const bi_size_t size_n = n->ob_digit_size;

    size_lo = MIN(size_n, size);
    size_hi = size_n - size_lo;

	hi = new BigInteger();
	if (hi == NULL)
        return -1;
	hi->biResize(size_hi);
    
	lo = new BigInteger();
	if (lo == NULL) 
	{
        delete hi;
        return -1;
    }
	lo->biResize(size_lo);

    memcpy(lo->ob_digit, n->ob_digit, size_lo * sizeof(uint16t));
    memcpy(hi->ob_digit, n->ob_digit + size_lo, size_hi * sizeof(uint16t));

	hi->normalize();
    *high = hi;
	lo->normalize();
    *low = lo;
    return 0;
}

BigInteger* BigInteger::mul(BigInteger &w)
{
    BigInteger *z;

    z = biKaratsubaMul(this, &w);
    // Negate if exactly one of the inputs is negative.
    if ((negative ^ w.negative) && z)
		z->negative = true;
    return z;
}

BigInteger BigInteger::operator+(BigInteger &b)
{
	BigInteger *result = add(b);
	BigInteger z = *result;
	delete result;
	return z;
}

BigInteger& BigInteger::operator++()
{
	BigInteger *result = add(BigInteger(1));
	*this = *result;
	delete result;
	return *this;
}

BigInteger& BigInteger::operator+=(int b)
{
	BigInteger *result = add(BigInteger(b));
	*this = *result;
	delete result;
	return *this;
}

BigInteger BigInteger::operator-(BigInteger &b)
{
	BigInteger *result = sub(b);
	BigInteger z = *result;
	delete result;
	return z;
}

BigInteger BigInteger::operator-(int b)
{
	BigInteger bb(b);
	BigInteger *result = sub(bb);
	BigInteger z = *result;
	delete result;
	return z;
}

BigInteger& BigInteger::operator--()
{
	BigInteger *result = sub(BigInteger(1));
	*this = *result;
	delete result;
	return *this;
}

BigInteger& BigInteger::operator-=(int b)
{
	BigInteger *result = sub(BigInteger(b));
	*this = *result;
	delete result;
	return *this;
}

BigInteger BigInteger::operator*(BigInteger &b)
{
	BigInteger *result = mul(b);
	BigInteger z = *result;
	delete result;
	return z;
}

BigInteger BigInteger::operator*=(int b)
{
	BigInteger bb(b);
	BigInteger *p = mul(bb);
	*this = *p;
	delete p;
	return *this;
}

BigInteger& BigInteger::operator=(const BigInteger &b)
{
	if(this == &b)
		return *this;
	negative = b.negative;
	ob_digit_size = b.ob_digit_size;
	biResize(ob_digit_size);
	memcpy(ob_digit,b.ob_digit, ob_digit_size*sizeof(uint16t));
	return *this;
}

BigInteger& BigInteger::operator=(const int &b)
{
	BigInteger bb(b);
	*this = bb;
	return *this;
}

BigInteger* BigInteger::biFromBinaryBase(char **str, int base)
{
    char *p = *str;
    char *start = p;
    int bits_per_char;
    bi_size_t n;
    BigInteger *z;
    uint32t accum;
    int bits_in_accum;
    uint16t *pdigit;

    assert(base >= 2 && base <= 32 && (base & (base - 1)) == 0);
    n = base;
    for (bits_per_char = -1; n; ++bits_per_char)
        n >>= 1;
    // n <- total # of bits needed, while setting p to end-of-string.
    while (BiDigitValue[*p & 0xff] < base)
        ++p;
    *str = p;
    // n <- # of Python digits needed, = ceiling(n/BIG_INTEGER_SHIFT).
    n = (p - start) * bits_per_char + BIG_INTEGER_SHIFT - 1;
    if (n / bits_per_char < p - start) 
	{
        cerr<<"long string too large to convert";
        return NULL;
    }
    n = n / BIG_INTEGER_SHIFT;
	z = new BigInteger();
	z->biResize(n);
    if (z == NULL)
        return NULL;

    // Read string from right, and fill in long from left; i.e.,
    // from least to most significant in both.
    accum = 0;
    bits_in_accum = 0;
    pdigit = z->ob_digit;
    while (--p >= start) 
	{
        int k = BiDigitValue[*p & 0xff];
        assert(k >= 0 && k < base);
        accum |= (uint32t)k << bits_in_accum;
        bits_in_accum += bits_per_char;
        if (bits_in_accum >= BIG_INTEGER_SHIFT) {
            *pdigit++ = (uint16t)(accum & BIG_INTEGER_MASK);
            assert(pdigit - z->ob_digit <= n);
            accum >>= BIG_INTEGER_SHIFT;
            bits_in_accum -= BIG_INTEGER_SHIFT;
            assert(bits_in_accum < BIG_INTEGER_SHIFT);
        }
    }
    if (bits_in_accum) 
	{
        assert(bits_in_accum <= BIG_INTEGER_SHIFT);
        *pdigit++ = (uint16t)accum;
        assert(pdigit - z->ob_digit <= n);
    }
    while (pdigit - z->ob_digit < n)
        *pdigit++ = 0;
	z->normalize();
    return z;
}

BigInteger* BigInteger::biFromString(char *str, char **pend, int base)
{
	int sign = 1;
    char *start, *orig_str = str;
    BigInteger *z;
    string *strobj, *strrepr;
    bi_size_t slen;

    if ((base != 0 && base < 2) || base > 36) 
	{
        cerr<<"biFromString() arg 'base' must be >= 2 and <= 36";
        return NULL;
    }
    while (*str != '\0' && isspace(*str & 0xff))
        str++;
    if (*str == '+')
        ++str;
    else if (*str == '-') 
	{
        ++str;
        sign = -1;
    }
    while (*str != '\0' && isspace(*str & 0xff))
        str++;
    if (base == 0) 
	{
        // No base given.  Deduce the base from the contents
        //   of the string
        if (str[0] != '0')
            base = 10;
        else if (str[1] == 'x' || str[1] == 'X')
            base = 16;
        else if (str[1] == 'o' || str[1] == 'O')
            base = 8;
        else if (str[1] == 'b' || str[1] == 'B')
            base = 2;
        else
            base = 8;
    }
    // Whether or not we were deducing the base, skip leading chars
    //   as needed 
    if (str[0] == '0' &&
        ((base == 16 && (str[1] == 'x' || str[1] == 'X')) ||
         (base == 8  && (str[1] == 'o' || str[1] == 'O')) ||
         (base == 2  && (str[1] == 'b' || str[1] == 'B'))))
        str += 2;

    start = str;
    if ((base & (base - 1)) == 0)
        z = biFromBinaryBase(&str, base);
    else 
	{
        register uint32t c;           // current input character
        bi_size_t size_z;
        int i;
        int convwidth;
        uint32t convmultmax, convmult;
        uint16t *pz, *pzstop;
        char* scan;

        static double log_base_PyLong_BASE[37] = {0.0e0,};
        static int convwidth_base[37] = {0,};
        static uint32t convmultmax_base[37] = {0,};

        if (log_base_PyLong_BASE[base] == 0.0) 
		{
            uint32t convmax = base;
            int i = 1;

            log_base_PyLong_BASE[base] = (log((double)base) / log((double)BIG_INTEGER_BASE));
            for (;;) 
			{
                uint32t next = convmax * base;
                if (next > BIG_INTEGER_BASE)
                    break;
                convmax = next;
                ++i;
            }
            convmultmax_base[base] = convmax;
            assert(i > 0);
            convwidth_base[base] = i;
        }

        // Find length of the string of numeric characters.
        scan = str;
        while (BiDigitValue[*scan & 0xff] < base)
            ++scan;

        // Create a long object that can contain the largest possible
        // integer with this base and length.  Note that there's no
        // need to initialize z->ob_digit -- no slot is read up before
        // being stored into.
        size_z = (bi_size_t)((scan - str) * log_base_PyLong_BASE[base]) + 1;
        // Uncomment next line to test exceedingly rare copy code
        // size_z = 1;
        assert(size_z > 0);
		z = new BigInteger();
		z->biResize(size_z);
        if (z == NULL)
            return NULL;
		z->ob_digit_size = 0;

        // `convwidth` consecutive input digits are treated as a single
        // digit in base `convmultmax`.
        convwidth = convwidth_base[base];
        convmultmax = convmultmax_base[base];

        // Work ;-) 
        while (str < scan) 
		{
            // grab up to convwidth digits from the input string 
            c = (uint16t)BiDigitValue[*str & 0xff]; ///////////////////////////////////////////maybe not correct
			str++;
            for (i = 1; i < convwidth && str != scan; ++i, ++str) 
			{
                c = (uint32t)(c *  base + BiDigitValue[*str & 0xff]);
                assert(c < BIG_INTEGER_BASE);
            }

            convmult = convmultmax;
            // Calculate the shift only if we couldn't get
            // convwidth digits.
            if (i != convwidth) 
			{
                convmult = base;
                for ( ; i > 1; --i)
                    convmult *= base;
            }

            // Multiply z by convmult, and add c.
            pz = z->ob_digit;
			pzstop = pz + z->ob_digit_size;
            for (; pz < pzstop; ++pz) 
			{
                c += (uint32t)*pz * convmult;
                *pz = (uint16t)(c & BIG_INTEGER_MASK);
                c >>= BIG_INTEGER_SHIFT;
            }
            // carry off the current end
            if (c) 
			{
                assert(c < BIG_INTEGER_BASE);
				if (z->ob_digit_size < size_z) 
				{
                    *pz = (uint16t)c;
					++(z->ob_digit_size);
                }
                else
				{
                    BigInteger *tmp;
                    // Extremely rare.  Get more space.
					assert(z->ob_digit_size == size_z);
					tmp = new BigInteger();
					tmp->biResize(size_z + 1);
                    if (tmp == NULL) 
					{
                        delete z;
                        return NULL;
                    }
                    memcpy(tmp->ob_digit, z->ob_digit, sizeof(uint16t) * size_z);
                    delete z;
                    z = tmp;
                    z->ob_digit[size_z] = (uint16t)c;
                    ++size_z;
                }
            }
        }
    }
    if (z == NULL)
        return NULL;
    if (str == start)
        goto onError;
    if (sign < 0)
		z->negative = true;
    if (*str == 'L' || *str == 'l')
        str++;
    while (*str && isspace(*str & 0xff))
        str++;
    if (*str != '\0')
        goto onError;
    if (pend)
        *pend = str;
    return z;

  onError:
    delete z;
    slen = strlen(orig_str) < 200 ? strlen(orig_str) : 200;
    strobj = new string(orig_str);
    if (strobj == NULL)
        return NULL;
    strrepr = new string(*strobj);
    delete strobj;
    if (strrepr == NULL)
        return NULL;
    cerr<<"invalid literal for long() with base %d: %s";
    delete strrepr;
    return NULL;
}

/* Convert a long integer to a base 10 string.  Returns a new non-shared
   string.  (Return value is non-shared so that callers can modify the
   returned value if necessary.) */
string BigInteger::biAsDecimalString(BigInteger *aa, int addL)
{
    BigInteger *scratch, *a;
    char *str;
    bi_size_t size, strlen, size_a, i, j;
    uint16t *pout, *pin, rem, tenpow;
    char *p;

    a = aa;
    if (a == NULL) 
	{
        std::cout<<"no aa\n";
        return NULL;
    }
	size_a = a->ob_digit_size;

    // quick and dirty upper bound for the number of digits
    //   required to express a in base BI_DECIMAL_BASE:

    //     #digits = 1 + floor(log2(a) / log2(BI_DECIMAL_BASE))

    //   But log2(a) < size_a * BIG_INTEGER_SHIFT, and
    //   log2(BI_DECIMAL_BASE) = log2(10) * BI_DECIMAL_SHIFT
    //                              > 3 * BI_DECIMAL_SHIFT

    if (size_a > BI_SIZE_T_MAX) 
	{
        cerr<<"long is too large to format";
        return NULL;
    }
    // the expression size_a * BIG_INTEGER_SHIFT is now safe from overflow
    size = 1 + size_a * BIG_INTEGER_SHIFT / (3 * BI_DECIMAL_SHIFT);
    scratch = new BigInteger();
	scratch->biResize(size);
    if (scratch == NULL)
        return NULL;

    // convert array of base BIG_INTEGER_BASE digits in pin to an array of
    //   base BI_DECIMAL_BASE digits in pout, following Knuth (TAOCP,
    //   Volume 2 (3rd edn), section 4.4, Method 1b). 
    pin = a->ob_digit;
    pout = scratch->ob_digit;
    size = 0;
    for (i = size_a; --i >= 0; ) 
	{
        uint16t hi = pin[i];
        for (j = 0; j < size; j++) 
		{
            uint32t z = (uint32t)pout[j] << BIG_INTEGER_SHIFT | hi;
            hi = (uint16t)(z / BI_DECIMAL_BASE);
            pout[j] = (uint16t)(z - (uint32t)hi * BI_DECIMAL_BASE);
        }
        while (hi) 
		{
            pout[size++] = hi % BI_DECIMAL_BASE;
            hi /= BI_DECIMAL_BASE;
        }
    }
    // pout should have at least one digit, so that the case when a = 0
    //   works correctly
    if (size == 0)
        pout[size++] = 0;

    // calculate exact length of output string, and allocate 
    strlen = 0;
	strlen += (addL != 0)?1:0;
	strlen += aa->negative?1:0;
	strlen += 1 + (size - 1) * BI_DECIMAL_SHIFT;
    tenpow = 10;
    rem = pout[size-1];
    while (rem >= tenpow) 
	{
        tenpow *= 10;
        strlen++;
    }
	str = (char*)MemHelper::BiMalloc(strlen+1);
	memset(str, 0, strlen);
    if (str == NULL) 
	{
		delete(scratch);
        return NULL;
    }

    // fill the string right-to-left
	p = str + strlen;
    *p = '\0';
    if (addL)
        *--p = 'L';
    // pout[0] through pout[size-2] contribute exactly
    //   BI_DECIMAL_SHIFT digits each
    for (i=0; i < size - 1; i++) 
	{
        rem = pout[i];
        for (j = 0; j < BI_DECIMAL_SHIFT; j++) 
		{
            *--p = '0' + rem % 10;
            rem /= 10;
        }
    }
    // pout[size-1]: always produce at least one decimal digit 
    rem = pout[i];
    do 
	{
        *--p = '0' + rem % 10;
        rem /= 10;
    } 
	while (rem != 0);

    // and sign 
    if (aa->negative)
        *--p = '-';

    // check we've counted correctly 
	assert(p == str);
    delete(scratch);
	string ret_str(p);
	MemHelper::BiFree(str);
    return ret_str;
}

string BigInteger::toString()
{
	return biAsDecimalString(this, 1);
}

BigInteger& BigInteger::fromString(char* str)
{
	BigInteger *ret;
	char *pend;
	if(strlen(str)<1) //如果str的长度为0，则赋其值为0 
		ret = biFromString("0", &pend, 10);
	else              //str的长度不为0， 赋值为str代表的值
		ret = biFromString(str, &pend, 10);
	*this = *ret;
	delete ret;
	return *this;
}


BigInteger BigInteger::operator*=(BigInteger& b)
{
	BigInteger *p = mul(b);
	*this = *p;
	delete p;
	return *this;
}

/* Divide long pin, w size digits, by non-zero digit n, storing quotient
   in pout, and returning the remainder.  pin and pout point at the LSD.
   It's OK for pin == pout on entry */

uint16t BigInteger::digitDivRem(uint16t *pout, uint16t *pin, bi_size_t size, uint16t n)
{
    uint32t rem = 0;

    assert(n > 0 && n <= BIG_INTEGER_MASK);
    pin += size;
    pout += size;
    while (--size >= 0) {
        uint16t hi;
        rem = (rem << BIG_INTEGER_SHIFT) | *--pin;
        *--pout = hi = (uint16t)(rem / n);
        rem -= (uint32t)hi * n;
    }
    return (uint16t)rem;
}

/* Divide a long integer by a digit, returning both the quotient
   (as function result) and the remainder (through *prem).
   The sign of a is ignored; n should not be zero. */

BigInteger *BigInteger::digitBigIntDivRem(BigInteger *a, uint16t n, uint16t *prem)
{
	const bi_size_t size = a->ob_digit_size;
    BigInteger *z;

    assert(n > 0 && n <= BIG_INTEGER_MASK);
	z = new BigInteger();
    if (z == NULL)
        return NULL;
	z->biResize(size);
    *prem = digitDivRem(z->ob_digit, a->ob_digit, size, n);
	z->normalize();
    return z;
}

/* Long division with remainder, top-level routine */
int BigInteger::biDivRem(BigInteger *a, BigInteger *b, BigInteger **pdiv, BigInteger **prem)
{
	bi_size_t size_a = a->ob_digit_size, size_b = b->ob_digit_size;
    BigInteger *z;

    if (size_b == 0) 
	{
        cerr<<"long division or modulo by zero.";
        return -1;
    }
    if (size_a < size_b ||
        (size_a == size_b &&
         a->ob_digit[size_a-1] < b->ob_digit[size_b-1])) 
	{
        // |a| < |b|.
		*pdiv = new BigInteger();
		(*pdiv)->biFromInt(0);
        if (*pdiv == NULL)
            return -1;
        *prem = new BigInteger(*a);
        return 0;
    }
    if (size_b == 1) 
	{
        uint16t rem = 0;
        z = digitBigIntDivRem(a, b->ob_digit[0], &rem);
        if (z == NULL)
            return -1;
		*prem = new BigInteger();
		(*prem)->biFromInt(rem);
        if (*prem == NULL) 
		{
            delete z;
            return -1;
        }
    }
    else 
	{
        z = biRem(a, b, prem);
        if (z == NULL)
            return -1;
    }

    //   计算正负号
    //   商 z 的符号为 a*b;
    //   余数 r 的符号为 a,
    //   所以 a = b*z + r. 
    if ((a->negative) != (b->negative))
        z->negative = true;
    if (a->negative && (*prem)->ob_digit_size != 0)
        (*prem)->negative = true;
    *pdiv = z;
    return 0;
}

/*Follow Knuth [The Art of Computer Programming, Vol. 2 (3rd
edn.), section 4.3.1, Algorithm D], except that we don't explicitly
handle the special case when the initial estimate q for a quotient
digit is >= BIG_INTEGER_BASE: the max value for q is BIG_INTEGER_BASE+1,
and that won't overflow a digit. */
BigInteger* BigInteger::biRem(BigInteger *v1, BigInteger *w1, BigInteger **prem)
{
    BigInteger *v, *w, *a;
    bi_size_t i, k, size_v, size_w;
    int d;
    uint16t wm1, wm2, carry, q, r, vtop, *v0, *vk, *w0, *ak;
    uint32t vv;
    int16t zhi;
    int32t z;

    // allocate space; w will also be used to hold the final remainder
	size_v = v1->ob_digit_size;
	size_w = w1->ob_digit_size;
    assert(size_v >= size_w && size_w >= 2); // Assert checks by div()
	v = new BigInteger();
	v->biResize(size_v+1);
    if (v == NULL) 
	{
        *prem = NULL;
        return NULL;
    }
	w = new BigInteger();
	w->biResize(size_w);
    if (w == NULL) 
	{
		delete v;
        *prem = NULL;
        return NULL;
    }

    // normalize: shift w1 left so that its top digit is >= BIG_INTEGER_BASE/2.
    //   shift v1 left by the same amount.  Results go into w and v.
    d = BIG_INTEGER_SHIFT - NumberHelper::bits_in_digit(w1->ob_digit[size_w-1]);
    carry = digitLshift(w->ob_digit, w1->ob_digit, size_w, d);
    assert(carry == 0);
    carry = digitLshift(v->ob_digit, v1->ob_digit, size_v, d);
    if (carry != 0 || v->ob_digit[size_v-1] >= w->ob_digit[size_w-1]) 
	{
        v->ob_digit[size_v] = carry;
        size_v++;
    }

    // Now v->ob_digit[size_v-1] < w->ob_digit[size_w-1], so quotient has
    //   at most (and usually exactly) k = size_v - size_w digits.
    k = size_v - size_w;
    assert(k >= 0);
	a = new BigInteger();
	a->biResize(k);
    if (a == NULL) 
	{
		delete w;
		delete v;
        *prem = NULL;
        return NULL;
    }
    v0 = v->ob_digit;
    w0 = w->ob_digit;
    wm1 = w0[size_w-1];
    wm2 = w0[size_w-2];
    for (vk = v0+k, ak = a->ob_digit + k; vk-- > v0;) 
	{
        // inner loop: divide vk[0:size_w+1] by w0[0:size_w], giving
        // single-digit quotient q, remainder in vk[0:size_w].
        // estimate quotient digit q; may overestimate by 1 (rare) 
        vtop = vk[size_w];
        assert(vtop <= wm1);
        vv = ((uint32t)vtop << BIG_INTEGER_SHIFT) | vk[size_w-1];
        q = (uint16t)(vv / wm1);
        r = (uint16t)(vv - (uint32t)wm1 * q); // r = vv % wm1
        while ((uint32t)wm2 * q > (((uint32t)r << BIG_INTEGER_SHIFT) | vk[size_w-2])) 
		{
            --q;
            r += wm1;
            if (r >= BIG_INTEGER_BASE)
                break;
        }
        assert(q <= BIG_INTEGER_BASE);

        // subtract q*w0[0:size_w] from vk[0:size_w+1]
        zhi = 0;
        for (i = 0; i < size_w; ++i) 
		{
            // invariants: -BIG_INTEGER_BASE <= -q <= zhi <= 0;
            //  -BIG_INTEGER_BASE * q <= z < BIG_INTEGER_BASE
            z = (int16t)vk[i] + zhi - (int32t)q * (int32t)w0[i];
            vk[i] = (uint16t)z & BIG_INTEGER_MASK;
            zhi = (int16t)( z>>BIG_INTEGER_SHIFT );
        }

        // add w back if q was too large (this branch taken rarely)
        assert((int16t)vtop + zhi == -1 || (int16t)vtop + zhi == 0);
        if ((int16t)vtop + zhi < 0) 
		{
            carry = 0;
            for (i = 0; i < size_w; ++i) 
			{
                carry += vk[i] + w0[i];
                vk[i] = carry & BIG_INTEGER_MASK;
                carry >>= BIG_INTEGER_SHIFT;
            }
            --q;
        }

        // store quotient digit
        assert(q < BIG_INTEGER_BASE);
        *--ak = q;
    }

    // unshift remainder; we reuse w to store the result
    carry = digitRshift(w0, v0, size_w, d);
    assert(carry==0);
	delete v;
	w->normalize();
	*prem = w;
	a->normalize();
    return a;
}

int BigInteger::biDivMod(BigInteger *v, BigInteger *w, BigInteger **pdiv, BigInteger **pmod)
{
    BigInteger *div, *mod;

    if (biDivRem(v, w, &div, &mod) < 0)
        return -1;
	if ((mod->ob_digit_size < 0 && w->ob_digit_size > 0) ||
			(mod->ob_digit_size > 0 && w->ob_digit_size < 0)) 
	{
        BigInteger *temp;
        BigInteger *one;
        temp = biAbsAdd(mod, w);
        delete mod;
        mod = temp;
        if (mod == NULL) 
		{
			delete div;
            return -1;
        }
		one = new BigInteger();
		one->biFromInt(1L);
        if (one == NULL || (temp = biAbsSub(div, one)) == NULL) 
		{
			delete mod;
			delete div;
			delete one;
            return -1;
        }
		delete one;
		delete div;
        div = temp;
    }
    if (pdiv != NULL)
        *pdiv = div;
    else
		delete div;

    if (pmod != NULL)
        *pmod = mod;
    else
		delete mod;

    return 0;
}

/* Shift digit vector a[0:m] d bits left, with 0 <= d < BIG_INTEGER_SHIFT.  Put
 * result in z[0:m], and return the d bits shifted out of the top.
 */
uint16t BigInteger::digitLshift(uint16t *z, uint16t *a, bi_size_t m, int d)
{
    bi_size_t i;
    uint16t carry = 0;

    assert(0 <= d && d < BIG_INTEGER_SHIFT);
    for (i=0; i < m; i++) 
	{
        uint32t acc = (uint32t)a[i] << d | carry;
        z[i] = (uint16t)acc & BIG_INTEGER_MASK;
        carry = (uint16t)(acc >> BIG_INTEGER_SHIFT);
    }
    return carry;
}

/* Shift digit vector a[0:m] d bits right, with 0 <= d < BIG_INTEGER_SHIFT.  Put
 * result in z[0:m], and return the d bits shifted out of the bottom.
 */
uint16t BigInteger::digitRshift(uint16t *z, uint16t *a, bi_size_t m, int d)
{
    bi_size_t i;
    uint16t carry = 0;
    uint16t mask = ((uint16t)1 << d) - 1U;

    assert(0 <= d && d < BIG_INTEGER_SHIFT);
    for (i=m; i-- > 0;) 
	{
        uint32t acc = (uint32t)carry << BIG_INTEGER_SHIFT | a[i];
        carry = (uint16t)acc & mask;
        z[i] = (uint16t)(acc >> d);
    }
    return carry;
}

BigInteger BigInteger::operator/(BigInteger &b)
{
	BigInteger *pdiv;
	biDivMod(this, &b, &pdiv, NULL);
	BigInteger a = *pdiv;
	delete pdiv;
	return a;
}

BigInteger BigInteger::operator%(BigInteger &b)
{
	BigInteger *pmod;
	biDivMod(this, &b, NULL, &pmod);
	BigInteger a = *pmod;
	delete pmod;
	return a;
}

BigInteger* BigInteger::biInvert(BigInteger *v)
{
    // Implement ~x as -(x+1)
    BigInteger *x;
    BigInteger *w;
	w = new BigInteger(1);
    if (w == NULL)
        return NULL;
	x = biAbsAdd(v, w);
    delete w;
    if (x == NULL)
        return NULL;
	x->negative = x->negative?false:true;
    return x;
}

BigInteger* BigInteger::biRshift(BigInteger *v, BigInteger *w)
{
	bool overflow = true;

	bi_size_t size = w->biAsInt(&overflow);
	if(overflow)
		return NULL;
	return biRshift(v, size);
}

BigInteger* BigInteger::biRshift(BigInteger *v, bi_size_t w)
{
    BigInteger *a;
    BigInteger *z = NULL;
    bi_size_t shiftby, newsize, wordshift, loshift, hishift, i, j;
    uint16t lomask, himask;
	bool overflow = true;

	a = v;

	if (a->ob_digit_size < 0) 
	{
        // Right shifting negative numbers is harder
        BigInteger *a1, *a2;
        a1 = (BigInteger *) biInvert(a);
        if (a1 == NULL)
		{
            return z;
		}
        a2 = (BigInteger *) biRshift(a1, w);
        delete a1;
        if (a2 == NULL)
		{
            return z;
		}
        z = (BigInteger *) biInvert(a2);
        delete a2;
    }
    else 
	{
		shiftby = w;
		if (shiftby<0) 
		{
            cerr<<"negative shift count";
            return z;
        }
        wordshift = shiftby / BIG_INTEGER_SHIFT;
		newsize = a->ob_digit_size - wordshift;
        if (newsize <= 0) 
		{
			z = new BigInteger(0);
            return z;
        }
        loshift = shiftby % BIG_INTEGER_SHIFT;
        hishift = BIG_INTEGER_SHIFT - loshift;
        lomask = ((uint16t)1 << hishift) - 1;
        himask = BIG_INTEGER_MASK ^ lomask;
		z = new BigInteger();
		z->biResize(newsize);
        if (z == NULL)
		{
            return z;
		}
		if (a->negative)
			z->negative = z->negative?false:true;
        for (i = 0, j = wordshift; i < newsize; i++, j++) 
		{
            z->ob_digit[i] = (a->ob_digit[j] >> loshift) & lomask;
            if (i+1 < newsize)
                z->ob_digit[i] |= (a->ob_digit[j+1] << hishift) & himask;
        }
		z->normalize();
    }
    return z;
}

BigInteger* BigInteger::biLshift(BigInteger *v, BigInteger *w)
{
	bool overflow = true;

	bi_size_t size = w->biAsInt(&overflow);
	if(overflow)
		return NULL;
	return biLshift(v, size);
}

BigInteger* BigInteger::biLshift(BigInteger *v, bi_size_t w)
{
    // This version due to Tim Peters
    BigInteger *a;
    BigInteger *z = NULL;
    bi_size_t shiftby, oldsize, newsize, wordshift, remshift, i, j;
    uint32t accum;
	bool overflow = true;

	a = v;
	shiftby = w;
    if (shiftby == -1L)
	{
		return z;
	}
    if (shiftby < 0) 
	{
        cerr<<"negative shift count";
		return z;
    }
    // wordshift, remshift = divmod(shiftby, BIG_INTEGER_SHIFT)
    wordshift = shiftby / BIG_INTEGER_SHIFT;
    remshift  = shiftby - wordshift * BIG_INTEGER_SHIFT;

	oldsize = a->ob_digit_size;
    newsize = oldsize + wordshift;
    if (remshift)
        ++newsize;
	z = new BigInteger();
	z->biResize(newsize);
    if (z == NULL)
	{
		return z;
	}
    if (a->negative)
		z->negative = z->negative?false:true;
    for (i = 0; i < wordshift; i++)
        z->ob_digit[i] = 0;
    accum = 0;
    for (i = wordshift, j = 0; j < oldsize; i++, j++) 
	{
        accum |= (uint32t)a->ob_digit[j] << remshift;
        z->ob_digit[i] = (uint16t)(accum & BIG_INTEGER_MASK);
        accum >>= BIG_INTEGER_SHIFT;
    }
    if (remshift)
        z->ob_digit[newsize-1] = (uint16t)accum;
    else
        assert(!accum);
	z->normalize();
    return z;
}

BigInteger BigInteger::operator<<(BigInteger &b)
{
	BigInteger *p = biLshift(this, &b);
	BigInteger ret(*p);
	delete p;
	return ret;
}
	
BigInteger BigInteger::operator<<(int b)
{
	BigInteger *p = biLshift(this, b);
	BigInteger ret(*p);
	delete p;
	return ret;
}

BigInteger& BigInteger::operator<<=(int b)
{
	BigInteger *p = biLshift(this, b);
	*this = *p;
	delete p;
	return *this;
}

BigInteger BigInteger::operator>>(BigInteger &b)
{
	BigInteger *p = biRshift(this, &b);
	BigInteger ret(*p);
	delete p;
	return ret;
}

BigInteger BigInteger::operator>>(int b)
{
	BigInteger *p = biRshift(this, b);
	BigInteger ret(*p);
	delete p;
	return ret;
}

BigInteger& BigInteger::operator>>=(int b)
{
	BigInteger *p = biRshift(this, b);
	*this = *p;
	delete p;
	return *this;
}

bool BigInteger::isEven()
{
	if(ob_digit_size == 0)
		return true;
	return (ob_digit[0] & 1) == 0;
}

bool BigInteger::isZero()
{
	return ob_digit_size == 0;
}

int BigInteger::biCompare(BigInteger *a, BigInteger *b)
{
    bi_size_t sign;
	bi_size_t a_s, b_s;
	a_s = a->negative?-a->ob_digit_size:a->ob_digit_size;
	b_s = b->negative?-b->ob_digit_size:b->ob_digit_size;
	if (a_s != b_s) 
	{
        sign = a_s - b_s;
    }
    else 
	{
		bi_size_t i = a->ob_digit_size;
        while (--i >= 0 && a->ob_digit[i] == b->ob_digit[i])
            ;
        if (i < 0)
            sign = 0;
        else 
		{
            sign = (int16t)a->ob_digit[i] - (int16t)b->ob_digit[i];
            if (a_s < 0)
                sign = -sign;
        }
    }
    return sign < 0 ? -1 : sign > 0 ? 1 : 0;
}

bool BigInteger::operator==(int b)
{
	BigInteger bb(b);
	return (*this - bb).isZero();
}

bool BigInteger::operator==(BigInteger &b)
{
	return (*this-b).isZero();
}