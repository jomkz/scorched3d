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

#include <engine/ThreadCallback.h>

ThreadCallback::ThreadCallback(bool allowedSync) :
	callbackOutstanding_(false), allowedSync_(allowedSync)
{
}

ThreadCallback::~ThreadCallback()
{
}

void ThreadCallback::addCallback(ThreadCallbackI *callback)
{
	boost::unique_lock<boost::mutex> lock(callbackMutex_);
	callbacks_.push_back(callback);
	callbackOutstanding_ = true;
}

void ThreadCallback::addCallbackSync(ThreadCallbackI &callback)
{
	if (!allowedSync_)
	{
		S3D::dialogExit("ThreadCallback", "Sync not enabled");
	}

	boost::unique_lock<boost::mutex> lock(callbackMutex_);
	callback.sync = new ThreadCallbackISync();
	callbacks_.push_back(&callback);
	callbackOutstanding_ = true;
	while(callback.sync->wait_)
    {
        callback.sync->cond_.wait(lock);
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
		ThreadCallbackISync *sync = callback->sync;
		callback->callbackInvoked();
		if (sync)
		{
			sync->wait_ = false;
			sync->cond_.notify_one();
		}
	}
	callbackOutstanding_ = false;
	callbackMutex_.unlock();
}