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

#if !defined(AFX_ACCESSORYCREATECONTEXT_H__21765D5B_DB45_4275_AB63_BAD1E84C1790__INCLUDED_)
#define AFX_ACCESSORYCREATECONTEXT_H__21765D5B_DB45_4275_AB63_BAD1E84C1790__INCLUDED_

#include <map>
#include <string>

class OptionsScorched;
class AccessoryStore;
class WeaponLabel;
class AccessoryCreateContext  
{
public:
	AccessoryCreateContext(OptionsScorched &optionsGame, AccessoryStore *store);
	virtual ~AccessoryCreateContext();

	OptionsScorched &getOptionsGame() { return optionsGame_; }
	AccessoryStore *getAccessoryStore() { return store_; }
	WeaponLabel *getLabel(const char *label);
	void addLabel(const char *label, WeaponLabel *weapon);
	void removeLabel(const char *label);

protected:
	OptionsScorched &optionsGame_;
	AccessoryStore *store_;
	std::map<std::string, WeaponLabel*> labels_;

private:
	AccessoryCreateContext(const AccessoryCreateContext&);
	AccessoryCreateContext &operator=(const AccessoryCreateContext &other);
};

#endif // !defined(AFX_ACCESSORYCREATECONTEXT_H__21765D5B_DB45_4275_AB63_BAD1E84C1790__INCLUDED_)
