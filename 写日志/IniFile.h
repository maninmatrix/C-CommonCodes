#ifndef __INIFILE_H__
#define __INIFILE_H__

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string>
#include <vector>
using namespace std;

vector<string> split(const string &s, const string &seperator);

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

#endif // __INIFILE_H__
