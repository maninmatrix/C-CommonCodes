#ifndef _GAME_LOGIC_H_
#define _GAME_LOGIC_H_
#include <iostream>
#include "Game/GameProvider.h"
#include "GameServerNode.h"
#include "ReconnectService.h"
using namespace std;
class MainLogic : public GameProvider
{
public:
	MainLogic();
	~MainLogic();
public:
	virtual long Start();
	virtual IPlayerNode* OnCreatePlayer();
	virtual void OnTimer(long tmNow);
	virtual void OnNetMessage(IPlayerNode *lpPlayerNode, unsigned short type, void *buffer, long length);
	virtual void OnClosed(IPlayerNode *lpPlayerNode, bool bClosed);
	static MainLogic& Instance();
	GameServerNode* GetServerList();
	long ConnectServer(const char *lpszServerName, IPlayerNode *&lpPlayerNode);
	long ConnectMasterServer();
public:
	void HandleSayHiReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
	void OnServerRegisterReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
public:
	IPlayerNode *m_lpMyServer;
	GameServerNode *m_lpServerNode;
	static MainLogic* m_pThis;
	ReconnectService m_Reconnect;
	IPlayerNode * m_pProducer;
};
#endif