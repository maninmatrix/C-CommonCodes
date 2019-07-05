#ifndef __LOG_H__
#define __LOG_H__
#include <stdlib.h>
typedef enum LOG_LEVEL
{
    _ALL, _DEBUG, _WARN, _ERROR
}LOG_LEVEL;

bool InitializeLog(const char *lpszConfigFile, const char *lpszLogFileName = NULL);
void __log(LOG_LEVEL level, const char *lpszModule, const char *lpszFormat, ...);

#endif // __LOG_H__
