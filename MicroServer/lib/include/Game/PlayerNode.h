#ifndef __PLAYERNODE_H__
#define __PLAYERNODE_H__

#include "NetGame.h"

class PlayerNode : public IPlayerNode
{
public:
    virtual void Initialize()
    {

    }

    virtual void GetContext(EPOLLContext **lpContext)
    {
        if (NULL != lpContext)
        {
            *lpContext = m_lpContext;
        }
    }

private:
    virtual void SetContext(EPOLLContext *lpContext)
    {
        m_lpContext = lpContext;
    }

    virtual void Release()
    {
        delete this;
    }

private:
    EPOLLContext *m_lpContext;
};

#endif //__PLAYERNODE_H__
