#include "GameLogic.h"
#include "GameServerNode.h"
#include "BaseMsg.h"
#include "WorkThreadLogic.h"
CIniFile g_cfg;
MainLogic* MainLogic::m_pThis = nullptr;
MainLogic::MainLogic()
{
	m_lpRedisServer = new CRedisServer();
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
	m_pGameMsgPool = new ThreadPool< CWorkThread >;
	m_pGameMsgPool->AddTaskArgs(4096, 10);
}

MainLogic::~MainLogic()
{

}

void MainLogic::OnTimer(long tmNow)
{
	static time_t tLastTimeRead = time(NULL);
	static int mStatus = 0;
	//һ��ʱ���ȡһ�������ļ�
	if (tmNow - tLastTimeRead >= 3)
	{
		tLastTimeRead = tmNow;
	}
}
void MainLogic::OnNetMessage(IPlayerNode *lpPlayerNode, unsigned short type, void *buffer, long length)
{
	printf("MainLogic::OnNetMessage type %0x\n", type);
	CWorkThread* mthread = m_pGameMsgPool->take();
	if (mthread != NULL)
	{
		__log(_DEBUG, __FUNCTION__, "take thread %s", mthread->GetMsgQueName());
		m_pGameMsgPool->take()->EnQueGameMsg(lpPlayerNode, type, buffer, length);
		
	}
	else
	{
		__log(_ERROR, __FUNCTION__, "take thread error");
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
                 __log(_DEBUG, "MainLogic", "szAddr %s", szAddr);
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
		for (int i = 0; i < 1;i++)
		{
			SendHi();
		}
		
	}
	else
	{
		__log(_ERROR, __FUNCTION__, "connect master failed");
	}
	if (m_lpRedisServer)
	{
		char cRedisIP[20];
		sprintf(cRedisIP, "%s", "10.7.125.26");
		char cRedisSecert[20];
		sprintf(cRedisSecert, "%s", "123456");
		if (!m_lpRedisServer->Start(cRedisIP, 6667, cRedisSecert, 30))
		{
			__log(_ERROR, "main", "redis listen:start failed!");
			return false;
		}
		else
		{
			__log(_DEBUG, "main", "redis listen:start  success!");
		}
		//test
		for (int i = 10080; i < 10090;i++)
		{
			sleep(1);
			//int irand = rand() % 10000;
			int irand = 10000;
			//m_lpRedisServer->PushRedisRequestUserID(i);
			m_lpRedisServer->ZaddPlayerSCore(i, irand, time(NULL));
			
		}
		vector<PlayerInfo> m_vRankList;
		m_lpRedisServer->PrintfRanklist(10, m_vRankList);
		int64_t iRank = 0;
		/*m_lpRedisServer->GetPlayerRank(10080, iRank);
		m_lpRedisServer->UpdatePlayerHighScore(10080, 500, time(NULL));
		m_lpRedisServer->UpdatePlayerHighScore(10080, 9999, time(NULL));
		m_lpRedisServer->GetPlayersAfter(10480, 5);
		m_lpRedisServer->GetPlayersBefore(10480, 5);

		cout<< "score str:"<< m_lpRedisServer->ConvertScoreToStr(100, time(NULL))<<endl;*/
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
		if (NULL == pServerNode->m_lpPrior) //ͷ�ڵ�
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

void MainLogic::SendHi()
{
	SS_SayHi mMsgBack;
	mMsgBack.iSendNum = 0;
	SendData(m_pProducer, S_S_SAYHI_INFO, &mMsgBack, sizeof(mMsgBack));
}
