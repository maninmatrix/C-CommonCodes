#ifndef __WORK_THREAD_H_
#define __WORK_THREAD_H_
#include "work_thread.h"
#include "msgqueue.h"
#include "GameServerNode.h"

#define LOBBY_LOGIN_REQ 0x001

class CWorkThread:public Thread
{
public:
	CWorkThread(int iMsgLen, int iQueueSize);
	virtual~CWorkThread();

private:
	virtual int Run();
	void CreateMsgQue(int iMsgLen, int iQueSize);
public:
	//设置消息队列名称
	void SetMsgQueName(char* szName);
	char* GetMsgQueName();
	//压入数据请求消息
	void EnQueGameMsg(IPlayerNode *lpPlayerNode,unsigned short type, void* buffer, long lenth);
	int GetQueMsgNum();	
	//消息处理
	void CallOnNetMessage(IPlayerNode *lpPlayerNode, unsigned short iMsgType, char* pMsg, int iLen);
	void HandleSayHiReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
private:
	int  m_iMsgLen;
	int  m_iQueSize;
	MsgQueue*  m_lpMsgQue;
};
#endif