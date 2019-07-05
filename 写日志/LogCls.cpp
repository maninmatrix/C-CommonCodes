#include "LogCls.h"
#include <time.h>
#include <memory.h>

#ifdef WIN32
#include <windows.h>
#define ReleaseEvent(x)
#else
#include <unistd.h> //usleep
#include <pthread.h>
#include <sched.h>
#include <limits.h>
#include <stdarg.h> //va_start
typedef void*  HANDLE;
typedef void* (*LPTHREAD_START_ROUTINE)(void*);
#define FALSE   0
#define TRUE    1
#define ReleaseEvent(x) delete (pthread_mutex_t*)x
#define Sleep(x) usleep(x)
#define CloseHandle(x) pthread_mutex_destroy((pthread_mutex_t*)x)
#define WaitForSingleObject(x, y) pthread_mutex_lock((pthread_mutex_t*)x)
#define SetEvent(x) pthread_mutex_unlock((pthread_mutex_t*)x)
inline void *CreateEvent(void*, int, int, void*)
{
    //pthread_mutexattr_t MutexAttr;
    //pthread_mutexattr_init(&MutexAttr);
    //pthread_mutexattr_settype(&MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
    //pthread_mutex_init(&h, &MutexAttr);
    pthread_mutex_t *h = new pthread_mutex_t;
    pthread_mutex_init(h, NULL);
    return h;
}
inline void CreateThread(void*, int, LPTHREAD_START_ROUTINE x, void* p, int, void*)
{
    pthread_t id;
    sched_param param;
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_attr_getschedparam(&attr, &param);
    param.sched_priority = 50;
    pthread_attr_setschedparam(&attr, &param);

    pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);

    pthread_create(&id, &attr, x, p);
    pthread_attr_destroy(&attr);
}
#endif //WIN32

#define MAX_BUFFER_SIZE 1024

typedef struct ListNode //log 链表
{
    char     szLog[MAX_BUFFER_SIZE];
    time_t   time;
    long     lLength;
    ListNode *lpNext;
}LISTNODE, *LPLISTNODE;

typedef struct Variant
{
    FILE *m_pf;  //文件指针
    char *m_pfn; //文件名
    LPLISTNODE m_write; //缓冲1链表头
    LPLISTNODE m_read;  //缓冲2链表头
    LPLISTNODE m_tail; //缓冲链表尾, 用于添加
    LPLISTNODE m_free;  //空闲链表池
    void *m_hWR; //添加log, 同步互斥对象指针
    void *m_hLQ; //链表池同步对象指针
    tm   m_tmToday; //记录今天的日期
    long m_lLength; //文件名长度
    long m_lPos;  //扩展名分隔
    bool m_bDelExist; //删除存在的文件
    bool m_bExit;//退出线程
    bool m_bInit;//初始化?
    bool m_bThread; //是否使用线程
    bool m_bDay; //每天产生一个log文件

    Variant() {memset(this, 0, sizeof(Variant));}
    ~Variant()
    {
        m_bExit = true;

        if (m_bThread)
        {
            while (true == m_bInit)
            {
                Sleep(1);
            }
        }

        LPLISTNODE p;
        while (NULL != m_free)
        {
            p = m_free;
            m_free = p->lpNext;
            delete p;
        }

        while (NULL != m_write)
        {
            p = m_write;
            m_write = p->lpNext;
            delete p;
        }

        while (NULL != m_read)
        {
            p = m_read;
            m_read = p->lpNext;
            delete p;
        }

        if (NULL != m_hWR)
        {
            CloseHandle((HANDLE)m_hWR);
            ReleaseEvent(m_hWR);
        }
        if (NULL != m_hLQ)
        {
            CloseHandle((HANDLE)m_hLQ);
            ReleaseEvent(m_hLQ);
        }
        if (NULL != m_pfn)
        {
            delete[] m_pfn;
        }
        if (NULL != m_pf)
        {
            fclose(m_pf);
        }
        memset(this, 0, sizeof(Variant));
    }
}Variant, *LPVariant;

void LogThread(void *p);

//////////////////////////////////////////////////////////////////////////
// CLogCls
//////////////////////////////////////////////////////////////////////////

const char lpszDay1[] = "%s%04d%02d%02d.%s";
const char lpszDay2[] = "%s%04d%02d%02d";
const char lpszTime[] = "%04d-%02d-%02d %02d:%02d:%02d ";
const char lpszLine[] = "\r\n";
const char lpszOpen[] = "a+b";

CLogCls::CLogCls()
{
    m_pv = NULL;
}

CLogCls::CLogCls(const char *lpszLogFile, bool bDelExist, bool bInThread, bool bInDay)
{
    m_pv = NULL;
    InitLog(lpszLogFile, bDelExist, bInThread, bInDay);
}

