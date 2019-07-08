#ifndef _GAME_LOGIC_PALYER_H_
#define _GAME_LOGIC_PALYER_H_
#include "Game/PlayerNode.h"
static unsigned long _PLAYERNODE_ID = 0;
class GameServerNode :public PlayerNode
{
public:
	operator unsigned long() const
	{
		return m_ulPlayerNodeId;
	}

	void operator = (unsigned long id)
	{
		m_ulPlayerNodeId = id;
	}

	virtual GameServerNode* operator->()
	{
		return this;
	}

protected:
	virtual void Initialize()
	{
		PlayerNode::Initialize();

		if (0 == ++_PLAYERNODE_ID)
		{
			++_PLAYERNODE_ID;
		}
		m_ulPlayerNodeId = _PLAYERNODE_ID;

		m_lpNext = NULL;
		m_lpPrior = NULL;

		m_iGameID = 0;
		m_iServerD = 0;
	}


public:
	GameServerNode *m_lpNext;
	GameServerNode *m_lpPrior;

	int m_iGameID;
	int m_iServerD;
	int iRoomLevel;

private:
	unsigned long m_ulPlayerNodeId;
};

#endif