////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <stdlib.h>
#include <stdarg.h>
#include <common/Defines.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

void S3D::trim(std::string &value)
{
	int start = value.find_first_not_of(" \t\n");
	int end = value.find_last_not_of(" \t\n");
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

std::string S3D::formatStringList(const char *format, va_list ap)
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

	std::string result(p);
	delete [] p;

	return result;
}

std::string S3D::formatStringBuffer(const char *format, ...)
{
	va_list ap; 
	va_start(ap, format); 
	std::string result = S3D::formatStringList(format, ap);
	va_end(ap); 

	return result;
}


