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

#if !defined(AFX_TankAICurrentTarget_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
#define AFX_TankAICurrentTarget_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_

#include <list>
#include <map>
#include <XML/XMLEntrySimpleTypes.h>

class Target;
class Tanket;
class TankAICurrentTarget : public XMLEntryGroup
{
public:
	TankAICurrentTarget();
	virtual ~TankAICurrentTarget();

	void clear();
	void getTargets(Tanket *tanket, std::list<Tanket *> &targets);

	void gaveDamage(Tanket *tanket, float damage);
	void tookDamage(Tanket *tanket, float damage);
	void shotAt(Tanket *tanket);

	float getTotalDamageTaken();

protected:
	std::map<Tanket *, float> damageGiven_;
	std::map<Tanket *, float> damageTaken_;
	std::map<Tanket *, float> shotAt_;

	XMLEntryFixed health_;
	XMLEntryFixed random_;
	XMLEntryFixed score_;
	XMLEntryFixed damagedone_;
	XMLEntryFixed damagetaken_;
	XMLEntryFixed shield_;
	XMLEntryFixed repeat_;
	XMLEntryFixed player_;
	XMLEntryFixed distance_;

	float rankPlayer(std::list<Tanket *> &players, Tanket *player);
	float rankPlayer(std::multimap<float, Tanket *> &players, Tanket *player);
};

#endif // !defined(AFX_TankAICurrentTarget_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
