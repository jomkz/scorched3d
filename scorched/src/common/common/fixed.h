//////////////////////////////////////////////////////////////////////////
//
//  Fixed Point Math Class
//
//////////////////////////////////////////////////////////////////////////
//
//  Released under GNU license
//		Erik H Gawtry
//			July, 2005	Version 1.0
//		Altered G Camp
//			Aug, 2007 Version 1.1
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

#ifndef _FIXED_H
#define _FIXED_H

#define	FIXED_RESOLUTION		10000
#define	FIXED_RESOLUTION_FLOAT	10000.0f

class fixed
{
private:
	int	m_nVal;
public:
	fixed()
	{
		m_nVal = 0;
	}

	fixed(const fixed& fixedVal)
	{
		m_nVal = fixedVal.m_nVal;
	}

	fixed(bool bInternal, int nVal)
	{
		m_nVal = nVal;
	}

	fixed(unsigned int nVal)
	{
		m_nVal = nVal*FIXED_RESOLUTION;
	}

	fixed(int nVal)
	{
		m_nVal = nVal*FIXED_RESOLUTION;
	}

	fixed(const char *nVal);

	~fixed()
	{
	}

	fixed operator++()
	{
		m_nVal += FIXED_RESOLUTION;
		return *this;
	}

	fixed operator--()
	{
		m_nVal -= FIXED_RESOLUTION;
		return *this;
	}

	fixed operator-()
	{
		return fixed(0) - *this;
	}

	fixed& operator=(fixed fixedVal)
	{
		m_nVal = fixedVal.m_nVal;
		return *this;
	}

	bool operator==(fixed fixedVal)
	{
		return (m_nVal == fixedVal.m_nVal);
	}

	bool operator!=(fixed fixedVal)
	{
		return (m_nVal != fixedVal.m_nVal);
	}

	bool operator<(fixed fixedVal)
	{
		return (m_nVal < fixedVal.m_nVal);
	}

	bool operator<=(fixed fixedVal)
	{
		return (m_nVal <= fixedVal.m_nVal);
	}

	bool operator>(fixed fixedVal)
	{
		return (m_nVal > fixedVal.m_nVal);
	}

	bool operator>=(fixed fixedVal)
	{
		return (m_nVal >= fixedVal.m_nVal);
	}

	const char *asString();

	float asFloat()
	{
		return m_nVal/FIXED_RESOLUTION_FLOAT;
	}

	int asInt()
	{
		return (int)(m_nVal/FIXED_RESOLUTION);
	}

	int getInternal() 
	{ 
		return m_nVal; 
	}

	int	*getInternalData() 
	{
		return &m_nVal;
	}

	fixed floor()
	{
		return fixed(m_nVal/FIXED_RESOLUTION);
	}

	fixed ceil()
	{
		return fixed(m_nVal/FIXED_RESOLUTION+1);
	}

	fixed operator+(fixed b)
	{
		fixed a;
		a.m_nVal = m_nVal+b.m_nVal;
		return a;
	}

	fixed operator-(fixed b)
	{
		fixed a;
		a.m_nVal = m_nVal-b.m_nVal;
		return a;
	}

	fixed operator*(fixed b);
	fixed operator/(fixed b);
	fixed sqrt();
	fixed pow(fixed fixedPower);
	fixed log10();
	fixed log();
	fixed exp();
	fixed cos();
	fixed sin();
	fixed tan();
	fixed asin();
	fixed acos();
	fixed atan();

	fixed abs()
	{
		if (m_nVal > 0) return fixed(*this);
		else return fixed(true, -m_nVal);
	}

	fixed operator%(fixed fixedVal)
	{
		fixed a;
		a.m_nVal = m_nVal%fixedVal.m_nVal;
		return a;
	}

	fixed operator*=(fixed val);
	fixed operator/=(fixed val);

	fixed operator-=(fixed val)
	{
		m_nVal -= val.m_nVal;
		return *this;
	}

	fixed operator+=(fixed val)
	{
		m_nVal += val.m_nVal;
		return *this;
	}

	static fixed MAX_FIXED;
	static fixed XPI;
	static fixed X2PI;
	static fixed XPIO2;

	static fixed fromFloat(float flt);
};

fixed absx( fixed p_Base );
fixed floorx(fixed fixedVal);
fixed ceilx(fixed fixedVal);
fixed sqrtx(fixed fixedVal);
fixed powx(fixed fixedVal, fixed fixedPower);
fixed log10x(fixed fixedVal);
fixed logx(fixed fixedVal);
fixed expx(fixed fixedVal);
fixed sinx(fixed x);
fixed asinx(fixed x);
fixed cosx(fixed x);
fixed acosx(fixed x);
fixed tanx(fixed x);
fixed atanx(fixed x);
fixed atan2x(fixed x, fixed y);

#endif // _FIXED_H

