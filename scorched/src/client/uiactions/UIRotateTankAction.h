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

#if !defined(__INCLUDE_UIRotateTankActionh_INCLUDE__)
#define __INCLUDE_UIRotateTankActionh_INCLUDE__

#include <client/ClientUISync.h>
#include <common/fixed.h>

// Called by other tanks not from the current client 
// to show this client their rotation
class UIRotateTankAction : public ClientUISyncAction
{
public:
	UIRotateTankAction(unsigned int playerId, fixed rotation, fixed elevation);
	virtual ~UIRotateTankAction();

	// ClientUISyncAction
	virtual void performUIAction();
protected:
	unsigned int playerId_;
	fixed rotation_, elevation_;
};

#endif // __INCLUDE_UIRotateTankActionh_INCLUDE__
