//////////////////////////////////////////////////////////////////////////
//
//  Fixed Point Math Class
//
//////////////////////////////////////////////////////////////////////////
//
//  Released under GNU license
//		Erik H Gawtry
//			July, 2005	Version 1.0
//
//
//  Algorythms borrowed from:
//		Andrew Ryder, 11 September 2001
//      Joseph Hall, Unknown Date
//
//
//////////////////////////////////////////////////////////////////////////
//
// Written for doing fixed point math on DSP processors
//
//////////////////////////////////////////////////////////////////////////

#include "fixed.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <common/DefinesAssert.h>
#include <common/DefinesString.h>
#include <common/Logger.h>

#define _XPI      31415 // 3.1415926535897932384626433832795
fixed fixed::XPI = fixed(true,_XPI);
#define _X2PI     62831 // 6.283185307179586476925286766559
fixed fixed::X2PI =    fixed(true,_X2PI);
#define _XPIO2    15707 // 1.5707963267948966192313216916398
fixed fixed::XPIO2 =   fixed(true,_XPIO2);
#define _XPIO4    7853 // 0.78539816339744830961566084581988
#define XPIO4    fixed(true, _XPIO4)
#define _XLN_E    27182 // 2.71828182845904523536
#define XLN_E    fixed(true,_XLN_E)
#define _XLN_10   23025 // 2.30258509299404568402
#define XLN_10   fixed(true,_XLN_10)

fixed fixed::MAX_FIXED(true, INT_MAX); // 32 bit

inline int fixed_internal_mult(int a, int b)
{
	int c = 0;
	if (sizeof(long) == 8)
	{
		c = (int)((((long)a)*((long)b))/FIXED_RESOLUTION);
	}
	else if (sizeof(long long) == 8)
	{
		c = (int)((((long long)a)*((long long)b))/FIXED_RESOLUTION);
	}
	return c;
}

inline int fixed_internal_div(int a, int b)
{
	if(b == 0) return 0xFFFFFFF;

	int c = 0;
	if (sizeof(long) == 8)
	{
		c = (int)((((long)a)*((long)FIXED_RESOLUTION))/b);
	}
	else if (sizeof(long long) == 8)
	{
		c = (int)((((long long)a)*((long long)FIXED_RESOLUTION))/b);
	}
	return c;
}

fixed::fixed(const char *nVal)
{
	if (strlen(nVal) > 12)
	{
		Logger::log(S3D::formatStringBuffer("Warning: Ignoring fixed string conversion of %s", nVal));

		m_nVal = 0;
		return;
	}

	char i[15];
	char f[15];

	int ip = 0;
	int fp = 0;
	bool fract = false;
	bool neg = false;
	for (const char *c=nVal; *c; c++)
	{
		if (*c == '.') fract = true;
		else if (*c == '-') neg = true;
		else 
		{
			if (fract) f[fp++] = *c;
			else i[ip++] = *c;
		}
	}

	for (; fp<4; fp++)
	{
		f[fp] = '0';
	}
	i[ip] = '\0';
	f[fp] = '\0';
	DIALOG_ASSERT(ip < 15 && fp < 15);

	int ipa = atoi(i);
	int fpa = atoi(f);

	m_nVal = ipa * FIXED_RESOLUTION + fpa;
	if (neg) m_nVal =- m_nVal;
}

const char *fixed::asString()
{
	static char result[15];
	int r = 0;

	char buffer[15];
	if (m_nVal < 0) 
	{
		snprintf(buffer, 15, "%i", -m_nVal);
		result[r++] = '-';
	}
	else 
	{
		snprintf(buffer, 15, "%i", m_nVal);
	}
	int len = strlen(buffer);

	if (len <= 4)
	{
		result[r++] = '0';
		result[r++] = '.';
		for (int i=len; i<4; i++)
		{
			result[r++] = '0';
		}
		for (int i=0; i<len; i++)
		{
			result[r++] = buffer[i];
		}
	}
	else
	{
		for (int i=0; i<len-4; i++)
		{
			result[r++] = buffer[i];
		}
		result[r++] = '.';
		for (int i=len-4; i<len; i++)
		{
			result[r++] = buffer[i];
		}
	}

	result[r++] = '\0';
	DIALOG_ASSERT(r < 15);

	return result;
}

fixed fixed::operator*(fixed b)
{
	return fixed(true, fixed_internal_mult(m_nVal, b.m_nVal));
}

fixed fixed::operator/(fixed b)
{
	return fixed(true, fixed_internal_div(m_nVal, b.m_nVal));
}

