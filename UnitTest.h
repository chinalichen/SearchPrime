/*	UintTest.h
 *	create by Phaeton
 *	2013.12.28
 */

#include "stdafx.h"
#include "BigInteger.h"

using std::cout;

//unit test
void add_test()
{
	int result = 0;
	bool overflow;

	BigInteger a1 = 10, a2 = 20;  //正+正
	if((a1+a2).biAsInt(&overflow)!=10+20 || overflow)
	{
		cout<<"add error: 1\n";
		result++;
	}

	BigInteger b1 = 0, b2 = 0;    //0+0
	if((b1+b2).biAsInt(&overflow)!=0 || overflow)
	{
		cout<<"add error: 2\n";
		result++;
	}

	BigInteger c1 = -10, c2 = -20;//负+负
	if((c1+c2).biAsInt(&overflow)!=-10-20 || overflow)
	{
		cout<<"add error: 3\n";
		result++;
	}

	BigInteger d1 = 10, d2 = -10; //正+负
	if((d1+d2).biAsInt(&overflow)!=0 || overflow)
	{
		cout<<"add error: 4\n";
		result++;
	}

	BigInteger e1 = -10, e2 = 20; //负+正
	if((e1+e2).biAsInt(&overflow)!=10 || overflow)
	{
		cout<<"add error: 5\n";
		result++;
	}

	BigInteger f1 = 0x7fff, f2 = 1;
	if((f1+f2).biAsInt(&overflow)!=0x7fff+1 || overflow)
	{
		cout<<"add error: 6\n";
		result++;
	}

	if(result == 0)
		cout<<"add test OK!\n";
	else
		cout<<result<<" errors occured in add test\n";
}

void sub_test()
{
	int result = 0;
	bool overflow;

	BigInteger a1 = 10, a2 = 20;   //正-正
	if((a1-a2).biAsInt(&overflow)!=10-20 || overflow)
	{
		cout<<"sub error: 1\n";
		result++;
	}

	BigInteger b1 = 0, b2 = 0;     //0-0
	if((b1-b2).biAsInt(&overflow)!=0 || overflow)
	{
		cout<<"sub error: 2\n";
		result++;
	}

	BigInteger c1 = -10, c2 = -20; //负-负
	BigInteger c3 = c1-c2;
	if(c3.biAsInt(&overflow)!=10 || overflow)
	{
		cout<<"sub error: 3\n";
		result++;
	}

	BigInteger d1 = 10, d2 = -10;  //正+负
	if((d1-d2).biAsInt(&overflow)!=20 || overflow)
	{
		cout<<"sub error: 4\n";
		result++;
	}

	BigInteger e1 = -10, e2 = 10;  //负+正
	if((e1-e2).biAsInt(&overflow)!=-20 || overflow)
	{
		cout<<"sub error: 5\n";
		result++;
	}

	BigInteger f1 = 0x8000, f2 = 1;
	if((f1-f2).biAsInt(&overflow)!=0x8000-1 || overflow)
	{
		cout<<"sub error: 6\n";
		result++;
	}

	if(result == 0)
		cout<<"sub test OK!\n";
	else
		cout<<result<<" errors occured in sub test\n";
}

void mul_test()
{
	int result = 0;
	bool overflow;

	BigInteger a1 = 10, a2 = 20;   //正*正
	if((a1*a2).biAsInt(&overflow)!=200 || overflow)
	{
		cout<<"mul error: 1\n";
		result++;
	}

	BigInteger b1 = 0, b2 = 0;     //0*0
	if((b1*b2).biAsInt(&overflow)!=0 || overflow)
	{
		cout<<"mul error: 1\n";
		result++;
	}

	BigInteger c1 = -10, c2 = -20; //负*负
	BigInteger c3 = c1*c2;
	if(c3.biAsInt(&overflow)!=200 || overflow)
	{
		cout<<"mul error: 2\n";
		result++;
	}

	BigInteger d1 = 10, d2 = -10;  //正*负
	if((d1*d2).biAsInt(&overflow)!=-100 || overflow)
	{
		cout<<"mul error: 3\n";
		result++;
	}

	BigInteger e1 = -10, e2 = 10;  //负*正
	if((e1*e2).biAsInt(&overflow)!=-100 || overflow)
	{
		cout<<"mul error: 4\n";
		result++;
	}

	BigInteger f1 = 0x7fff, f2 = 2;
	if((f1*f2).biAsInt(&overflow)!=0x7fff*2 || overflow)
	{
		cout<<"mul error: 5\n";
		result++;
	}

	BigInteger g1 = 0x7ffff, g2 = 0x2;
	if((g1*g2).biAsInt(&overflow)!=0x7ffff*2 || overflow)
	{
		cout<<"mul error: 6\n";
		result++;
	}

	if(result == 0)
		cout<<"mul test OK!\n";
	else
		cout<<result<<" errors occured in mul test\n";
}

