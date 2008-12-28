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

#include <lang/LangParam.h>
#include <common/DefinesString.h>

LangParam::LangParam(const LangStringConverter &convValue)
{
	const LangString &value = convValue.getValue();

	int valueLen = value.size();
	int current = 0;
	while (current < valueLen)
	{
		int bstart = value.find('{', current);
		if (bstart == std::string::npos) 
		{
			parts_.push_back(LangString(value, current));
			break;
		}

		int bend = value.find('}', bstart);
		if (bend == std::string::npos)
		{
			parts_.push_back(LangString(value, current));
			break;
		}

		static LangString numbers = LANG_STRING("0123456789");
		int charstart = value.find_first_not_of(numbers, bstart+1);
		if (charstart != bend)
		{
			parts_.push_back(LangString(value, current));
			break;
		}

		LangString position(value, bstart+1, bend - bstart - 1);
		LangString part(value, current, bstart-current);

		std::string positionString = LangStringUtil::convertFromLang(position);
		positions_.push_back(atoi(positionString.c_str()));
		parts_.push_back(part);

		current = bend + 1;
		if (current >= valueLen)
		{
			parts_.push_back(LangString());
		}
	}	

	//parts_[0].append(LANG_STRING("--"));
}

LangString LangParam::getParameterizedString()
{
	if (parts_.size() != 1 || positions_.size() != 0) return LangString();
	return parts_.front();
}

LangString LangParam::getParameterizedString(const LangStringConverter &param1)
{
	if (parts_.size() != 2 || positions_.size() != 1) return LangString();
	LangString result;
	result.append(parts_[0]).append(param1.getValue()).append(parts_[1]);
	return result;
}

LangString LangParam::getParameterizedString(const LangStringConverter &param1, const LangStringConverter &param2)
{
	if (parts_.size() != 3 || positions_.size() != 2) return LangString();
	LangString result;
	result.append(parts_[0]);
	result.append(positions_[0]==0?param1.getValue():param2.getValue());
	result.append(parts_[1]);
	result.append(positions_[1]==0?param1.getValue():param2.getValue());
	result.append(parts_[2]);
	return result;
}

LangString LangParam::getParameterizedString(const LangStringConverter &param1, 
	const LangStringConverter &param2, const LangStringConverter &param3)
{
	if (parts_.size() != 4 || positions_.size() != 3) return LangString();
	LangString result;
	result.append(parts_[0]);
	result.append(positions_[0]==0?param1.getValue():positions_[0]==1?param2.getValue():param3.getValue());
	result.append(parts_[1]);
	result.append(positions_[1]==0?param1.getValue():positions_[1]==1?param2.getValue():param3.getValue());
	result.append(parts_[2]);
	result.append(positions_[2]==0?param1.getValue():positions_[2]==1?param2.getValue():param3.getValue());
	result.append(parts_[3]);
	return result;
}

LangString LangParam::getParameterizedString(const LangStringConverter &param1, 
	const LangStringConverter &param2, const LangStringConverter &param3, const LangStringConverter &param4)
{
	if (parts_.size() != 5 || positions_.size() != 4) return LangString();
	LangString result;
	result.append(parts_[0]);
	result.append(positions_[0]==0?param1.getValue():positions_[0]==1?param2.getValue():positions_[0]==2?param3.getValue():param4.getValue());
	result.append(parts_[1]);
	result.append(positions_[1]==0?param1.getValue():positions_[1]==1?param2.getValue():positions_[1]==2?param3.getValue():param4.getValue());
	result.append(parts_[2]);
	result.append(positions_[2]==0?param1.getValue():positions_[2]==1?param2.getValue():positions_[2]==2?param3.getValue():param4.getValue());
	result.append(parts_[3]);
	result.append(positions_[3]==0?param1.getValue():positions_[3]==1?param2.getValue():positions_[3]==2?param3.getValue():param4.getValue());
	result.append(parts_[4]);
	return result;
}

LangString LangParam::getValue()
{
	LangString result;
	for (unsigned int i=0; i<parts_.size(); i++)
	{
		result.append(parts_[i]);
		if (i + 1 < parts_.size()) 
		{
			result.append(LANG_STRING(S3D::formatStringBuffer("{%i}", i)));
		}
	}
	return result;
}

LangString LangParam::getKey(const LangStringConverter &value,
	const LangStringConverter &param1)
{
	LangParam parameterized(value);
	return parameterized.getParameterizedString(param1);
}

LangString LangParam::getKey(const LangStringConverter &value,
	const LangStringConverter &param1,
	const LangStringConverter &param2)
{
	LangParam parameterized(value);
	return parameterized.getParameterizedString(param1, param2);
}

LangString LangParam::getKey(const LangStringConverter &value,
	const LangStringConverter &param1,
	const LangStringConverter &param2,
	const LangStringConverter &param3)
{
	LangParam parameterized(value);
	return parameterized.getParameterizedString(param1, param2, param3);
}

LangString LangParam::getKey(const LangStringConverter &value,
	const LangStringConverter &param1,
	const LangStringConverter &param2,
	const LangStringConverter &param3,
	const LangStringConverter &param4)
{
	LangParam parameterized(value);
	return parameterized.getParameterizedString(param1, param2, param3, param4);
}