fixed fixed::operator*=(fixed b)
{
	m_nVal = fixed_internal_mult(m_nVal, b.m_nVal);
	return *this;
}

fixed fixed::operator/=(fixed b)
{
	m_nVal = fixed_internal_div(m_nVal, b.m_nVal);
	return *this;
}

#define sqrt_step(shift) \
    if((0x40000000l >> shift) + root <= value)          \
    {                                                   \
        value -= (0x40000000l >> shift) + root;         \
        root = (root >> 1) | (0x40000000l >> shift);    \
    }                                                   \
    else                                                \
    {                                                   \
        root = root >> 1;                               \
    }

static int iSqrt(int value)
{
    int root = 0;

    sqrt_step( 0);
    sqrt_step( 2);
    sqrt_step( 4);
    sqrt_step( 6);
    sqrt_step( 8);
    sqrt_step(10);
    sqrt_step(12);
    sqrt_step(14);
    sqrt_step(16);
    sqrt_step(18);
    sqrt_step(20);
    sqrt_step(22);
    sqrt_step(24);
    sqrt_step(26);
    sqrt_step(28);
    sqrt_step(30);

    // round to the nearest integer, cuts max error in half

    if(root < value)
    {
        ++root;
    }

    return root;
}

fixed absx( fixed p_Base )
{
	if( p_Base < fixed(0) ) return -p_Base;
	return p_Base;
}
/*
                     (x^h) - 1
   ln(x)  =   lim    -------      
             h -> 0     h

*/

static fixed iLog2( fixed p_Base )
{   
    fixed w = 0;
	fixed y = 0;
	fixed z = 0;
	fixed num = 1;
	fixed dec = 0;

	if( p_Base == fixed(1) )
		return 0;

	for( dec=fixed(0) ; absx( p_Base ) >= fixed(2) ; ++dec )
		p_Base /= XLN_E;

	p_Base -= fixed(1);
	z = p_Base;
	y = p_Base;
	w = 1;

	while( y != y + w )
		y += ( w = ( z = fixed(0) - ( z * p_Base ) ) / ( fixed(num) += fixed(1) ) );

	return y + fixed(dec);
}

/*
	calculate the exponential function using the following series :

                          x^2     x^3     x^4     x^5
	exp(x) == 1  +  x  +  ---  +  ---  +  ---  +  ---  ...
                           2!      3!      4!      5!

*/

static fixed iExp2(fixed p_Base)
{
	fixed w;
	fixed y;
	fixed num;

	for( w=fixed(1), y=fixed(1), num=fixed(1) ; y != y+w ; ++num )
		y += ( w *= p_Base / fixed(num) );

	return y;
}

static fixed ipow( fixed p_Base, fixed p_Power )
{
	if( p_Base < fixed(0) && p_Power%2 != fixed(0) )
		return - iExp2( (p_Power * iLog2( -p_Base )) );
	else
		return iExp2( (p_Power * iLog2(absx( p_Base ))) );
}

static fixed ilog10( fixed p_Base )
{
	return iLog2( p_Base ) / XLN_10;
}

fixed fixed::sqrt()
{
	int val = iSqrt(m_nVal);
	val *= 100;
	return fixed(true, val);
}

fixed sqrtx(fixed fixedVal)
{
	return fixedVal.sqrt();
}

fixed fixed::pow(fixed fixedPower)
{
	return ipow(*this, fixedPower);
}

fixed powx(fixed fixedVal, fixed fixedPower)
{
	return fixedVal.pow(fixedPower);
}

fixed fixed::exp()
{
	return iExp2(*this);
}

fixed expx(fixed fixedVal)
{
	return fixedVal.exp();
}

fixed fixed::log10()
{
	return ilog10(*this);
}

fixed log10x(fixed fixedVal)
{
	return fixedVal.log10();
}

fixed fixed::log()
{
	return iLog2(*this);
/*
	Calculate the POW function by calling EXP :

                  Y      A                 
                 X   =  e    where A = Y * log(X)
*/
}

fixed logx(fixed fixedVal)
{
	return fixedVal.log();
}

fixed floorx(fixed fixedVal)
{
	return fixedVal.floor();
}

fixed ceilx(fixed fixedVal)
{
	return fixedVal.ceil();
}

//
// Taylor Algorythm
// x - x^3/3! + x^5/5! - x^7/7! + x^9/9! ........    
//
// Note: Make xresult a float to get more precision
//
// Only accurate from -PI/2 to PI/2

