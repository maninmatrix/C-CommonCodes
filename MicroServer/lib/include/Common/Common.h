#ifndef __COMMONALITY_H__
#define __COMMONALITY_H__

//error: 0; ok: 1
long md5_hex(const char *lpszBuff, unsigned long uLength, void *lpszMD5, unsigned long size = 16);
long md5_str(const char *lpszBuff, unsigned long uLength, void *lpszMD5, unsigned long size = 33);

char *EncodeXor(char *lpszSrc, unsigned long ulSrcLen, const char *lpszKey, unsigned long ulKeyLen);

//return real length of destination buffer used, 0 indicate fail
//just encode ascii character
unsigned long Encode7Bit(const char *lpszSrc, unsigned long ulSrcLen, char *lpszDst, unsigned long ulDstLen);
unsigned long Decode7Bit(const char *lpszSrc, unsigned long ulSrcLen, char *lpszDst, unsigned long ulDstLen);

//lIndex1, lIndex2, two element of compare data
//lParam, the lParam of the sort function
typedef long (*COMPAREFUNC)(long lIndex1, long lIndex2, long lParam);
typedef void (*EXCHANGEDATA)(long lIndex1, long lIndex2, long lParam);

void qsort(long left, long right, long lParam, COMPAREFUNC comp, EXCHANGEDATA exchange);

//the Merge Sort must be save data in the other space, so

//lIndex1, index to save of the other sapce
//lIndex2, index of compare data
//lParam, the lParam of the sort function
typedef void (*SAVEDATA)(long lIndex1, long lIndex2, long lParam);

//lIndex1, index of compare data to be lay back
//lIndex2, index of the other space
//lParam, the lParam of the sort function
typedef void (*LAYBACK)(long lIndex1, long lIndex2, long lParam);

void MergeSort(long low, long high, long lParam, COMPAREFUNC comp, SAVEDATA save, LAYBACK layback);

//CIniFile, ListCtrlEdit, CTrayIcon, CThunk, CLogCls, HtmlDocs, CWebBrowser

//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

class CIniFile
{
public:
    CIniFile();
    CIniFile(const char *lpFileName);
    virtual ~CIniFile();

    //fail is return zero
    int GetSectionNames(char *lpBuf, int size);
    int GetKeyNames(const char *lpSectionName, char *lpBuf, int size);
    int GetValueStr(const char *lpSectionName, const char *lpKeyName, char *lpBuf, int size);
    int GetValueInt(const char *lpSectionName, const char *lpKeyName, int iDefault);

    int SetValueStr(const char *lpSectionName, const char *lpKeyName, const char *lpStr, bool bSplit = true, int len = -1);
    int SetValueInt(const char *lpSectionName, const char *lpKeyName, int nValue);

    int DeleteSection(const char *lpSectionName);
    int DeleteKey(const char *lpSectionName, const char *lpKeyName);

    int GetLineCount();

    bool InitFile(const char *lpFileName);

private:
    char     *m_lpDataBuf;
    char     m_szFileName[260];
    int      m_iLineCount;
    void     *m_lpLink;
};

//////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
typedef enum LOG_LEVEL
{
    _ALL, _DEBUG, _WARN, _ERROR
}LOG_LEVEL;

bool InitializeLog(const char *lpszConfigFile, const char *lpszLogFileName = NULL);
void __log(LOG_LEVEL level, const char *lpszModule, const char *lpszFormat, ...);

//////////////////////////////////////////////////////////////////////////
#include <pthread.h>

class CMutex
{
public:
    CMutex();
    virtual ~CMutex();

    int lock();
    int unlock();

private:
    pthread_mutex_t m_mutex;
};

//////////////////////////////////////////////////////////////////////////
#include <semaphore.h>

class CEvent
{
public:
    CEvent();
    virtual ~CEvent();

    bool Init(bool bManual = false, bool bState = false);
    bool Set();
    bool Reset();
    bool Wait();
    bool TryWait();

private:
    bool  m_bManual;
    sem_t m_sem;
};

#endif //__COMMONALITY_H__
