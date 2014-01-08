/*
Desura is the leading indie game distribution platform
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)

$LicenseInfo:firstyear=2014&license=lgpl$
Copyright (C) 2014, Linden Research, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation;
version 2.1 of the License only.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, see <http://www.gnu.org/licenses/>
or write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
$/LicenseInfo$
*/

#include "Common.h"
#include "thread/UpdateThread.h"
#include "mcfcore/ProgressInfo.h"

namespace MCFCore
{
namespace Thread
{

UpdateProgThread::UpdateProgThread(uint16 count, uint64 totSize) 
	: BaseThread( "Update Progress Thread" )
	, m_pCount(count)
	, m_uiTotalSize(totSize)
{
}

UpdateProgThread::~UpdateProgThread()
{
	stop();
}

void UpdateProgThread::setTotal(uint64 totSize)
{
	m_uiTotalSize = totSize;
}

void UpdateProgThread::setDone(uint64 done)
{
	m_uiDoneSize = done;
}

void UpdateProgThread::reportProg(uint32 id, uint64 ammount)
{
	if (id >= m_vProgInfo.size())
		return;

	std::lock_guard<std::mutex> guard(m_pProgMutex);

	m_vProgInfo[id] = ammount;
	m_tLastUpdateTime = gcTime();
}

void UpdateProgThread::stopThread(uint32 id)
{
}

void UpdateProgThread::run()
{
	m_tStartTime = gcTime();
	m_tLastUpdateTime = gcTime();

	for (uint8 x=0; x<m_pCount; x++)
		m_vProgInfo.push_back(0);

	while (!isStopped())
	{
		doPause();
		gcSleep(500);
		calcResults();
	}
}

void UpdateProgThread::calcResults()
{
	if (isPaused())
		return;

	uint64 done = 0;
	gcTime curTime;

	auto elasped = curTime - m_tStartTime;

	//only go fowards if total time elasped is greater than a second
	if (elasped.seconds() == 0)
		return;

	{
		std::lock_guard<std::mutex> guard(m_pProgMutex);

		for (size_t x=0; x<m_vProgInfo.size(); x++)
			done += m_vProgInfo[x];
	}
	

	if (done == 0)
		return;

	MCFCore::Misc::ProgressInfo temp = MCFCore::Misc::ProgressInfo();

	temp.doneAmmount = done+m_uiDoneSize;
	temp.totalAmmount = m_uiTotalSize;
	temp.percent = (uint8)(((done+m_uiDoneSize)*100)/m_uiTotalSize);

	auto diff = curTime - m_tLastUpdateTime;

	if (temp.doneAmmount >= temp.totalAmmount)
	{
		temp.doneAmmount = temp.totalAmmount;
	}
	else if (diff.seconds() < 5)
	{
		auto total = curTime - m_tStartTime;
		total -= m_tTotPauseTime;

		double avgRate	= done / (double)total.seconds();
		uint64 pred		= (uint64)((m_uiTotalSize - done - m_uiDoneSize) / avgRate);
		
		auto predTime = gcDuration(std::chrono::seconds((long)pred));

		temp.hour	= (uint8)predTime.hours();
		temp.min	= (uint8)predTime.minutes();
		temp.rate	= (uint32)avgRate;
	}
	else
	{
		temp.hour	= (uint8)-1;
		temp.min	= (uint8)-1;
	}

	onProgUpdateEvent(temp);
}

void UpdateProgThread::onPause()
{
	m_tPauseStartTime = gcTime();
}

void UpdateProgThread::onUnpause()
{
	m_tTotPauseTime += gcTime() - m_tPauseStartTime;
}

void UpdateProgThread::onStop()
{
}


}
}
