#include "GameLogic.h"
#include "GameServerNode.h"
#include "BaseMsg.h"
CIniFile g_cfg;
MainLogic* MainLogic::m_pThis = nullptr;
MainLogic::MainLogic()
{
	m_lpMyServer = nullptr;
	m_lpServerNode = nullptr;
	g_cfg.InitFile(CONFIG_FILE);

	int iInterval = g_cfg.GetValueInt("network", "interval", 20);
	int iTimeout = g_cfg.GetValueInt("network", "timeout", 120);
	int iMaxSendCount = g_cfg.GetValueInt("network", "MAX_SEND_COUNT", 500);
	int iMaxRecvCount = g_cfg.GetValueInt("network", "MAX_RECV_COUNT", 500);
	
	__log(_DEBUG, __FUNCTION__, "iInterval = [%d],iTimeout = [%d],iMaxSendCount = [%d],iMaxRecvCount = [%d]\n", iInterval, iTimeout, iMaxSendCount, iMaxRecvCount);

	if (true == GameProvider::Initialize(iMaxRecvCount, iMaxSendCount, iInterval, iTimeout))
	{
		__log(_DEBUG, "MainLogic", "interval[%d], timeout[%d]", iInterval, iTimeout);
	}
	else
	{
		__log(_ERROR, "MainLogic", "GameProvider::Initialize() error !");
	}
	
}

MainLogic::~MainLogic()
{

}

void MainLogic::OnTimer(long tmNow)
{
	static time_t tLastTimeRead = time(NULL);
	static int mStatus = 0;
	//一定时间读取一次配置文件
	if (tmNow - tLastTimeRead >= 3)
	{
		tLastTimeRead = tmNow;
	}
}
void MainLogic::OnNetMessage(IPlayerNode *lpPlayerNode, unsigned short type, void *buffer, long length)
{
	switch (type)
	{
		printf("type %0x\n", type);
		case GAME_SERVER_ID_NOTICE:
		{								
			OnServerRegisterReq(lpPlayerNode, buffer, length);
			break;
		}
		case S_S_SAYHI_INFO:
		{
			HandleSayHiReq(lpPlayerNode, buffer, length);
			break;
		}
		default:
			break;
	}
}

long MainLogic::Start()
{
	long lResult = -1;
	char szBuffer[32];
	
	if (m_Reconnect.Start())
	{
	printf("m_Reconnect.Start()\n");
	}
	else
	{
	printf("m_Reconnect.Start() failed\n");
	}
	if (0 != g_cfg.GetValueStr("listen", "addr", szBuffer, sizeof(szBuffer)))
	{
		int iMaxConn = g_cfg.GetValueInt("listen", "maxconnection", 10000);

		char szAddr[32] = { 0 };
		int iPort = 0;
		sscanf(szBuffer, "%[^:]:%d", szAddr, &iPort);

		m_lpMyServer = GameProvider::Create(true, szAddr, iPort, 0, iMaxConn);

		if (NULL != m_lpMyServer)
		{
			if (true == GameProvider::RecvData(m_lpMyServer))
			{
				__log(_DEBUG, "LogServer::Start", "listen and start service success, host[%s], port[%d], node = %08x !", szAddr, iPort, m_lpMyServer);

				lResult = 0;
			}
			else
			{
				__log(_ERROR, "LogServer::Start", "RecvData() error !, node = %08x", m_lpMyServer);

				GameProvider::Close(m_lpMyServer);
			}
		}
		else
		{
			__log(_ERROR, "LogServer::Start", "GameProvider::Create() error, host[%s], port[%d], errno[%d], info[%s] !", szAddr, iPort, errno, strerror(errno));
		}
	}
	else
	{
		__log(_ERROR, "LogServer::Start", "g_cfg.GetValueStr() 'addr' error !");
	}
	if (0 == ConnectMasterServer())
	{
		__log(_DEBUG, __FUNCTION__, "connect master success");
		SS_SayHi mMsgBack;
		mMsgBack.iSendNum = 0;
		SendData(m_pProducer,S_S_SAYHI_INFO, &mMsgBack, sizeof(mMsgBack));
	}
	else
	{
		__log(_ERROR, __FUNCTION__, "connect master failed");
	}
	return lResult;
}

IPlayerNode* MainLogic::OnCreatePlayer()
{
	return new GameServerNode;
}

