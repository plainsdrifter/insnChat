//Message.cpp
#include "stdafx.h"
#include "insnChatClient.h"
#include "Message.h"

//Default constructor
Message::Message()
{
	pUnMessage = NULL;
	pEnMessage = NULL;
	pKey = NULL;

	initTable();
}

void Message::initTable()
{
	TCHAR c;
	int id;

	for (int j = 0; j < TABLESIZE; j++)
	{
		for (int i = 0; i < TABLESIZE; i++)
		{
			id = i+j;
			c = (TCHAR)id;
			table[j][i] = c;
		}
	}
}

void Message::encryptMessage()
{
	int across, down;
	int i, j;
	
	for(j = 0; j < (int)wcslen(pEnMessage); j++)
	{
		for(i = 0; i < TABLESIZE; i++)
		{
			if( pUnMessage[j] == table[0][i] )
			{
				across = i;
				break;
			}
		}
		
		for(i = 0; i < TABLESIZE; i++)
		{	
			if( pKey[j] == table[i][0] )
			{
				down = i;
				break;
			}
		}
		
		pEnMessage[j] = table[down][across];
	}	
}

void Message::decryptMessage()
{
	int down, across, i, j;
	
	for(j = 0; j < (int)wcslen(pEnMessage); j++)
	{
		for(i = 0; i < TABLESIZE; i++)
		{
			if( pKey[j] == table[0][i] )
			{
				across = i;
				break;
			}
		}
		
		for(i = 0; i < TABLESIZE; i++)
		{
			if( pEnMessage[j] == table[i][across] )
			{
				down = i;
				break;
			}
		}
		
		pUnMessage[j] = table[down][0];
	}
}

//Returns uncrypted message
const TCHAR* Message::getUnMessage() const
{
	return pUnMessage;
}

//Returns encrypted message
const TCHAR* Message::getEnMessage() const
{
	return pEnMessage;
}

void Message::setMessage(const TCHAR * pMsg)
{
	pUnMessage = new TCHAR[ wcslen(pMsg) + 1 ];
	wcscpy_s(pUnMessage, wcslen(pMsg)+1, pMsg);
	
	pEnMessage = new TCHAR[ wcslen(pMsg) + 1 ];
	wcscpy_s(pEnMessage, wcslen(pMsg)+1, pMsg);
}

void Message::setKey(const TCHAR * pTemp)
{	
	pKey = new TCHAR[ wcslen(pUnMessage) + 1 ];
	wcscpy_s(pKey, wcslen(pUnMessage)+1, pUnMessage);
	
	int i, j;
	
	for(i = 0, j = 0; i < (int)wcslen(pUnMessage), j < (int)wcslen(pUnMessage); i++, j++)
	{	
		if(pTemp[j] && pKey[i])
		{
			pKey[i] = pTemp[j];
		}
		else if(pTemp[j] == '\0')
		{
			j = -1;
			i--;
		}
		else
			break;
	}
}

//Destructor
Message::~Message()
{
	if (pUnMessage != NULL)
		delete[] pUnMessage;
	if (pEnMessage != NULL)
		delete[] pEnMessage;
	if (pKey != NULL)
		delete[] pKey;
}