CLogCls::~CLogCls()
{
    if (NULL != m_pv)
    {
        delete (Variant*)m_pv;
        m_pv = NULL;
    }
}

bool CLogCls::InitLog(const char *lpszLogFile, bool bDelExist, bool bInThread, bool bInDay)
{
    bool bResult = false;

    if (NULL == m_pv)
    {
        m_pv = new Variant;
    }

    LPVariant p = (LPVariant)m_pv;

    if (false == p->m_bInit)
    {
        if (NULL != lpszLogFile)
        {
            if (NULL == p->m_pfn)
            {
                p->m_pfn = new char[260];
            }

            p->m_lLength = 0;
            while (0 != lpszLogFile[p->m_lLength])
            {
                if ('.' == lpszLogFile[p->m_lLength])
                {
                    p->m_lPos = p->m_lLength;
                }
                p->m_lLength++;
            }
            memset(p->m_pfn, 0, 260);
            memcpy(p->m_pfn, lpszLogFile, p->m_lLength < 259 ? p->m_lLength : 259);
            if (true == bDelExist) remove(lpszLogFile);
            bResult = (NULL != (p->m_pf = fopen((const char*)p->m_pfn, lpszOpen)));
        }

        if (true == bResult)
        {
            p->m_hWR = CreateEvent(NULL, FALSE, TRUE, NULL);
            if (true == bInThread)
            {
                p->m_hLQ = CreateEvent(NULL, FALSE, TRUE, NULL);
                CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LogThread, p, 0, NULL);
            }
            p->m_bInit = true;
        }
        p->m_bThread = bInThread;
        p->m_bDelExist = bDelExist;

        if (true == bInDay) //每日log
        {
            time_t t = time(NULL);
            tm *ptm = localtime(&t);
            if (NULL != ptm)
            {
                ptm->tm_year += 1900;
                ptm->tm_mon += 1;
                p->m_tmToday = *ptm;
            }
            p->m_bDay = bInDay;
        }
    }

    return bResult;
}

void CLogCls::_log_2(const char *lpszLog, long lLen)
{
    if ((NULL != lpszLog) && (NULL != m_pv) && (true == LPVariant(m_pv)->m_bInit) && (false == LPVariant(m_pv)->m_bExit))
    {
        LPVariant p = (LPVariant)m_pv;

        if (!p->m_bThread)
        {
            WaitForSingleObject(p->m_hWR, INFINITE);
            time_t t = time(NULL);
            tm *ptm = localtime(&t);
            tm stm = {0}; //定义, 以防ptm = NULL
            if (NULL != ptm)
            {
                ptm->tm_year += 1900;
                ptm->tm_mon += 1;
                stm = *ptm;
            }

            if ((p->m_bDay) && (stm.tm_mday != p->m_tmToday.tm_mday) && (0 != stm.tm_year)) //每日log
            {
                char szbuf[300];
                if (0 != p->m_lPos)
                {
                    ((char*)p->m_pfn)[p->m_lPos] = 0;
                    sprintf(szbuf, lpszDay1, (char*)p->m_pfn, p->m_tmToday.tm_year, p->m_tmToday.tm_mon, p->m_tmToday.tm_mday, &((char*)p->m_pfn)[p->m_lPos + 1]);
                    ((char*)p->m_pfn)[p->m_lPos] = '.';
                }
                else
                {
                    sprintf(szbuf, lpszDay2, p->m_pfn, p->m_tmToday.tm_year, p->m_tmToday.tm_mon, p->m_tmToday.tm_mday);
                }

                remove(szbuf);
                rename((const char*)p->m_pfn, szbuf);
                fclose(p->m_pf);
                if (NULL == (p->m_pf = fopen((const char*)p->m_pfn, lpszOpen)))
                {
                    p->m_pf = stdout;
                }

                p->m_tmToday = stm;
            }

            char buf[32];
            sprintf(buf, lpszTime, stm.tm_year, stm.tm_mon, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec);
            fwrite(buf, 20, 1, p->m_pf);
            if (-1 == lLen)
            {
                lLen = 0;
                while (0 != lpszLog[lLen])
                {
                    lLen++;
                }
            }
            fwrite(lpszLog, lLen, 1, p->m_pf);
            fwrite(lpszLine, 2, 1, p->m_pf);
            fflush(p->m_pf);

            SetEvent(p->m_hWR);
            return;
        }

        LPLISTNODE pList = NULL;

        WaitForSingleObject((HANDLE)p->m_hWR, INFINITE); //为了保证产生log的顺序, 必须一开始就要获取事件

        WaitForSingleObject((HANDLE)p->m_hLQ, INFINITE);
        if (NULL != p->m_free)
        {
            pList = p->m_free;
            p->m_free = pList->lpNext;
        }
        else
        {
            pList = new ListNode;
        }
        SetEvent((HANDLE)p->m_hLQ);

        if (-1 == lLen)
        {
            pList->lLength = 0;
            while (0 != lpszLog[pList->lLength])
            {
                pList->lLength++;
            }
        }
        else
        {
            pList->lLength = lLen;
        }
        memcpy(pList->szLog, lpszLog, pList->lLength + 1);
        pList->time = time(NULL);
        pList->lpNext = NULL;

        //WaitForSingleObject((HANDLE)p->m_hWR, INFINITE); //为了保证log产生顺序, 不能在这里才获取事件
        if (NULL == p->m_tail)
        {
            p->m_tail = pList;
            p->m_write = pList;
        }
        else
        {
            p->m_tail->lpNext = pList;
            p->m_tail = pList;
        }
        SetEvent((HANDLE)p->m_hWR);
    }
}

