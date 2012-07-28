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

#include <ui/RocketEventListener.h>
#include <ui/RocketEventProcessor.h>
#include <stdio.h>

RocketEventListener::RocketEventListener(const Rocket::Core::String& value) : value_(value)
{
}

RocketEventListener::~RocketEventListener()
{
}

// Sends the event value through to Invader's event processing system.
void RocketEventListener::ProcessEvent(Rocket::Core::Event& evt)
{
	RocketEventProcessor::instance().ProcessEvent(value_, evt);
}

// Destroys the event.
void RocketEventListener::OnDetach(Rocket::Core::Element* ROCKET_UNUSED(element))
{
	delete this;
}