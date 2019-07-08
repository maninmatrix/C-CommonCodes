#ifndef __EPOLLNET_H__
#define __EPOLLNET_H__

#include <pthread.h>
#include <limits.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_packet.h>
#include <net/if_arp.h>
#include <netpacket/packet.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <signal.h>


class EPOLLContext;

class IObject
{
public:
    virtual ~IObject(){};
};

class EPOLLNet : public IObject
{
public:
    //must be Initialize first; ( 0 < ulThreadCount <= NumberOfProcessors * 2 + 2, however, equal NumberOfProcessors )
    virtual bool Initialize(ulong ulThreadCount = 0, ulong ulContextInit = 0, ulong ulReadBuffer = 0) = 0;
    virtual void Shutdown() = 0;

    //time is millisecond
    virtual EPOLLContext* Create(bool bBind, const char *lpszHost, ushort nPort, ulong ulConnectTime = 0, ulong ulMaxConnections = 2000, long family = AF_INET, long type = SOCK_STREAM, long protocol = IPPROTO_TCP) = 0;
    virtual EPOLLContext* Create(bool bBind, sockaddr_in &addr, ulong ulConnectTime = 0, ulong ulMaxConnections = 2000, long family = AF_INET, long type = SOCK_STREAM, long protocol = IPPROTO_TCP) = 0;
};

#endif //__EPOLLNET_H__
