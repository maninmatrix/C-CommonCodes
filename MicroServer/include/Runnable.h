#ifndef _SG_RUNNABLE_H_
#define _SG_RUNNABLE_H_

#include "Define.h"

namespace SGLib
{
    class IRunnable
    {
    public:
        virtual ~IRunnable(){}
        virtual void Run() = 0;
    };

};

#endif

