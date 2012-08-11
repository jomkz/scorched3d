////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#if !defined(__INCLUDE_ClientHandlersh_INCLUDE__)
#define __INCLUDE_ClientHandlersh_INCLUDE__

#include <client/ClientConnectionRejectHandler.h>
#include <client/ClientLinesHandler.h>
#include <client/ClientOperationHandler.h>
#include <client/ClientGameStoppedHandler.h>
#include <client/ClientLoadLevelHandler.h>
#include <client/ClientAdminResultHandler.h>

class ClientHandlers  
{
public:
	ClientHandlers(ComsMessageHandler &comsMessageHandler);
	virtual ~ClientHandlers();

	ClientLoadLevelHandler &getClientLoadLevelHandler() { return loadLevelHandler_; }
	ClientAdminResultHandler &getClientAdminResultHandler() { return adminResultHandler_; }

protected:
	ClientConnectionRejectHandler connectionRejectHandler_;
	ClientLinesHandler linesHandler_;
	ClientOperationHandler operationHandler_;
	ClientGameStoppedHandler gameStoppedHandler_;
	ClientLoadLevelHandler loadLevelHandler_;
	ClientAdminResultHandler adminResultHandler_;
};

#endif
