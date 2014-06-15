////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <stdlib.h>
#include <stdarg.h>
#include <common/Defines.h>

void S3D::trim(std::string &value)
{
	size_t start = value.find_first_not_of(" \t\n");
	size_t end = value.find_last_not_of(" \t\n");
	if (start == std::string::npos) value = "";
	else if (end == std::string::npos) value = "";
	else value = std::string(value, start, end-start+1);
}

char *S3D::stristr(const char *x, const char *y)
{
	std::string newX(x);
	std::string newY(y);
	_strlwr((char *) newX.c_str());
	_strlwr((char *) newY.c_str());

	char *result = (char *) strstr(newX.c_str(), newY.c_str());
	if (!result) return 0;

	return (char *)(x + (result - newX.c_str()));
}

#ifndef va_copy
# define va_copy(d, s)		(d) = (s)
#endif

std::string &S3D::formatStringList(const char *format, va_list ap)
{
	int size = 256;
	char *p = new char[256];
	va_list ap_copy;

	while (1) 
	{
		/* Try to print in the allocated space. */
		va_copy(ap_copy, ap);
		int n = vsnprintf (p, size, format, ap_copy);
		va_end(ap_copy);

		/* If that worked, return the string. */
		if (n > -1 && n < size) break;

		/* Check size is within limits */
		if (size > 10 * 1024) break;

		/* Else try again with more space. */
		if (n > -1)    /* glibc 2.1 */
			size = n+1; /* precisely what is needed */
		else           /* glibc 2.0 */
			size *= 2;  /* twice the old size */

		/* Allocate more space */
		delete [] p;
		p = new char[size];
	}

	std::string &result = S3D::getThreadLocalStringCopy(p);
	delete [] p;

	return result;
}

std::string &S3D::formatStringBuffer(const char *format, ...)
{
	va_list ap; 
	va_start(ap, format); 
	std::string &result = S3D::formatStringList(format, ap);
	va_end(ap); 

	return result;
}

std::string &S3D::formatMoney(int amount)
{
	if (abs(amount) < 1000)
	{
		return S3D::formatStringBuffer("$%i", amount);
	}
	else
	{
		int thou = amount / 1000;
		amount = abs(amount) - abs(thou * 1000);
		return S3D::formatStringBuffer("$%i,%03i", thou, amount);
	}
}

class ThreadLocalStringPool
{
public:
	ThreadLocalStringPool() : position_(0)
	{
		stringPool_ = new std::string[10];
	}
	~ThreadLocalStringPool()
	{
		delete []stringPool_;
		stringPool_ = 0;
	}

	std::string &getNext() 
	{ 
		position_++; 
		return stringPool_[position_%10]; 
	}

private:
	int position_;
	std::string *stringPool_;
};

#include <boost/thread/tss.hpp>
static boost::thread_specific_ptr<ThreadLocalStringPool> threadLocalStringPool;

std::string &S3D::getThreadLocalString()
{
	if (!threadLocalStringPool.get())
	{
		threadLocalStringPool.reset(new ThreadLocalStringPool());
	}
	return threadLocalStringPool->getNext();
}

std::string &S3D::getThreadLocalStringCopy(const std::string &other)
{
	std::string &result = S3D::getThreadLocalString();
	result.clear();
	result.append(other);
	return result;
}

void S3D::strtok(const std::string &str, const std::string &tok, std::list<std::string> &result)
{
	char *tmp = new char[str.length() + 1];
	tmp[0] = '\0';
	strcpy(tmp, str.c_str());
	for (char *token = ::strtok(tmp, tok.c_str());
		token != 0;
		token = ::strtok(0, tok.c_str()))
	{
		result.push_back(token);
	}
	delete[] tmp;
}

std::string &S3D::replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return str;
	str.replace(start_pos, from.length(), to);
	return str;
}