static fixed _sinx(fixed x)
{
	fixed xpwr;
	int xftl;
	fixed xresult;
	bool positive;

	xresult = 0;
	xpwr = x;
	xftl = 1;
	positive = true;

	// Note: 12! largest for int
	for(int i = 1; i < 7; i+=2)
	{
		if( positive )
			xresult += (xpwr/fixed(xftl));
		else
			xresult -= (xpwr/fixed(xftl));

		xpwr *= x;
		xpwr *= x;
		xftl *= i+1;
		xftl *= i+2;
		positive = !positive;
	}

	return xresult;
}

fixed fixed::sin()
{
	fixed xresult;
	bool bBottom = false;
	static fixed xPI = XPI;
	static fixed x2PI = X2PI;
	static fixed xPIO2 = XPIO2;

	fixed x(true, m_nVal%_X2PI);
	if( x < fixed(0) )
		x += x2PI;

	if( x > xPI )
	{
		bBottom = true;
		x -= xPI;
	}

	if( x <= xPIO2 )
		xresult = _sinx(x);
	else
		xresult = _sinx(xPIO2-(x-xPIO2));

	if( bBottom )
		return -xresult;

	return xresult;
}

fixed sinx(fixed x)
{
	return x.sin();
}

fixed fixed::acos()
{
	return fixed(0);
}

fixed fixed::cos()
{
	fixed xresult;
	bool bBottom = false;
	static fixed xPI = XPI;
	static fixed x2PI = X2PI;
	static fixed xPIO2 = XPIO2;

	fixed x(true, (m_nVal+_XPIO2)%_X2PI);
	if( x < fixed(0) )
		x += x2PI;

	if( x > xPI )
	{
		bBottom = true;
		x -= xPI;
	}

	if( x <= xPIO2 )
		xresult = _sinx(x);
	else
		xresult = _sinx(xPIO2-(x-xPIO2));

	if( bBottom )
		return -xresult;

	return xresult;
}

fixed cosx(fixed x)
{
	return x.cos();
}

fixed fixed::tan()
{
	return sin()/cos();
}

fixed tanx(fixed x)
{
	return x.tan();
}

fixed fixed::fromFloat(float flt)
{
	fixed result;
	result.m_nVal = int(flt * FIXED_RESOLUTION_FLOAT);
	return result;
}

static fixed CON1(true, 2679);	/* 2 - sqrt(3)			*/
static fixed ROOT_3M1(true, 7321);	/* sqrt(3) - 1			*/
static fixed ROOT_3(true, 17321); /* square root of 3 */

static fixed p0 = fixed(true, -136887);
static fixed p1 = fixed(true, -205058);
static fixed p2 = fixed(true, -84946);
static fixed p3 = fixed(true, -8375);
static fixed q0 = fixed(true,  410663);
static fixed q1 = fixed(true,  861573);
static fixed q2 = fixed(true,  595784);
static fixed q3 = fixed(true,  150240);

static fixed at[] = 
{
    0,
    fixed(true, 5235),		/* pi / 6			*/
	fixed::XPIO2,			/* pi / 2			*/
    fixed(true, 10472)		/* pi / 3			*/
};

static fixed _atanx(fixed f, int n)
{
    fixed g, q, r;

    if (f > CON1) 
	{
		f = (((ROOT_3M1 * f - fixed(true, 5000)) - fixed(true, 5000)) + f) / (ROOT_3 + f);
		n++;
    }
	{
		g = f * f;
		q = (((g + q3)*g + q2)*g + q1)*g + q0;
		r = (((p3*g + p2)*g + p1)*g + p0)*g / q;
		f = f + f * r;
    }

	if (n > 1) f = -f;
    return(f + at[n]);
}

fixed atanx(fixed x)
{
    fixed f = x < 0 ? -x : x;
    if (f > 1) f = _atanx(fixed(1) / f, 2);
    else f = _atanx(f, 0);

    return(x < fixed(0) ? -f : f);
}

fixed atan2x(fixed v, fixed u)
{
	fixed f;
	fixed av = v < 0 ? -v : v;
	fixed au = u < 0 ? -u : u;

	if (u != 0) 
	{
		if (av > au) 
		{
			if ((f = au / av) == 0) f = fixed::XPIO2;
			else f = _atanx(f, 2);
		}
		else 
		{
			if ((f = av / au) == 0) f = 0;
			else f = _atanx(f, 0);
		}
	}
	else 
	{
		if (v != 0) f = fixed::XPIO2;
		else 
		{
			//cmemsg(FP_ATAN, &v);
			f = 0;
		}
	}
	if (u < 0) f = fixed::XPI - f;

	return(v < 0 ? -f : f);
}
