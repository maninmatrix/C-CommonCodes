#ifndef __EPOLLCONTEXT_H__
#define __EPOLLCONTEXT_H__

#include <arpa/inet.h>

class EPOLLContext
{
public:
    virtual ulong SetContextAssociate(ulong ulAssociate) = 0;
    virtual ulong GetContextAssociate() = 0;
    virtual sockaddr_in& GetContextAddrIn(bool bLocal) = 0;
    virtual bool RecvData() = 0;
    virtual long SendData(void *buf, long len, bool block, sockaddr_in *to = NULL) = 0;
    virtual long SendData(void *buf, long len, bool block, char *lpszToHost, ushort nPort) = 0;
    virtual void Shutdown(long how) = 0;
    virtual void CloseContext() = 0;
    virtual bool IsClosed() = 0;
    virtual long setsockopt(long level, long optname, const char *optval, long optlen) = 0;
    virtual long getsockopt(long level, long optname, char *optval, long *optlen) = 0;
    virtual long SetKeepAlive(ulong onoff = 1, ulong keepalivetime = 10, ulong keepaliveinterval = 10, ulong keepcount = 5) = 0;
    virtual long SetCustomHeader() = 0;
    virtual long SetRecvAll(char *ifrn_name) = 0;

private:
    virtual void Initialize() = 0;
    virtual void Release() = 0;

protected:
    virtual ~EPOLLContext() = 0;
};

#endif //__EPOLLCONTEXT_H__