void MainLogic::OnServerRegisterReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	GameServerNode *lpServerNode = (GameServerNode*)lpPlayerNode;
	if (NULL == lpServerNode)
	{
		__log(_ERROR, __FUNCTION__, "node is nil");
		return;
	}
	if (length != sizeof(GameServerIdNotice))
	{
		__log(_ERROR, __FUNCTION__, "length not match! length[%d] != sizeof(GameServerIdNotice)[%d]", length, sizeof(GameServerIdNotice));
		return;
	}

	GameServerIdNotice* lpGameNotice = (GameServerIdNotice*)buffer;
	if (NULL == lpGameNotice)
	{
		__log(_ERROR, __FUNCTION__, "msg is nil!");
		return;
	}
	__log(_DEBUG, __FUNCTION__, "gameid[%d], serverid[%d],roomlevel[%d] connected!",  lpGameNotice->ulServerID);
	lpServerNode->m_iServerD = lpGameNotice->ulServerID;
	if (m_lpServerNode != NULL)
	{
		lpServerNode->m_lpNext = m_lpServerNode;
		m_lpServerNode->m_lpPrior = lpServerNode;
	}
	m_lpServerNode = lpServerNode;
	printf("m_lpServerNode %0x\n", m_lpServerNode);
}

void MainLogic::OnClosed(IPlayerNode *lpPlayerNode, bool bClosed)
{
	if (lpPlayerNode == m_lpMyServer)
	{
		__log(_WARN, __FUNCTION__, "m_lpMyServer server has been closed !, bClosed[%d]", bClosed);
	}
	else
	{
		GameServerNode* pServerNode = (GameServerNode*)lpPlayerNode;
		__log(_WARN, __FUNCTION__, "client has been closed! bClosed[%d], serverid[%d]", bClosed, pServerNode->m_iServerD);
		if (NULL == pServerNode->m_lpPrior) //头节点
		{
			m_lpServerNode = pServerNode->m_lpNext;
			if (NULL != m_lpServerNode)
			{
				m_lpServerNode->m_lpPrior = NULL;
			}
		}
		else
		{
			pServerNode->m_lpPrior->m_lpNext = pServerNode->m_lpNext;
			if (NULL != pServerNode->m_lpNext)
			{
				pServerNode->m_lpNext->m_lpPrior = pServerNode->m_lpPrior;
			}
		}
	}

	*(GameServerNode*)lpPlayerNode = 0;
	GameProvider::OnClosed(lpPlayerNode, bClosed);
}

MainLogic& MainLogic::Instance()
{
	if (m_pThis == nullptr)
	{
		m_pThis = new MainLogic();
	}
	return *m_pThis;
}

GameServerNode* MainLogic::GetServerList()
{
	return m_lpServerNode;
}

long MainLogic::ConnectServer(const char *lpszServerName, IPlayerNode *&lpPlayerNode)
{
	long lResult = -1;
	char szBuffer[32];

	if (0 != g_cfg.GetValueStr("connect", lpszServerName, szBuffer, sizeof(szBuffer))) {
		char szAddr[32] = { 0 };
		int iPort = 0;

		sscanf(szBuffer, "%[^:]:%d", szAddr, &iPort);
		lpPlayerNode = GameProvider::Create(false, szAddr, iPort, 5000);

		if (NULL != lpPlayerNode) {
			if (1) {
				if (true == GameProvider::RecvData(lpPlayerNode)) {
					//__log(_DEBUG, "LobbyServer::ConnectServer", "connect '%s' success, host[%s], port[%d], node = %08x !", lpszServerName, szAddr, iPort, lpPlayerNode);
					lResult = 0;
				}
				else {
					__log(_ERROR, "FishWebServer::ConnectServer", "connect '%s', host[%s], port[%d], RecvData() error !, node = %08x", lpszServerName, szAddr, iPort, lpPlayerNode);
					GameProvider::Close(lpPlayerNode);
				}
			}
			else {
				__log(_ERROR, "FishWebServer::ConnectServer", "connect '%s', host[%s], port[%d], SendData() error ! node = %08x !", lpszServerName, szAddr, iPort, lpPlayerNode);
			}
		}
		else {
			if (NULL == m_lpMyServer) {
				__log(_ERROR, "FishWebServer::ConnectServer", "Create() error, '%s', host[%s], port[%d], errno[%d], info[%s] !", lpszServerName, szAddr, iPort, errno, strerror(errno));
			}
		}
	}
	else {
		__log(_ERROR, "FishWebServer::ConnectServer", "g_cfg.GetValueStr() '%s' error !", lpszServerName);
	}

	return lResult;
}

void MainLogic::HandleSayHiReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	SS_SayHi* mMsg = (SS_SayHi*)buffer;
	S_S_MSG_TYPE mMsgType;
	SS_SayHi mMsgBack;
	cout << "receve num value:" << mMsg->iSendNum << endl;
	mMsgBack.iSendNum = mMsg->iSendNum + 1;
	SendData(lpPlayerNode, S_S_SAYHI_INFO, &mMsgBack, sizeof(mMsgBack));
}

long MainLogic::ConnectMasterServer()
{
	long lResult = ConnectServer("MasterServer", m_pProducer);

	if (lResult == 0) {

		__log(_DEBUG, __FUNCTION__, "connnect MQ Producer server success");
	}
	else
	{
		__log(_ERROR, __FUNCTION__, "connnect MQ Producer server success");
	}

	return lResult;
}
