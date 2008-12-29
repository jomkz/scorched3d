////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#if !defined(AFX_TankAICurrentMove_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
#define AFX_TankAICurrentMove_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_

#include <tankai/TankAICurrentTarget.h>
#include <tankai/TankAICurrentMoveWeapons.h>
#include <common/Vector.h>

class TankAICurrentMove
{
public:
	TankAICurrentMove();
	virtual ~TankAICurrentMove();

	virtual bool parseConfig(XMLNode *node);

	void clear();
	void playMove(Tank *tank, 
		TankAIWeaponSets::WeaponSet *weapons,
		bool useBatteries);

	TankAICurrentTarget &getTargets() { return targets_; }

protected:
	struct ShotRecord
	{
		Vector position;
		float projectileCurrentDistance;
		float sniperCurrentDistance;
	};

	std::map<Tank *, ShotRecord> shotRecords_;
	TankAICurrentTarget targets_;
	float totalDamageBeforeMove_;
	bool useResign_, useFuel_;
	float movementDamage_, movementDamageChance_, movementLife_;
	float movementRandom_, movementCloseness_;
	float groupShotChance_, groupTargetDistance_;
	int groupShotSize_;
	float resignLife_;
	float largeWeaponUseDistance_;
	float sniperUseDistance_;
	float sniperStartDistance_, sniperEndDistance_;
	float sniperMinDecrement_, sniperMaxDecrement_;
	float sniperMovementFactor_;
	float projectileStartDistance_, projectileEndDistance_;
	float projectileMinDecrement_, projectileMaxDecrement_;
	float projectileMovementFactor_;
	float projectileMinDistance_;

	bool shootAtTank(Tank *tank, Tank *targetTank, 
		TankAICurrentMoveWeapons &weapons);
	bool makeProjectileShot(Tank *tank, Tank *targetTank,
		TankAICurrentMoveWeapons &weapons);
	bool makeSniperShot(Tank *tank, Tank *targetTank,
		TankAICurrentMoveWeapons &weapons);
	bool makeLaserSniperShot(Tank *tank, Tank *targetTank, 
		TankAICurrentMoveWeapons &weapons);
	bool makeBurriedShot(Tank *tank, Tank *targetTank, 
		TankAICurrentMoveWeapons &weapons);
	bool makeMoveShot(Tank *tank, 
		TankAIWeaponSets::WeaponSet *weapons,
		std::list<Tank *> &sortedTanks);
	bool makeGroupShot(Tank *tank, 
		TankAIWeaponSets::WeaponSet *weapons,
		std::list<Tank *> &sortedTanks);

	bool inHole(Vector &position);
	Vector lowestHighest(TankAICurrentMoveWeapons &weapons,
		Vector &position, bool highest);

	float getShotDistance(Tank *tank, bool projectile);
	void shotAtTank(Tank *tank, bool projectile, float newDistance);
	bool useAvailableBatteries(Tank *tank);
	void setWeapon(Tank *tank, Accessory *accessory);
	void useBattery(Tank *tank, unsigned int batteryId);
	void resign(Tank *tank);
	void skipMove(Tank *tank);
	void fireShot(Tank *tank);

};

#endif // !defined(AFX_TankAICurrentMove_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
