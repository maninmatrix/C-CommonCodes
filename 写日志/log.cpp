#include "log.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include "IniFile.h"
#include "LogCls.h"
class GLogService : public CLogCls
{
public:
    GLogService()
    {
        m_level = -1;
    }

    void __log(int level, const char *lpszModule, const char *lpszFormat, va_list list)
    {
        if ((-1 != m_level) && (m_level <= level))
        {
            char szLog[2048] = {0};
            char szFormat[1024] = {0};
            char cColor;
            char cDefault = '3';
            char *lpszLevel;

            switch (level)
            {
            case _ALL:
                lpszLevel = "ALL";
                cDefault = '0';
                cColor = '0';
                break;

            case _DEBUG:
                lpszLevel = "DEBUG";
                cColor = '2';
                break;

            case _WARN:
                lpszLevel = "WARN";
                cColor = '3';
                break;

            case _ERROR:
                lpszLevel = "ERROR";
                cColor = '1';
                break;

            default:
                lpszLevel = "Unknown";
                cDefault = '0';
                cColor = '0';
                break;
            }

            time_t t = time(NULL);
            tm *ptm = localtime(&t);
            tm stm = {0}; //����, �Է�ptm = NULL
            if (NULL != ptm)
            {
                ptm->tm_year += 1900;
                ptm->tm_mon += 1;
                stm = *ptm;
            }

            sprintf(szFormat, "\e[%c%cm%04d-%02d-%02d %02d:%02d:%02d [%s] [%s] %s\e[0m",
                    cDefault, cColor, stm.tm_year, stm.tm_mon, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, lpszLevel, lpszModule, lpszFormat);

            long len = vsprintf(szLog, szFormat, list);
            _log_2(&szLog[25], len - 25 - 4);
            szLog[len++] = 0x0D;
            szLog[len] = 0x0A;
            printf(szLog);
        }
    }

public:
    int m_level;
}g_log;


bool InitializeLog(const char *lpszConfigFile, const char *lpszLogFileName)
{
    char szBuffer[1024] = {0};
    long len;

    if (-1 != (len = readlink("/proc/self/exe", szBuffer, sizeof(szBuffer))))
    {
        while ('/' != szBuffer[len--]);
        szBuffer[len + 1] = 0;
        chdir(szBuffer);
    }

    CIniFile ini(lpszConfigFile);

    g_log.m_level = _ALL;

    if (0 != (len = ini.GetValueStr("log", "level", szBuffer, sizeof(szBuffer))))
    {
        if (0 == strncasecmp(szBuffer, "ERROR", len))
        {
            g_log.m_level = _ERROR;
        }
        else if (0 == strncasecmp(szBuffer, "WARN", len))
        {
            g_log.m_level = _WARN;
        }
        else if (0 == strncasecmp(szBuffer, "DEBUG", len))
        {
            g_log.m_level = _DEBUG;
        }
    }

    mkdir("log", 0777); //S_IREAD | S_IWRITE | S_IEXEC

    if (NULL == lpszLogFileName)
    {
        lpszLogFileName = (0 != (len = ini.GetValueStr("log", "FileName", &szBuffer[512], sizeof(szBuffer) - 512))) ?
                &szBuffer[512] : "test.log";
    }

    sprintf(szBuffer, "./log/%s", lpszLogFileName);

    return g_log.InitLog(szBuffer, false, false, true);
}

void __log(LOG_LEVEL level, const char *lpszModule, const char *lpszFormat, ...)
{
    va_list list;
    va_start(list, lpszFormat); 
    g_log.__log(level, lpszModule, lpszFormat, list);
}
