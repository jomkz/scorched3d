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

#include <engine/ThreadCallback.h>

ThreadCallback::ThreadCallback() :
	callbackOutstanding_(false)
{
}

ThreadCallback::~ThreadCallback()
{
}

void ThreadCallback::addCallback(ThreadCallbackI *callback)
{
	callbackMutex_.lock();
	callbacks_.push_back(callback);
	callbackOutstanding_ = true;
	callbackMutex_.unlock();
}

void ThreadCallback::addCallbackSync(ThreadCallbackI *callback)
{
	addCallback(callback);

	// Easy but not the best
	while (callbackOutstanding_)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	}
}

void ThreadCallback::processCallbacks()
{
	if (!callbackOutstanding_) return;

	callbackMutex_.lock();
	while (!callbacks_.empty())
	{
		ThreadCallbackI *callback = callbacks_.back();
		callbacks_.pop_back();
		callback->callbackInvoked();
		delete callback;
	}
	callbackOutstanding_ = false;
	callbackMutex_.unlock();
}