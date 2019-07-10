/*	SearchPrime.cpp
 *	create by Phaeton
 *	2013.12.28
 */

#include "stdafx.h"
#include "BigInteger.h"
#include "UnitTest.h"

using std::cout;
using std::string;

int BigInteger::obj_sum = 0;


/*已知p，计算公式p-1 = 2^s * d中的s和d*/
void two_power_one(BigInteger p, int *s, BigInteger *d)
{
	*s=0;
    *d=1;
    --p;
    while(1)
	{
		if(!p.isEven())
		{
            *d = p;
            break;
		}
        else
		{
            ++(*s);
            p >>= 1;
		}
	}
}

/*幂模运算,计算a^b%n
  幂模运算一般会根据公式
  (a*b)%n = (a%n)*(b%n)%n
  转化成多个乘模运算*/
BigInteger pow_mod(BigInteger a, BigInteger b, BigInteger n)
{
	BigInteger d(1);
	while(!b.isZero())
	{
		if(!b.isEven())
		{
			d = (d * a)%n;
		}
		a = (a * a)%n;
		b>>=1;
		//cout<<"b:"<<b.toString()<<"  "<<b.getSize()<<endl;
	}
	return d;
}

/*Miller-Rabin 素性算法*/
bool is_prime(BigInteger &p)
{
	BigInteger c(0), a(1), d(1), base_two(1);
	int s=0;
	two_power_one(p, &s, &d);

	for(int i=0;i<4;i++)
	{
		a = rand()%111;
		base_two = 1;
		for(int r=0;r<s;r++)
		{
			if( (pow_mod(a, d, p) == 1) || (pow_mod(a, base_two*d, p) == p-1) )
				return true;
			//cout<<"r="<<r<<"\n";
			base_two<<=1;
		}
	}
	return false;
}

//一直判断质数
void print_constantly()
{
	BigInteger p(1);
	p<<=1024;
	p+=2;

	while(1)
	{
		if(p.isEven())
		{
			++p;
			continue;
		}
		cout<<p.toString();
		if(is_prime(p))
		{
			cout<<" ---- yes.\n"<<endl;
		}
		else
		{
			cout<<" ---- no."<<endl;
		}
		++p;
	}
}

/*从p=2^1024 + 2开始计算 1000 个质数
 *		2^1024 + 2 = "17976931348623159077293051907890247336179769789423065727343008115773267580550096
 *                    31327084773224075360211201138798713933576587897688144166224928474306394741243777
 *                    67893424865485276302219601246094119453082952085005768838150682342462881473913110
 *                    540827237163350510684586298239947245938479716304835356329624224137218L"
 */	
void print_1000_prime()
{
	BigInteger p(1);
	p<<=1024;
	p+=2;

	int i=1000;
	while(i)
	{
		if(p.isEven())
		{
			++p;
			continue;
		}
		cout<<p.toString();
		if(is_prime(p))
		{
			--i;
			cout<<" ---- yes.\n"<<endl;
		}
		else
		{
			cout<<" ---- no."<<endl;
		}
		++p;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	//print_1000_prime();//从p=2^1024 + 2开始计算 1000 个质数
	print_constantly();//从p=2^1024 + 2开始一直判断质数
	
	return 0;
}


