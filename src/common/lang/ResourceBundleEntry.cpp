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

#include <lang/ResourceBundleEntry.h>
#include <common/DefinesAssert.h>

ResourceBundleEntry::ResourceBundleEntry(const std::string &key) :
	key_(key)
{

}

ResourceBundleEntry::ResourceBundleEntry(const std::string &key, 
	const std::string &value) :
	key_(key), value_(value)
{

}

LangString ResourceBundleEntry::getString()
{
	DIALOG_ASSERT(parts_.size() == 0 && positions_.size() == 0);
	return parts_.front();
}

LangString ResourceBundleEntry::getString(std::string param1)
{
	DIALOG_ASSERT(parts_.size() == 2 && positions_.size() == 1);
	LangString result;
	result.append(parts_[0]);
	LangStringUtil::append(result, param1);
	result.append(parts_[1]);
	return result;
}

LangString ResourceBundleEntry::getString(std::string param1, std::string param2)
{
	DIALOG_ASSERT(parts_.size() == 3 && positions_.size() == 2);
	LangString result;
	result.append(parts_[0]);
	LangStringUtil::append(result, positions_[0]==0?param1:param2);
	result.append(parts_[1]);
	LangStringUtil::append(result, positions_[1]==0?param1:param2);
	result.append(parts_[2]);
	return result;
}

LangString ResourceBundleEntry::getString(std::string param1, std::string param2, std::string param3)
{
	DIALOG_ASSERT(parts_.size() == 4 && positions_.size() == 3);
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
