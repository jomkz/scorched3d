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

#include <lang/ResourceBundleEntryImpl.h>
#include <common/DefinesAssert.h>

ResourceBundleEntryImpl::ResourceBundleEntryImpl(const std::string &key) :
	key_(key)
{

}

ResourceBundleEntryImpl::ResourceBundleEntryImpl(const std::string &key, 
	const std::string &value) :
	key_(key)
{
	int valueLen = value.size();
	int current = 0;
	while (current < valueLen)
	{
		int bstart = value.find("{", current);
		if (bstart == std::string::npos) 
		{
			parts_.push_back(LANG_STRING(std::string(value, current)));
			break;
		}

		int bend = value.find("}", bstart);
		if (bend == std::string::npos)
		{
			parts_.push_back(LANG_STRING(std::string(value, current)));
			break;
		}

		int charstart = value.find_first_not_of("0123456789", bstart+1);
		if (charstart != bend)
		{
			parts_.push_back(LANG_STRING(std::string(value, current)));
			break;
		}

		std::string position(value, bstart+1, bend - bstart - 1);
		std::string part(value, current, bstart-current);

		positions_.push_back(atoi(position.c_str()));
		parts_.push_back(LANG_STRING(part));

		current = bend + 1;
		if (current >= valueLen)
		{
			parts_.push_back(LANG_STRING(""));
		}
	}	
}

LangString ResourceBundleEntryImpl::getString()
{
	if (parts_.size() != 1 || positions_.size() != 0) return LangString();
	return parts_.front();
}

LangString ResourceBundleEntryImpl::getString(const std::string &param1)
{
	if (parts_.size() != 2 || positions_.size() != 1) return LangString();
	LangString result;
	result.append(parts_[0]);
	LangStringUtil::append(result, param1);
	result.append(parts_[1]);
	return result;
}

LangString ResourceBundleEntryImpl::getString(const std::string &param1, const std::string &param2)
{
	if (parts_.size() != 3 || positions_.size() != 2) return LangString();
	LangString result;
	result.append(parts_[0]);
	LangStringUtil::append(result, positions_[0]==0?param1:param2);
	result.append(parts_[1]);
	LangStringUtil::append(result, positions_[1]==0?param1:param2);
	result.append(parts_[2]);
	return result;
}

LangString ResourceBundleEntryImpl::getString(const std::string &param1, 
	const std::string &param2, const std::string &param3)
{
	if (parts_.size() != 4 || positions_.size() != 3) return LangString();
	LangString result;
	result.append(parts_[0]);
	LangStringUtil::append(result, positions_[0]==0?param1:positions_[0]==1?param2:param3);
	result.append(parts_[1]);
	LangStringUtil::append(result, positions_[1]==0?param1:positions_[1]==1?param2:param3);
	result.append(parts_[2]);
	LangStringUtil::append(result, positions_[2]==0?param1:positions_[2]==1?param2:param3);
	result.append(parts_[3]);
	return result;
}

LangString ResourceBundleEntryImpl::getString(const std::string &param1, 
	const std::string &param2, const std::string &param3, const std::string &param4)
{
	if (parts_.size() != 5 || positions_.size() != 4) return LangString();
	LangString result;
	result.append(parts_[0]);
	LangStringUtil::append(result, positions_[0]==0?param1:positions_[0]==1?param2:positions_[0]==2?param3:param4);
	result.append(parts_[1]);
	LangStringUtil::append(result, positions_[1]==0?param1:positions_[1]==1?param2:positions_[1]==2?param3:param4);
	result.append(parts_[2]);
	LangStringUtil::append(result, positions_[2]==0?param1:positions_[2]==1?param2:positions_[2]==2?param3:param4);
	result.append(parts_[3]);
	LangStringUtil::append(result, positions_[3]==0?param1:positions_[3]==1?param2:positions_[3]==2?param3:param4);
	result.append(parts_[5]);
	return result;
}

