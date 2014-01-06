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
#include "gcDDEManager.h"
#include <ddeml.h>

#define DDERETURN HDDEDATA
static gcDDEManager g_DDEManager;

gcDDEManager* GetDDEManager()
{
	return &g_DDEManager;
}


HDDEDATA CALLBACK _DDECallback(WORD wType, WORD wFmt, HCONV hConv, HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD lData1, DWORD lData2)
{
	return GetDDEManager()->processCallBack(wType, wFmt, hConv, hsz1, hsz2, hData, lData1, lData2);
}


gcDDEManager::gcDDEManager()
{
	m_pCurConnecting = nullptr;
	m_hDDEIdInst = 0;
	m_bDDEInit = false;
}

gcDDEManager::~gcDDEManager()
{
	cleanUp();
}

void gcDDEManager::init()
{
    if ( !m_bDDEInit )
    {
        UINT rc = DdeInitialize(&m_hDDEIdInst, (PFNCALLBACK)_DDECallback, APPCLASS_STANDARD, 0L);
        if ( rc != DMLERR_NO_ERROR )
        {
            printf("Failed to initialize DDE [%d]\n", rc);
        }
        else
        {
            m_bDDEInit = true;
        }
    }
}

void gcDDEManager::cleanUp()
{
	safe_delete(m_vClient);
	safe_delete(m_vServer);

	for (std::set<HSZ>::iterator x=m_vAtomList.begin(); x!=m_vAtomList.end(); ++x)
	{
		DdeFreeStringHandle(m_hDDEIdInst, *x);
	}

	m_vAtomList.clear();

    if ( m_hDDEIdInst != 0 )
    {
        DdeUninitialize(m_hDDEIdInst);
        m_hDDEIdInst = 0;
		m_bDDEInit = false;
    }
}


gcDDEServer* gcDDEManager::findServer(const char* name)
{
	if (!name)
		return nullptr;

	for (size_t x=0; x<m_vServer.size(); x++)
	{
		if (!m_vServer[x])
			continue;

		if (m_vServer[x]->getName() && strcmp(name, m_vServer[x]->getName())==0)
			return m_vServer[x];
	}

	return nullptr;
}

void gcDDEManager::removeServer(gcDDEServer* server)
{
	if (!server)
		return;

	for (size_t x=0; x<m_vServer.size(); x++)
	{
		if (!m_vServer[x])
			continue;

		if (m_vServer[x] == server)
		{
			m_vServer[x] = nullptr;
			break;
		}
	}
}

void gcDDEManager::removeClient(gcDDEClient* client)
{
	if (!client)
		return;

	for (size_t x=0; x<m_vClient.size(); x++)
	{
		if (!m_vClient[x])
			continue;

		if (m_vClient[x] == client)
		{
			m_vClient[x] = nullptr;
			break;
		}
	}
}

void gcDDEManager::addServer(gcDDEServer* server)
{
	if (!server)
		return;

	m_vServer.push_back(server);
}

void gcDDEManager::addClient(gcDDEClient* client)
{
	if (!client)
		return;

	m_vClient.push_back(client);
}

gcDDEConnection* gcDDEManager::findConnection(HCONV conv)
{
	for (size_t x=0; x<m_vServer.size(); x++)
	{
		if (!m_vServer[x])
			continue;

		gcDDEConnection* con = m_vServer[x]->findConnection(conv);

		if (con)
			return con;
	}

	for (size_t x=0; x<m_vClient.size(); x++)
	{
		if (!m_vClient[x])
			continue;

		gcDDEConnection* con = m_vClient[x]->findConnection(conv);

		if (con)
			return con;
	}

	return nullptr;
}


HSZ gcDDEManager::atomFromString(const char* string)
{
	HSZ hsz = DdeCreateStringHandleA(m_hDDEIdInst, string, CP_WINANSI);
	m_vAtomList.insert(hsz);

	return hsz;
}

void gcDDEManager::stringFromAtom(HSZ atom, char* str, size_t size)
{
	char buff[256];
	DdeQueryStringA(m_hDDEIdInst, atom, buff, 256, CP_WINANSI);
	Safe::snprintf(str, size, "%s", buff);
}

HDDEDATA gcDDEManager::processCallBack(WORD wType, WORD wFmt, HCONV hConv, HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD lData1, DWORD lData2)
{
	//printf("\tProcessing msg: %d\n", wType);

    switch (wType)
    {
        case XTYP_CONNECT:
            {
				char szTopic[255];
				stringFromAtom(hsz1, szTopic, 255);

				char szServer[255];
				stringFromAtom(hsz2, szServer, 255);

                gcDDEServer *server = findServer(szServer);
                if (server)
					return server->acceptConnection(szTopic, hConv);

                break;
            }

        case XTYP_CONNECT_CONFIRM:
            {
                if (m_pCurConnecting)
                {
                    m_pCurConnecting->setConv(hConv);
                    m_pCurConnecting = nullptr;
                    return (DDERETURN)(DWORD)true;
                }
                break;
            }

        case XTYP_DISCONNECT:
            {
                gcDDEConnection *connection = findConnection(hConv);
                if (connection)
                    return connection->disconnect();

                break;
            }

        case XTYP_POKE:
            {
                gcDDEConnection *connection = findConnection(hConv);

                if (connection)
					return (DDERETURN)connection->handlePoke(hsz2, hData);
                else
                    return (DDERETURN)DDE_FNOTPROCESSED;
            }

        case XTYP_ADVSTART:
        case XTYP_ADVSTOP:
        case XTYP_EXECUTE:
        case XTYP_REQUEST:
        case XTYP_ADVREQ:
        case XTYP_ADVDATA:
			printf("gcDDE: This method not implemented! [%d]", wType);
    }

    return (DDERETURN)0;
}