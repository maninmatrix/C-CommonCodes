#include "WorkThreadLogic.h"
#include "GameLogic.h"
#include "BaseMsg.h"
#define  GAME_SERVER_ID_NOTICE  0xFEFE
#define S_S_SAYHI_INFO  0x1901
CWorkThread::CWorkThread(int iMsgLen, int iQueueSize)
{
	m_lpMsgQue = NULL;
	m_iMsgLen = iMsgLen;
	m_iQueSize = iQueueSize;
	CreateMsgQue(m_iMsgLen, m_iQueSize);
}

CWorkThread::~CWorkThread()
{
	if (m_lpMsgQue)
	{
		delete m_lpMsgQue;
		m_lpMsgQue = NULL;
	}
}

int CWorkThread::Run()
{
	char* pMsg = new char[m_iMsgLen];
	int iMsgLen = 0;
	unsigned short iMsgType = 0;
	IPlayerNode *lpPlayerNode = NULL;
	while(true)
	{
		printf("11\n");
		while ((iMsgLen = m_lpMsgQue->DeQueue((void**)&lpPlayerNode, iMsgType, pMsg, m_iMsgLen)) > 0)
		{
			CallOnNetMessage(lpPlayerNode, iMsgType, pMsg, iMsgLen);
		}
	}
	delete[] pMsg;
	return 0;
}

void CWorkThread::SetMsgQueName(char* szName)
{
	if (m_lpMsgQue)
	{
		m_lpMsgQue->SetQueName(szName);
	}
}

void CWorkThread::CreateMsgQue(int iMsgLen, int iQueSize)
{
	m_lpMsgQue = new MsgQueue(iQueSize, iMsgLen);
}


void CWorkThread::EnQueGameMsg(IPlayerNode *lpPlayerNode,unsigned short type, void* buffer, long lenth)
{
	if (m_lpMsgQue)
	{
		m_lpMsgQue->EnQueue(lpPlayerNode, type, buffer, lenth);
	}
}

int CWorkThread::GetQueMsgNum()
{
	return m_lpMsgQue->get_count;
}

void CWorkThread::CallOnNetMessage(IPlayerNode *lpPlayerNode, unsigned short iMsgType, char* pMsg, int iLen)
{
	if (NULL != lpPlayerNode)
	{
		switch(iMsgType)
		{
			printf("CWorkThread::CallOnNetMessage iMsgType %0x\n", iMsgType);
		case GAME_SERVER_ID_NOTICE:
		{
									  printf("GAME_SERVER_ID_NOTICE\n");
									  //OnServerRegisterReq(lpPlayerNode, buffer, length);
									  break;
		}
		case S_S_SAYHI_INFO:
		{
							   printf("S_S_SAYHI_INFO\n");
							   HandleSayHiReq(lpPlayerNode, pMsg, iLen);
							   break;
		}
		default:
			break;
		}
	}
	else
	{
		printf("error: player node is nil...\n");
	}
	
	
}

char* CWorkThread::GetMsgQueName()
{
	if (m_lpMsgQue)
	{
		return m_lpMsgQue->GetQuename();
	}
}

void CWorkThread::HandleSayHiReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	SS_SayHi* mMsg = (SS_SayHi*)buffer;
	S_S_MSG_TYPE mMsgType;
	SS_SayHi mMsgBack;
	//cout << "receve num value:" << mMsg->iSendNum << endl;
	__log(_ERROR, __FUNCTION__, "receve num value: %d", mMsg->iSendNum);
	if (mMsg->iSendNum <= 10)
	{
	//sleep(1);
	mMsgBack.iSendNum = mMsg->iSendNum + 1;
	MainLogic::Instance().SendData(lpPlayerNode, S_S_SAYHI_INFO, &mMsgBack, sizeof(mMsgBack));
	}
}



