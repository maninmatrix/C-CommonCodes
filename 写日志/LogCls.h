#ifndef __LOG_CLS_H__
#define __LOG_CLS_H__

#include <stdio.h>
#include <stdarg.h>

class CLogCls
{
public:
    CLogCls();
    CLogCls(const char *lpszLogFile, bool bDelExist = false, bool bInThread = true, bool bInDay = false);
    virtual ~CLogCls();
    
    bool InitLog(const char *lpszLogFile, bool bDelExist = false, bool bInThread = true, bool bInDay = false);
    void _log(const char *lpszFormat, ...);
    void _log_1(const char *lpszFormat, va_list list);
    void _log_2(const char *lpszLog, long lLen = -1);
    
private:
    void *m_pv;
};

#endif //__LOG_CLS_H__
