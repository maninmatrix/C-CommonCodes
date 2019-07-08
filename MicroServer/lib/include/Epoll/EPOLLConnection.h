#ifndef __EPOLLCONNECTION_H__
#define __EPOLLCONNECTION_H__

#include "EPOLLContext.h"

#define INVALID_SOCKET  -1
#define SOCKET_ERROR    -1

class EPOLLNet;

class EPOLLConnection : public EPOLLContext
{
public:
    EPOLLConnection();

    virtual ulong SetContextAssociate(ulong ulAssociate);
    virtual ulong GetContextAssociate();
    virtual sockaddr_in& GetContextAddrIn(bool bLocal);
    virtual bool RecvData();
    virtual long SendData(void *buf, long len, bool block, sockaddr_in *to = NULL);
    virtual long SendData(void *buf, long len, bool block, char *lpszToHost, ushort nPort);
    virtual void Shutdown(long how);
    virtual void CloseContext();
    virtual bool IsClosed();
    virtual long setsockopt(long level, long optname, const char *optval, long optlen);
    virtual long getsockopt(long level, long optname, char *optval, long *optlen);
    virtual long SetKeepAlive(ulong onoff = 1, ulong keepalivetime = 10, ulong keepaliveinterval = 10, ulong keepcount = 5);
    virtual long SetCustomHeader();
    virtual long SetRecvAll(char *ifrn_name);

private:
    virtual void Release();

protected:
    virtual ~EPOLLConnection();
    virtual void Initialize();

public:
    inline bool IsAcceptClient() {return m_bAcceptClient;}
    inline EPOLLNet* GetEPOLLNet() {return m_lpEPOLLNet;}

protected:
    friend class Epoll;
    friend class EPOLLContextManager;

    void CloseSocket();

    int     m_socket;
    ulong   m_ulAssociate;
    ulong   m_ulConnectionCount;
    ulong*  m_lpulConnections;
    ulong   m_ulMaxConnections;
    bool    m_bClosed;
    bool    m_bRecvData;
    bool    m_bAcceptClient;
    bool    m_bBind;
    long    m_lProtocol;
    sockaddr_in m_addrLocal;
    sockaddr_in m_addrRemote;
    EPOLLConnection* m_lpPreContext;
    EPOLLConnection* m_lpNextContext;
    EPOLLConnection* m_lpNextFree;
    EPOLLNet* m_lpEPOLLNet;
    pthread_mutex_t m_Lock;
};

#endif //__EPOLLCONNECTION_H__