void div_test()
{
	int result = 0;
	bool overflow;

	BigInteger a1 = 10, a2 = 20;   //正/正
	if((a1/a2).biAsInt(&overflow)!=0 || overflow)
	{
		cout<<"div error: 1\n";
		result++;
	}

	BigInteger b1 = 0, b2 = 90;     //0*0
	if((b1/b2).biAsInt(&overflow)!=0 || overflow)
	{
		cout<<"div error: 2\n";
		result++;
	}

	BigInteger c1 = -10, c2 = -2; //负*负
	BigInteger c3 = c1/c2;
	if(c3.biAsInt(&overflow)!=5 || overflow)
	{
		cout<<"div error: 3\n";
		result++;
	}

	BigInteger d1 = -10, d2 = 2;  //正*负
	if((d1/d2).biAsInt(&overflow)!=-5 || overflow)
	{
		cout<<"div error: 4\n";
		result++;
	}

	BigInteger e1 = 10, e2 = -2;  //负*正
	if((e1/e2).biAsInt(&overflow)!=-5 || overflow)
	{
		cout<<"div error: 5\n";
		result++;
	}

	BigInteger f1 = 0xffff, f2 = 2;
	if((f1/f2).biAsInt(&overflow)!=0x7fff || overflow)
	{
		cout<<"div error: 6\n";
		result++;
	}

	BigInteger g1 = 0x7ffff, g2 = 2;
	if((g1/g2).biAsInt(&overflow)!=0x7ffff/2 || overflow)
	{
		cout<<"div error: 7\n";
		result++;
	}

	if(result == 0)
		cout<<"div test OK!\n";
	else
		cout<<result<<" errors occured in div test\n";
}

void mod_test()
{
	int result = 0;
	bool overflow;

	BigInteger a1 = 10, a2 = 20;   //正/正
	if((a1%a2).biAsInt(&overflow)!=10 || overflow)
	{
		cout<<"mod error: 1\n";
		result++;
	}

	BigInteger b1 = 0, b2 = 90;     //0*0
	if((b1%b2).biAsInt(&overflow)!=0 || overflow)
	{
		cout<<"mod error: 2\n";
		result++;
	}

	BigInteger c1 = 10, c2 = 2; //负*负
	BigInteger c3 = c1%c2;
	if(c3.biAsInt(&overflow)!=0 || overflow)
	{
		cout<<"mod error: 3\n";
		result++;
	}

	BigInteger d1 = -10, d2 = 2;  //正*负
	if((d1%d2).biAsInt(&overflow)!=0 || overflow)
	{
		cout<<"mod error: 4\n";
		result++;
	}

	BigInteger e1 = 3, e2 = 7;  //负*正
	if((e1%e2).biAsInt(&overflow)!=3 || overflow)
	{
		cout<<"mod error: 5\n";
		result++;
	}

	BigInteger f1 = 0xffff, f2 = 2;
	if((f1%f2).biAsInt(&overflow)!=1 || overflow)
	{
		cout<<"mod error: 6\n";
		result++;
	}

	BigInteger g1 = 0x7ffff, g2 = 2;
	if((g1%g2).biAsInt(&overflow)!=1 || overflow)
	{
		cout<<"mod error: 7\n";
		result++;
	}

	if(result == 0)
		cout<<"mod test OK!\n";
	else
		cout<<result<<" errors occured in mod test\n";
}

void string_convert_test()
{
	int result = 0;

	BigInteger a;
	char *digit_stra =	"12345612345875421354687564125648976" //测试35*4=140位的十进制数转换
						"54213564854621356489756421798912356"
						"47892315648792135467892135467892315"
						"46789132465789756461324567898465432";
	a.fromString(digit_stra);
	if( strncmp(digit_stra, a.toString().c_str(), strlen(digit_stra)))
	{
		cout<<"string convert error: 1\n";
		result++;
	}

	BigInteger b;
	char *digit_strb = "";		//长度为0的字符串转换
	b.fromString(digit_strb);
	if( strncmp("0", b.toString().c_str(), 1) )
	{
		cout<<"string convert error: 2\n";
		result++;
	}

	if(result == 0)
		cout<<"string convert test OK!\n";
	else
		cout<<result<<" errors occured in string convert test\n";
}