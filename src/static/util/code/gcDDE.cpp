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
#include "util/gcDDE.h"

#include "gcDDEManager.h"

// default timeout for DDE operations (5sec)
#define DDE_TIMEOUT     5000

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

gcDDEServer::gcDDEServer()
{
	GetDDEManager()->init();
	GetDDEManager()->addServer(this);
	m_szName = nullptr;
}

gcDDEServer::~gcDDEServer()
{
	GetDDEManager()->removeServer(this);
	safe_delete(m_szName);
}

bool gcDDEServer::create(const char* name)
{
    Safe::strcpy(&m_szName, name, 255);
    HSZ hsz = GetDDEManager()->atomFromString(m_szName);

    if ( !hsz )
        return false;

    bool success = (DdeNameService(GetDDEManager()->getDDEInst(), hsz, (HSZ) nullptr, DNS_REGISTER) != nullptr);

    if (!success)
    {
        printf("Failed to register DDE server '%s'\n", m_szName);
    }

    return success;
}

HDDEDATA gcDDEServer::acceptConnection(const char* topic, HCONV conv)
{
	//printf("Got a connection request on topic: %s\n", topic);

	gcDDEConnection *connection = onAcceptConnection(topic);

	if (connection)
	{
		connection->setInfo( nullptr, topic, this);
		m_vConnections.push_back(connection);

		GetDDEManager()->m_pCurConnecting = connection;
		return (HDDEDATA)(DWORD)true;
	}

	return (HDDEDATA)(DWORD)false;
}

gcDDEConnection *gcDDEServer::onAcceptConnection(const char* topic)
{
    return new gcDDEConnection();
}

void gcDDEServer::removeConnection(gcDDEConnection *connection)
{
	if (!connection)
		return;

	for (size_t x=0; x<m_vConnections.size(); x++)
	{
		if (m_vConnections[x] == connection)
		{
			m_vConnections[x] = nullptr;
			break;
		}
	}
}

gcDDEConnection* gcDDEServer::findConnection(HCONV conv)
{
	for (size_t x=0; x<m_vConnections.size(); x++)
	{
		if (m_vConnections[x] && m_vConnections[x]->getConv() == conv)
			return m_vConnections[x];
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////


gcDDEClient::gcDDEClient()
{
	GetDDEManager()->init();
	GetDDEManager()->addClient(this);
}

gcDDEClient::~gcDDEClient()
{
	GetDDEManager()->removeClient(this);
}

gcDDEConnection *gcDDEClient::makeConnection(const char* server, const char* topic)
{
    HSZ hszServer = GetDDEManager()->atomFromString(server);

    if ( !hszServer )
        return nullptr;

    HSZ hszTopic = GetDDEManager()->atomFromString(topic);

    if ( !hszTopic )
        return nullptr;

    HCONV hConv = ::DdeConnect(GetDDEManager()->getDDEInst(), hszServer, hszTopic, (PCONVCONTEXT) nullptr);

    if ( !hConv )
    {
        printf("Failed to create connection to server '%s' on topic '%s'\n", server, topic );
    }
    else
    {
        gcDDEConnection *connection = (gcDDEConnection*) onMakeConnection();
        if (connection)
        {	
			connection->setInfo(hConv, topic, this);
            m_vConnections.push_back(connection);
            return connection;
        }
    }

    return nullptr;
}

gcDDEConnection *gcDDEClient::onMakeConnection()
{
    return new gcDDEConnection;
}

void gcDDEClient::removeConnection(gcDDEConnection *connection)
{
	if (!connection)
		return;

	for (size_t x=0; x<m_vConnections.size(); x++)
	{
		if (m_vConnections[x] == connection)
		{
			m_vConnections[x] = nullptr;
			break;
		}
	}
}

gcDDEConnection* gcDDEClient::findConnection(HCONV conv)
{
	for (size_t x=0; x<m_vConnections.size(); x++)
	{
		if (m_vConnections[x] && m_vConnections[x]->getConv() == conv)
			return m_vConnections[x];
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

gcDDEConnection::gcDDEConnection()
{
	m_pClient = nullptr;
	m_pServer = nullptr;

	m_hConv = nullptr;
	m_szTopic = nullptr;

	m_bConnected = false;
}	

gcDDEConnection::~gcDDEConnection()
{
	safe_delete(m_szTopic);

	if (m_pServer)
		m_pServer->removeConnection(this);

	if (m_pClient)
		m_pClient->removeConnection(this);
}

void gcDDEConnection::setInfo(HCONV con, const char* topic, gcDDEClient *client)
{
	m_hConv = con;
	m_pClient = client;
	Safe::strcpy(&m_szTopic, topic, 255);
}

void gcDDEConnection::setInfo(HCONV con, const char* topic, gcDDEServer *server)
{
	m_hConv = con;
	m_pServer = server;
	Safe::strcpy(&m_szTopic, topic, 255);
}

HDDEDATA gcDDEConnection::disconnect()
{
	m_bConnected = false;

    if (onDisconnect())
	{
		if (DdeDisconnect(m_hConv) == 0)
			 printf("Failed to disconnect from DDE server gracefully");

		delete this;
		return (HDDEDATA)(DWORD)true;
	}

	return (HDDEDATA)(DWORD)false;
}

bool gcDDEConnection::onDisconnect()
{	
	return true;
}

HDDEDATA gcDDEConnection::handlePoke(HSZ hszItem, HDDEDATA hData)
{
	char item_name[255];
	GetDDEManager()->stringFromAtom(hszItem, item_name, 255);

	if (!hData)
	{
		onPoke(item_name, nullptr, 0);
		return (HDDEDATA)DDE_FACK;
	}

	DWORD len = DdeGetData(hData, nullptr, 0, 0);

	if (len == 0)
	{
		onPoke(item_name, nullptr, 0);
		return (HDDEDATA)DDE_FACK;
	}

	char *data = new char[len+1];

	DdeGetData(hData, (LPBYTE)data, len, 0);
	data[len] = '\0';

	DdeFreeDataHandle(hData);

	if (len > 0)
	{
		int x = len-1;
		while ( x+1 )
		{
			if (data[x] == '\r' || data[x] == '\n')
			{
				data[x] = '\0'; 
				len--;
			}
			else
			{
				break;
			}

			x--;
		}
	}

	char f = data[0];
	char l = data[len-1];

	//if it has " " around it remove them
	if (f == 34 && l == 34)
	{
		for (size_t x=0; x<(len-2); x++)
		{
			data[x]=data[x+1];
		}

		data[len-2]='\0';
	}

	size_t size = strlen((char*)data);
	onPoke(item_name, (char*)data, size);

	safe_delete(data);
	return (HDDEDATA)DDE_FACK;
}


void gcDDEConnection::onPoke(const char* item, const char* data, size_t len)
{

}

bool gcDDEConnection::poke(const char* item, const char* data)
{
    DWORD result;

    HSZ item_atom = GetDDEManager()->atomFromString(item);

	char fdata[255];

	Safe::snprintf(fdata, 255, "%s\r\n\0", data);
	size_t size = strlen(fdata);

	HDDEDATA res = DdeClientTransaction((LPBYTE)fdata, size, m_hConv, item_atom, CF_TEXT, XTYP_POKE, DDE_TIMEOUT, &result);
	bool ok =  res?true:false;
    if ( !ok )
    {
        printf("DDE poke request failed");
    }

    return ok;
}

