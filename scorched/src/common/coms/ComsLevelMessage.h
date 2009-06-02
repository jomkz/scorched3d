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

#ifndef _comsLevelMessage_h
#define _comsLevelMessage_h

#include <vector>
#include <set>
#include <landscapedef/LandscapeDefinition.h>
#include <landscapemap/DeformLandscape.h>

class ComsLevelMessage : public ComsMessage
{
public:
	static ComsMessageType ComsLevelMessageType;

	ComsLevelMessage();
	virtual ~ComsLevelMessage();

	void createMessage(LandscapeDefinition &hdef);

	// Accessors
	LandscapeDefinition &getGroundMapsDefn();
	std::vector<DeformLandscape::DeformInfo> &getDeformInfos() { return deformInfos_; }
	std::set<unsigned int> &getTargetIds() { return targetIds_; }
	NetBuffer &getNewTargets() { return newTargets_; }
	NetBuffer &getOldTargets() { return oldTargets_; }

	// Inherited from ComsMessage
    virtual bool writeMessage(NetBuffer &buffer);
    virtual bool readMessage(NetBufferReader &reader);

protected:
	LandscapeDefinition hdef_;
	std::set<unsigned int> targetIds_;
	std::vector<DeformLandscape::DeformInfo> deformInfos_;
	NetBuffer newTargets_;
	NetBuffer oldTargets_;

private:
	ComsLevelMessage(const ComsLevelMessage &);
	const ComsLevelMessage & operator=(const ComsLevelMessage &);

};

#endif //_comsLevelMessage_h

