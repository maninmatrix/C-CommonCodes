#ifndef __NETGAME_H__
#define __NETGAME_H__

#include "Epoll/EPOLLNet.h"

class INetProvider : public EPOLLNet
{
public:
    //must be Initialize first; ( 0 < ulThreadCount <= NumberOfProcessors * 2 + 2, however, equal NumberOfProcessors )
    virtual bool Initialize(ulong ulInterval = 20, ulong ulTimeout = 2 * 60, ulong ulNetThreadCount = 0,
            ulong ulContextInit = 0, ulong ulReadBuffer = 0) = 0;

    virtual void ReleaseContext(EPOLLContext *lpContext) = 0;
};

class INetMessager : public IObject
{
public:
    virtual void OnReadCompleted(EPOLLContext *lpContext, void *buffer, long length) = 0;
    virtual void OnConnectionClosed(EPOLLContext *lpContext, bool bClosed) = 0;
    virtual void OnTimer(long tmNow) = 0;
    virtual EPOLLContext* OnCreateContext() = 0;
};

class IPlayerNode : public IObject
{
public:
    virtual void Initialize() = 0;
    virtual void Release() = 0;
    virtual void SetContext(EPOLLContext *lpContext) = 0;
    virtual void GetContext(EPOLLContext **lpContext) = 0;
};

class IGameProvider : public IObject
{
public:
    virtual long Start() = 0;
    //return TRUE if has been processed the message
    virtual bool OnReadComplete(IPlayerNode *lpPlayerNode, void *&NodeBuffer, long &NodeLength, void *ReadBuffer, long ReadLength) = 0;
    virtual void OnNetMessage(IPlayerNode *lpPlayerNode, unsigned short type, void *buffer, long length) = 0;
    virtual void OnClosed(IPlayerNode *lpPlayerNode, bool bClosed) = 0;
    virtual void OnTimer(long tmNow) = 0;
    virtual IPlayerNode* OnCreatePlayer() = 0;

    virtual ulong GetPlayerAddrIn(IPlayerNode *lpPlayerNode, sockaddr_in *lpAddrIn, bool bLocal = false) = 0;

    virtual char* GetEncodeName(long type) = 0;
    //return negative value indicate error
    virtual long Encode(char *lpMsgData, long lMsgLen, long encode, char *lpszEncode, long lLength) = 0;
    virtual long Decode(char *lpMsgData, long lMsgLen, long encode, char *lpszDecode, long lLength) = 0;
};

class INetSender : public IObject
{
public:
    virtual bool Initialize() = 0;

    virtual bool SendData(IPlayerNode *lpPlayerNode, unsigned short type, void *buf, long len, long encode) = 0;
    virtual bool SendData(IPlayerNode *lpPlayerNode, void *buf, long len) = 0;

    virtual void* AllocMassData(unsigned long ulCount, unsigned short type, void *buf, long len, long encode) = 0;
    virtual void* AllocMassData(unsigned long ulCount, void *buf, long len) = 0;
    virtual bool SendMassData(IPlayerNode *lpPlayerNode, void *lpMassData) = 0;

    virtual void Close(IPlayerNode *lpPlayerNode) = 0;
    virtual void OnClosed(IPlayerNode *lpPlayerNode) = 0;
};

#endif //__NETGAME_H__