void CLogCls::_log(const char *lpszFormat, ...)
{
    if ((NULL != m_pv) && (true == LPVariant(m_pv)->m_bInit) && (false == LPVariant(m_pv)->m_bExit))
    {
        va_list list;
        va_start(list, lpszFormat);
        _log_1(lpszFormat, list);
    }
}

void CLogCls::_log_1(const char *lpszFormat, va_list list)
{
    if ((NULL != m_pv) && (true == LPVariant(m_pv)->m_bInit) && (false == LPVariant(m_pv)->m_bExit) && (NULL != lpszFormat))
    {
        char szLog[MAX_BUFFER_SIZE] = {0};

        vsprintf(szLog, lpszFormat, list);
        _log_2(szLog);
    }
}

void LogThread(void *p)
{
    LPVariant self = (LPVariant)p;
    long lCount = 0;

    while (1)
    {
        if (NULL != self->m_read)
        {
            time_t t = time(NULL);
            tm *ptm = localtime(&t);
            tm stm = {0}; //定义, 以防ptm = NULL
            if (NULL != ptm)
            {
                ptm->tm_year += 1900;
                ptm->tm_mon += 1;
                stm = *ptm;
            }

            if ((self->m_bDay) && (stm.tm_mday != self->m_tmToday.tm_mday) && (0 != stm.tm_year)) //每日log
            {
                char szbuf[300];
                if (0 != self->m_lPos)
                {
                    ((char*)self->m_pfn)[self->m_lPos] = 0;
                    sprintf(szbuf, lpszDay1, self->m_pfn, self->m_tmToday.tm_year, self->m_tmToday.tm_mon, self->m_tmToday.tm_mday, &((char*)self->m_pfn)[self->m_lPos + 1]);
                    ((char*)self->m_pfn)[self->m_lPos] = '.';
                }
                else
                {
                    sprintf(szbuf, lpszDay2, self->m_pfn, self->m_tmToday.tm_year, self->m_tmToday.tm_mon, self->m_tmToday.tm_mday);
                }

                remove(szbuf);
                rename((const char*)self->m_pfn, szbuf);
                fclose(self->m_pf);
                if (NULL == (self->m_pf = fopen((const char*)self->m_pfn, lpszOpen)))
                {
                    self->m_pf = stdout;
                }

                self->m_tmToday = stm;
            }

            do
            {
                ptm = localtime(&self->m_read->time);
                memset(&stm, 0, sizeof(stm));
                if (NULL != ptm)
                {
                    ptm->tm_year += 1900;
                    ptm->tm_mon += 1;
                    stm = *ptm;
                }
                char buf[32];
                sprintf(buf, lpszTime, stm.tm_year, stm.tm_mon, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec);
                fwrite(buf, 20, 1, self->m_pf);
                fwrite(self->m_read->szLog, self->m_read->lLength, 1, self->m_pf);
                fwrite(lpszLine, 2, 1, self->m_pf);

                LPLISTNODE pList = self->m_read;
                self->m_read = pList->lpNext;

                WaitForSingleObject((HANDLE)self->m_hLQ, INFINITE);
                pList->lpNext = self->m_free;
                self->m_free = pList;
                SetEvent((HANDLE)self->m_hLQ);
            } while (NULL != self->m_read);

            fflush(self->m_pf);
        }
        else
        {
            if ((true == self->m_bExit) && (2 == ++lCount))
            {
                break;
            }
            Sleep(1);
        }

        WaitForSingleObject((HANDLE)self->m_hWR, INFINITE);
        self->m_read = self->m_write;
        self->m_write = NULL;
        self->m_tail = NULL;
        SetEvent((HANDLE)self->m_hWR);
    }
    self->m_bInit = false;
}
