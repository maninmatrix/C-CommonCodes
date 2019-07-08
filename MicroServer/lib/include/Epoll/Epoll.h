#ifndef __EPOLL_H__
#define __EPOLL_H__

#include "EPOLLNet.h"

class EPOLLContext;

class Epoll : public EPOLLNet
{
public:
    Epoll();
    virtual ~Epoll();

    //must be Initialize first; ( 0 < ulThreadCount <= NumberOfProcessors * 2 + 2, however, equal NumberOfProcessors )
    virtual bool Initialize(ulong ulThreadCount = 0, ulong ulContextInit = 0, ulong ulReadBuffer = 0);
    virtual void Shutdown();
    virtual bool IsShutdown();

    //time is millisecond
    virtual EPOLLContext* Create(bool bBind, const char *lpszHost, ushort nPort, ulong ulConnectTime = 0, ulong ulMaxConnections = 2000, long family = AF_INET, long type = SOCK_STREAM, long protocol = IPPROTO_TCP);
    virtual EPOLLContext* Create(bool bBind, sockaddr_in &addr, ulong ulConnectTime = 0, ulong ulMaxConnections = 2000, long family = AF_INET, long type = SOCK_STREAM, long protocol = IPPROTO_TCP);

    virtual void CloseAllConnections();

protected:
    //invoke Epoll::OnConnectionClosed and synchronize to the thread of SendData()
    //if TRUE of the bClosed, indication close socket by self, otherwise, destination closed
    virtual void OnConnectionClosed(EPOLLContext *lpContext, bool bClosed, sockaddr_in *addr);

    //please call Epoll::OnReadCompleted to release, if the buffer has been not used
    virtual void OnReadCompleted(EPOLLContext *lpContext, void *buffer, long lLength, sockaddr_in *from);

private:
    //the return value must be derive from EPOLLConnection
    virtual EPOLLContext* OnCreateConnection();

    //if to be add it, set bAdd to TRUE
    virtual void OnMaxLimitConnection(EPOLLContext *lpContext, sockaddr_in *from, bool& bAdd);

    //TCP client connect in
    virtual void OnConnectionEstablished(EPOLLContext *lpContext, sockaddr_in *from);

private:
    bool AddAConnection(EPOLLContext *lpContext, EPOLLContext *lpListener);
    void FreeConnection(EPOLLContext *lpContext);

    void PollWait();
    static void PollRoutine(void *pParam);

    friend class EPOLLConnection;
    friend class EPOLLContextManager;
private:
    bool    m_bStart;
    bool    m_bShutdown;
    int     m_iPoll;
    ulong   m_ulConnectionCount;
    long    m_lThreadCount;
    void*   m_lpEpollContext;
    void*   m_lpEpollBuffer;
    void*   m_lpLogCls;
    EPOLLContext* m_lpContextList;
    pthread_mutex_t m_ContextLock;
};

//0 is indicate success, or return error number;
int CreateThread(pthread_attr_t *__attr, size_t __stacksize, void *(*__start_routine)(void*), void *__arg, pthread_t *__id);

long atomic_inc(volatile long *v);
long atomic_dec(volatile long *v);
long atomic_add(volatile long *v, long i);
long atomic_xchg(volatile long *v, long i);

ulong  dns2ip(const char *lpDnsName);
ushort CheckSum(void *lpData, int iSize);

int BuildIcmpPacket(void *lpPacketBuf, int iBufSize, ushort id, void *lpDataBuf, int iDataSize);
int BuildUdpPacket(void *lpPacketBuf, int iBufSize, ulong ulSrcIp, ushort srcPort, ulong ulDestIp, ushort destPort, void *lpDataBuf, int iDataSize);
int BuildTcpPacket(void *lpPacketBuf, int iBufSize, ulong ulSrcIp, ushort srcPort, ulong ulDestIp, ushort destPort, int flag, uint seq, void *lpDataBuf, int iDataSize);

//ICMP 类型
#define ICMP_ECHOQUEST    8         //请求回应
#define ICMP_ECHOREPLY    0         //回应请求

//定义TCP标志
#define TCP_FIN     0x01
#define TCP_SYN     0x02
#define TCP_RST     0x04
#define TCP_PSH     0x08
#define TCP_ACK     0x10
#define TCP_URG     0x20
#define TCP_ACE     0x40
#define TCP_CWR     0x80

#endif //__EPOLL_H__
