/*
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)
Copyright (C) 2014 Bad Juju Games, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.

Contact us at legal@badjuju.com.
*/

#ifndef DESURA_UPDATETHREAD_H
#define DESURA_UPDATETHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "usercore/UserThreadI.h"
#include "util_thread/BaseThread.h"
#include "BaseUserThread.h"

namespace XML
{
	class gcXMLElement;
}

class UpdateThreadI
{
public:
	virtual void setInfo(const gcRefPtr<UserCore::UserI> &user, const gcRefPtr<WebCore::WebCoreI> &webcore) = 0;
	virtual void doRun() = 0;
	virtual void onStop() = 0;
	virtual bool onMessageReceived(const char* resource, const XML::gcXMLElement &root) = 0;
	virtual ~UpdateThreadI(){}

	Event<bool> isStoppedEvent;

protected:
	bool isStopped()
	{
		bool stopped = false;
		isStoppedEvent(stopped);
		return stopped;
	}
};



namespace UserCore
{
	namespace Thread
	{
		//! Polls for Desura updates
		//!
		class UpdateThread : public BaseUserThread<UserThreadI, ::Thread::BaseThread>
		{
		public:
			UpdateThread(Event<std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>> *onForcePollEvent, bool loadLoginItems);
			~UpdateThread();

		protected:
			void doRun() override;
			void onStop() override;

			virtual bool onMessageReceived(const char* resource, const XML::gcXMLElement &root);

			void isThreadStopped(bool &stopped);

		private:
			UpdateThreadI *m_pBaseTask;
		};

	}
}

#endif //DESURA_UPDATETHREAD_H
