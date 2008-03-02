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

#if !defined(AFX_TANK_H__52F37177_46EA_49C8_9B58_E6C57ABDB78A__INCLUDED_)
#define AFX_TANK_H__52F37177_46EA_49C8_9B58_E6C57ABDB78A__INCLUDED_

#include <target/Target.h>

class TankAI;
class TankAccessories;
class TankScore;
class TankState;
class TankPosition;
class TankModelContainer;
class TankMod;
class TankAvatar;
class TankCamera;
class Tank : public Target
{
public:
	// Constructor for tank
	// The name and color are copied
	Tank(ScorchedContext &context, 
		unsigned int playerId, 
		unsigned int destinationId,
		const char *name, 
		Vector &color, 
		const char *modelName,
		const char *typeName);
	virtual ~Tank();

	// Called when the state changes
	virtual void newMatch();
	virtual void newGame();
	virtual void clientNewGame();
	void rezTank();

	// Serialize the tank
	virtual bool writeMessage(NetBuffer &buffer, bool writeAccessories);
	virtual bool readMessage(NetBufferReader &reader);

	// The base attributes of the tank
	virtual bool getAlive();
	virtual bool isTarget() { return false; }
	unsigned int getDestinationId() { return destinationId_; }
	void setDestinationId(unsigned int id) { destinationId_ = id; }
	unsigned int getTeam() { return team_; }
	void setTeam(unsigned int team) { team_ = team; }
	unsigned int getKeepAlive() { return keepAlive_; }
	void setKeepAlive(unsigned int ka) { keepAlive_ = ka; }
	const char *getUniqueId() { return uniqueId_.c_str(); }
	void setUniqueId(const char *id) { uniqueId_ = id; }
	const char *getSUI() { return SUI_.c_str(); }
	void setSUI(const char *SecID) { SUI_ = SecID; }
	const char *getHostDesc() { return hostDesc_.c_str(); }
	void setHostDesc(const char *id) { hostDesc_ = id; }
	TankAI *getTankAI() { return tankAI_; }
	void setTankAI(TankAI *ai);
	Vector &getColor();
	void setColor(Vector &color) { color_ = color; }
	unsigned int getIpAddress() { return ipAddress_; }
	void setIpAddress(unsigned int ipAddress) { ipAddress_ = ipAddress; }
	virtual Weapon *getDeathAction();

	// Other attributes
	TankAccessories& getAccessories() { return *accessories_; }
	TankScore &getScore() { return *score_; }
	TankPosition &getPosition() { return *position_; }
	TankState &getState() { return *state_; }
	TankMod &getMod() { return *mod_; }
	TankAvatar &getAvatar() { return *avatar_; }
	TankCamera &getCamera() { return *camera_; }
	TankModelContainer &getModelContainer() { return *modelContainer_; }

protected:
	ScorchedContext &context_;
	TankModelContainer *modelContainer_;
	TankAccessories *accessories_;
	TankScore *score_;
	TankPosition *position_;
	TankState *state_;
	TankMod *mod_;
	TankAvatar *avatar_;
	TankCamera *camera_;
	TankAI *tankAI_;
	Vector color_;
	std::string uniqueId_;
	std::string SUI_;
	std::string hostDesc_;
	unsigned int team_;
	unsigned int playerId_;
	unsigned int destinationId_;
	unsigned int ipAddress_;
	unsigned int keepAlive_;

};

#endif // !defined(AFX_TANK_H__52F37177_46EA_49C8_9B58_E6C57ABDB78A__INCLUDED_)
