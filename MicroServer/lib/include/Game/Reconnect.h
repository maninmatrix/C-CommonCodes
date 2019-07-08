#ifndef __RECONNECT_H__
#define __RECONNECT_H__

#include "Game/NetGame.h"
#include "Common/Common.h"

class Reconnect
{
public:
    Reconnect();
    virtual ~Reconnect();

    bool Start();
    bool Stop();
    bool SetParameter(void *lpcbParam, int iType);

protected:
    //return false indicate reconnect again
    virtual bool OnConnected(void *lpcbParam, int iType) = 0;

private:
    void DoConnect();
    static void* ThreadProc(void *arg);

private:
    bool m_bExit;
    bool m_bStart;
    void *m_lpHostList;
    void *m_lpFactory;
    CEvent m_event;
};

#endif // __RECONNECT_H__
