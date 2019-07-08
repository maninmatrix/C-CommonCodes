#ifndef __GAMEPROVIDER_H__
#define __GAMEPROVIDER_H__

#include "Game/NetGame.h"
#include "Common/Common.h"
#include "Game/NetMessage.h"
#include "MqMsgManager.h"

class GameProvider : public IGameProvider
{
public:
    GameProvider();
    virtual ~GameProvider();

    //must be Initialize first; ( 0 < ulThreadCount <= NumberOfProcessors * 2 + 2, however, equal NumberOfProcessors )
    bool Initialize(ulong ulMaxRecvCount = 0, ulong ulMaxSendCount = 0, ulong ulInterval = 20, ulong ulTimeout = 2 * 60, ulong ulNetThreadCount = 0, ulong ulContextInit = 0, ulong ulReadBuffer = 0);

	//创建并初始化Mq管理器
	//pMqUrl			-- MQ 服务器地址
	//pProducerId		-- 您在MQ控制台创建的producer
	//pPublishTopics	-- 您在MQ控制台申请的topic
	//pAccessKey		-- 阿里云身份验证，在阿里云服务器管理控制台创建(账号)
	//pSecretKey		-- 阿里云身份验证，在阿里云服务器管理控制台创建(密码)
	//pLogPath			-- 日志文件路径
	void InitMqManager(char *pMqUrl, char *pProducerId, char *pPublishTopics, char *pAccessKey, char *pSecretKey, char* pLogPath = "./");
	//发送一般消息到Mq服务器
	bool SendCommonMqMsg(char *buffer);	
	//
	bool SendCommonMqMsg(CJsonPack &JsonPack);
	bool SendCommonMqMsg(CJsonPack* pJsonPack);
	
    void Shutdown();

    //time is millisecond
    IPlayerNode* Create(bool bBind, const char *lpszHost, ushort nPort, ulong ulConnectTime = 0, ulong ulMaxConnections = 2000, long family = AF_INET, long type = SOCK_STREAM, long protocol = IPPROTO_TCP);
    IPlayerNode* Create(bool bBind, sockaddr_in &addr, ulong ulConnectTime = 0, ulong ulMaxConnections = 2000, long family = AF_INET, long type = SOCK_STREAM, long protocol = IPPROTO_TCP);

    bool SendData(IPlayerNode *lpPlayerNode, unsigned short type, void *buf, long len, long encode = ENCODE_NONE);
    bool SendData(IPlayerNode *lpPlayerNode, void *buf, long len);

    bool RecvData(IPlayerNode *lpPlayerNode);

    void* AllocMassData(unsigned long ulCount, unsigned short type, void *buf, long len, long encode = ENCODE_NONE);
    bool SendMassData(IPlayerNode *lpPlayerNode, void *lpMassData);

    virtual ulong GetPlayerAddrIn(IPlayerNode *lpPlayerNode, sockaddr_in *lpAddrIn, bool bLocal = false);

    void Close(IPlayerNode *lpPlayerNode);

public:
    static long atomic_inc(volatile long *v);
    static long atomic_dec(volatile long *v);
    static long atomic_xchg(volatile long *v, long i);
    static ulong dns2ip(const char *lpDnsName);

protected:
    //IGameProvider
    virtual bool OnReadComplete(IPlayerNode *lpPlayerNode, void *&NodeBuffer, long &NodeLength, void *ReadBuffer, long ReadLength);
    virtual void OnNetMessage(IPlayerNode *lpPlayerNode, unsigned short type, void *buffer, long length);
    virtual void OnClosed(IPlayerNode *lpPlayerNode, bool bClosed);
    virtual void OnTimer(long tmNow);

    virtual char* GetEncodeName(long type);
    //return negative value indicate error
    virtual long Encode(char *lpMsgData, long lMsgLen, long encode, char *lpszEncode, long lLength);
    virtual long Decode(char *lpMsgData, long lMsgLen, long encode, char *lpszDecode, long lLength);

private:
    INetMessager *m_lpNetMessager;
    INetProvider *m_lpNetProvider;
    INetSender   *m_lpNetSender;

	//CMqMsgManager* m_lpMqMsgManager;
};

#endif // __GAMEPROVIDER_H__
