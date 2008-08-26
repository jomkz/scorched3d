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

#include <lang/LangResource.h>

LangString LangResource::getKey(const std::string &key)
{
	LANG_RESOURCE_VAR(result, key);
	return result;
}

LangString LangResource::getKey(const std::string &key, 
	const std::string &param1)
{
	LANG_RESOURCE_VAR_1(result, key, param1);
	return result;
}

LangString LangResource::getKey(const std::string &key, 
	const std::string &param1, const std::string &param2)
{
	LANG_RESOURCE_VAR_2(result, key, param1, param2);
	return result;
}

LangString LangResource::getKey(const std::string &key, 
	const std::string &param1, const std::string &param2, const std::string &param3)
{
	LANG_RESOURCE_VAR_3(result, key, param1, param2, param3);
	return result;
}

LangString LangResource::getKey(const std::string &key, 
	const std::string &param1, const std::string &param2, const std::string &param3, const std::string &param4)
{
	LANG_RESOURCE_VAR_4(result, key, param1, param2, param3, param4);
	return result;
}

static void testMacro()
{
	LANG_RESOURCE("hmm");

	LANG_RESOURCE_VAR(bob_0, "test");
	bob_0.c_str();

	LANG_RESOURCE_VAR_1(bob_1, "test", "1");
	bob_1.c_str();

	LANG_RESOURCE_VAR_2(bob_2, "test", "1", "2");
	bob_2.c_str();

	LANG_RESOURCE_VAR_3(bob_3, "test", "1", "2", "3");
	bob_3.c_str();

	LANG_RESOURCE_VAR_4(bob_4, "test", "1", "2", "3", "4");
	bob_4.c_str();

	LANG_RESOURCE_CONST_VAR(bob_c_0, "test");
	bob_c_0.c_str();

	LANG_RESOURCE_CONST_VAR_1(bob_c_1, "test", "1");
	bob_c_1.c_str();

	LANG_RESOURCE_CONST_VAR_2(bob_c_2, "test", "1", "2");
	bob_c_2.c_str();

	LANG_RESOURCE_CONST_VAR_3(bob_c_3, "test", "1", "2", "3");
	bob_c_3.c_str();

	LANG_RESOURCE_CONST_VAR_4(bob_c_4, "test", "1", "2", "3", "4");
	bob_c_4.c_str();
}
