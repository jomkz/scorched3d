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

#if !defined(AFX_TankAIStrings_H__C6D1A1B3_EA15_43B7_9232_A90A5D0CF60B__INCLUDED_)
#define AFX_TankAIStrings_H__C6D1A1B3_EA15_43B7_9232_A90A5D0CF60B__INCLUDED_

#include <common/FileLines.h>
#include <engine/ScorchedContext.h>

class TankAIStrings  
{
public:
	static TankAIStrings *instance();

	const char *getDeathLine(ScorchedContext &context);
	const char *getAttackLine(ScorchedContext &context);
	const char *getPlayerName();
	const char *getAIPlayerName(ScorchedContext &context);

protected:
	static TankAIStrings *instance_;
	FileLines deathLines_;
	FileLines attackLines_;
	FileLines playerNames_;
	FileLines aiPlayerNames_;

private:
	TankAIStrings();
	virtual ~TankAIStrings();

};

#endif // !defined(AFX_TankAIStrings_H__C6D1A1B3_EA15_43B7_9232_A90A5D0CF60B__INCLUDED_)
