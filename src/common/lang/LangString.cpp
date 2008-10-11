////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#include <lang/LangString.h>
#include <common/DefinesString.h>

LangString LangStringUtil::convertToLang(const std::string &input)
{
	LangString result;
	appendToLang(result, input);
	return result;
}

void LangStringUtil::appendToLang(LangString &output, const std::string &input)
{
	for (const char *i=input.c_str(); *i; i++)
	{
		if (*i == '\\')
		{
			const unsigned int next = *(i+1);
			switch (next)
			{
			case '\\':
				output.push_back('\\');
				i++;
				break;
			case 'n':
				output.push_back('\n');
				i++;
				break;
			case 't':
				output.push_back('\t');
				i++;
				break;
			case 'u':
				{
					char a[] = { *(i+2), *(i+3), *(i+4), *(i+5), '\0' };
					unsigned int value;
					sscanf(a, "%04X", &value);
					output.push_back(value);
					i+=5;
				}
				break;
			}
		}
		else
		{
			output.push_back(*i);
		}
	}
}

void LangStringUtil::replaceToLang(LangString &output, const std::string &input)
{
	output.clear();
	appendToLang(output, input);
}

std::string LangStringUtil::convertFromLang(const LangString &input)
{
	std::string result;
	for (const unsigned int *i=input.c_str(); *i; i++)
	{
		if (*i < 32 || *i > 126)
		{
			if (*i == '\n') result.append("\\n");
			else if (*i == '\\') result.append("\\\\");
			else if (*i == '\n') result.append("\\t");
			else result.append(S3D::formatStringBuffer("\\u%04X", *i));
		}
		else
		{
			result.push_back((char) *i);
		}
	}
	
	return result;
}

