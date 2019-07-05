#include "IniFile.h"

#define min(a,b)  (((a) < (b)) ? (a) : (b))

struct LINKNODE
{
    char *iX1;
    char *iX2;
    char *iV1;
    char *iV2;
    LINKNODE *pNext;
};

#ifdef WIN32 //windows

#define _strcmpi    strnicmp

#else //linux ?

#define _strcmpi    strncasecmp

#endif //windows

#define INIT()\
{\
    m_lpDataBuf = NULL;\
    m_lpLink = NULL;\
    m_iLineCount = 0;\
    memset(m_szFileName, 0, sizeof(m_szFileName));\
}

#define FINAL()\
{\
    if (NULL != m_lpDataBuf)\
{\
    delete[] m_lpDataBuf;\
    m_lpDataBuf = NULL;\
}\
    \
    while (NULL != m_lpLink)\
{\
    LINKNODE *pNode = ((LINKNODE*)m_lpLink)->pNext;\
    delete (LINKNODE*)m_lpLink;\
    m_lpLink = pNode;\
}\
    \
    m_iLineCount = 0;\
}

//////////////////////////////////////////////////////////////////////////
CIniFile::CIniFile()
{
    INIT();
}

//////////////////////////////////////////////////////////////////////////
CIniFile::CIniFile(const char *lpFileName)
{
    INIT();
    InitFile(lpFileName);
}

//////////////////////////////////////////////////////////////////////////
CIniFile::~CIniFile()
{
    FINAL();
}

//////////////////////////////////////////////////////////////////////////
int CIniFile::GetLineCount()
{
    return m_iLineCount;
}

//////////////////////////////////////////////////////////////////////////
bool CIniFile::InitFile(const char *lpFileName)
{
    bool b = false;

    if (NULL != lpFileName)
    {
        if (lpFileName != m_szFileName)
        {
            int iLen = 0;
            while (0 != lpFileName[iLen]) iLen++;
            memset(m_szFileName, 0, 260);
            memcpy(m_szFileName, lpFileName, min(iLen, 259));
        }

        static const char lpszRead[] = "rb";
        FILE *pf = fopen(lpFileName, lpszRead);
        if (NULL != pf)
        {
            FINAL();
            fseek(pf, 0, SEEK_END);
            long lSize = ftell(pf);
            if (0 < lSize)
            {
                m_lpDataBuf = new char[lSize];
                if (NULL != m_lpDataBuf)
                {
                    fseek(pf, 0, SEEK_SET);
                    fread(m_lpDataBuf, lSize, 1, pf);

                    LINKNODE *lpTail = new LINKNODE;
                    if (NULL != lpTail)
                    {
                        memset(lpTail, 0, sizeof(LINKNODE));
                        m_lpLink = lpTail;
                        bool bReturn = false;
                        int iSpaceCount = 0;
                        char *lpIndex = m_lpDataBuf;
                        unsigned char cByte;

                        for (long i = 0; i < lSize; i++, lpIndex++)
                        {
                            cByte = *lpIndex;

                            if ((0x20 != cByte) && (0x09 != cByte) && (0 == lpTail->iX1)) //���ǿո�, ����Table���ָ�, ����û�и�ֵ
                            {
                                lpTail->iX1 = lpIndex; //��ʾsection����key�Ŀ�ʼ
                            }
                            else if ((']' == cByte) && (0 != lpTail->iX1) && ('[' == lpTail->iX1[0])) //�����']',��ʾ�ǽڵĽ���, �����뿪ʼ��'['�ַ�
                            {
                                lpTail->iX2 = lpIndex; //��ʾsection����
                            }
                            else if ((0x20 != cByte) && (0x09 != cByte) && (0 != lpTail->iX2) && (0 == lpTail->iV1)) //���ǿո�, ����Table���ָ�, ����section��key�ѽ���, ����û�и�ֵ
                            {
                                lpTail->iV1 = lpIndex; //value��ʼ
                            }
                            else if ((0 != lpTail->iX1) && ('[' != lpTail->iX1[0]) && (0 == lpTail->iX2)) //section����key�Ѿ���¼��ʼ, ������section(Ҳ����key��¼�˿�ʼ), ���ҽ���û��ֵ
                            {
                                if ('=' == cByte)
                                {
                                    lpTail->iX2 = lpIndex - iSpaceCount; //key����
                                    iSpaceCount = 0;
                                }
                                else if ((0x20 == cByte) || (0x09 == cByte)) //'='��key��ʼ�Ŀո��Table���ָ���
                                {
                                    iSpaceCount++;
                                }
                                else //������ǿո����Table���ָ�, ��ʾkey֮ǰ�пո����Table���ָ�
                                {
                                    iSpaceCount = 0; //��0, ���¼���
                                }
                            }

                            if (0x0D == cByte) //�س�, ����DOSģʽ�Ļس�����
                            {
                                bReturn = true;
                            }
                            else  if (0x0A == cByte) //����
                            {
                                if (('[' == lpTail->iX1[0]) && (0 == lpTail->iX2)) //�����ʼ��'[', ��û��']'����, Ҳ��ʾ��section����
                                {
                                    lpTail->iX2 = (true == bReturn) ? lpIndex - 1 : lpIndex;
                                }
                                else //����section
                                {
                                    if (0 == lpTail->iX2) //key����û�н���, ����Ϊû��value����, ֻ��key
                                    {
                                        lpTail->iX2 = (true == bReturn) ? lpIndex - 1 : lpIndex;
                                    }
                                    else //value����
                                    {
                                        if (0 == lpTail->iV1) //key������(Ҳ����'='����)û��������, ����ֻ�пո�,Table���ָ�
                                        {
                                            lpTail->iV1 = lpTail->iV2 = (true == bReturn) ? lpIndex - 1 : lpIndex; //valueû������
                                        }
                                        else //value������
                                        {
                                            lpTail->iV2 = (true == bReturn) ? lpIndex - 1 : lpIndex;
                                        }
                                    }
                                }

                                m_iLineCount++;
                                bReturn = false;
                                LINKNODE *pNode = new LINKNODE;
                                if (NULL != pNode)
                                {
                                    memset(pNode, 0, sizeof(LINKNODE));
                                    lpTail->pNext = pNode;
                                    lpTail = pNode;
                                }
                            }
                        }
                        //�������һ��
                        if (0 == lpTail->iX1) //���к�,ֻ�пո��û������
                        {
                            lpTail->iX1 = lpTail->iX2 = lpIndex;
                        }
                        else //������, û�л���, û�н�����ʶ(']')����ںź���ȫ�ǿո��û����
                        {
                            if (0 == lpTail->iX2)
                            {
                                lpTail->iX2 = (true == bReturn) ? lpIndex - 1 : lpIndex;
                            }
                            else
                            {
                                if (0 == lpTail->iV1)
                                {
                                    lpTail->iV1 = lpTail->iV2 = (true == bReturn) ? lpIndex - 1 : lpIndex;
                                }
                                else
                                {
                                    lpTail->iV2 = (true == bReturn) ? lpIndex - 1 : lpIndex;
                                }
                            }
                        }
                        b = true;
                    }
                }
            }
            fclose(pf);
        }
    }

    return b;
}

//////////////////////////////////////////////////////////////////////////
int CIniFile::GetSectionNames(char *lpBuf, int size)
{
    int nResult = 0;

    LINKNODE *pNode = (LINKNODE*)m_lpLink;
    int nSize = size;

    while (NULL != pNode)
    {
        if ('[' == pNode->iX1[0])
        {
            int iLen = pNode->iX2 - pNode->iX1;
            nResult += iLen;

            if ((NULL != lpBuf) && (nSize > iLen))
            {
                memcpy(&lpBuf[size - nSize], pNode->iX1 + 1, iLen - 1);
                lpBuf[nResult - 1] = 0;
                nSize -= iLen;
            }
            else
            {
                nSize = -1;
            }
        }
        pNode = pNode->pNext;
    }

    if ((NULL != lpBuf) && (0 < size)) lpBuf[nResult] = 0; //0������

    if (0 != nResult) nResult++;

    return nResult;
}

//////////////////////////////////////////////////////////////////////////
int CIniFile::GetKeyNames(const char *lpSectionName, char *lpBuf, int size)
{
    int nResult = 0;

    if ((NULL != lpSectionName) && (NULL != m_lpLink))
    {
        LINKNODE *pNode = (LINKNODE*)m_lpLink;
        bool bFindSec = false;
        int nSize = size;
        int iSecLen = strlen(lpSectionName);

        while (NULL != pNode)
        {
            if (false == bFindSec)
            {
                if ('[' == pNode->iX1[0])
                {
                    if ((pNode->iX2 - pNode->iX1 - 1 == iSecLen) &&
                        (0 == _strcmpi(lpSectionName, pNode->iX1 + 1, iSecLen)))
                    {
                        bFindSec = true;
                    }
                }
            }
            else
            {
                if ('[' == pNode->iX1[0])
                {
                    break;
                }
                else
                {
                    if (pNode->iX1 != pNode->iX2)
                    {
                        int iLen = pNode->iX2 - pNode->iX1 + 1;
                        nResult += iLen;

                        if ((NULL != lpBuf) && (nSize > iLen))
                        {
                            memcpy(&lpBuf[size - nSize], pNode->iX1, iLen);
                            lpBuf[nResult - 1] = 0;
                            nSize -= iLen;
                        }
                        else
                        {
                            nSize = -1;
                        }
                    }
                }
            }
            pNode = pNode->pNext;
        }
    }

    if ((NULL != lpBuf) && (0 < size)) lpBuf[nResult] = 0; //0������

    if (0 != nResult) nResult++;

    return nResult;
}

//////////////////////////////////////////////////////////////////////////
int CIniFile::GetValueStr(const char *lpSectionName, const char *lpKeyName, char *lpBuf, int size)
{
    int nResult = 0;

    if ((NULL != lpSectionName) && (NULL != lpKeyName) && (NULL != m_lpLink))
    {
        LINKNODE *pNode = (LINKNODE*)m_lpLink;
        bool bFindSec = false;
        int  iLen = strlen(lpKeyName);

        while (NULL != pNode)
        {
            if (false == bFindSec)
            {
                if ('[' == pNode->iX1[0])
                {
                    int iLen = strlen(lpSectionName);
                    if ((pNode->iX2 - pNode->iX1 - 1 == iLen) &&
                        (0 == _strcmpi(lpSectionName, pNode->iX1 + 1, iLen)))
                    {
                        bFindSec = true;
                    }
                }
            }
            else
            {
                if ('[' == pNode->iX1[0])
                {
                    break;
                }
                else
                {
                    if (0 != pNode->iV1)
                    {
                        if ((pNode->iX2 - pNode->iX1 == iLen) && (0 == _strcmpi(lpKeyName, pNode->iX1, iLen)))
                        {
                            iLen = pNode->iV2 - pNode->iV1;
                            nResult = iLen + 1;
                            if ((NULL != lpBuf) && (size > iLen))
                            {
                                memcpy(lpBuf, pNode->iV1, nResult);
                            }
                            break;
                        }
                    }
                }
            }
            pNode = pNode->pNext;
        }
    }

    if ((NULL != lpBuf) && (0 < size))
    {
        (0 != nResult) ?
            lpBuf[nResult - 1] = 0: //0������
        lpBuf[0] = 0;
    }

    return nResult;
}

//////////////////////////////////////////////////////////////////////////
int CIniFile::GetValueInt(const char *lpSectionName, const char *lpKeyName, int iDefault)
{
    int result = iDefault;

    int iLen = GetValueStr(lpSectionName, lpKeyName, NULL, 0);

    if (0 != iLen)
    {
        char *lpValue = new char[iLen];
        if (NULL != lpValue)
        {
            GetValueStr(lpSectionName, lpKeyName, lpValue, iLen);
            result = atoi(lpValue);
            delete[] lpValue;
        }
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
static char const lpszWrite[] = "wb";
int CIniFile::SetValueStr(const char *lpSectionName, const char *lpKeyName, const char *lpStr, bool bSplit /* = true */, int len /* = -1 */)
{
    int nResult = 0;

    if ((0 != m_szFileName[0]) && (NULL != lpSectionName) && (NULL != lpKeyName))
    {
        FILE *pf = fopen(m_szFileName, lpszWrite);

        if (NULL != pf)
        {
            int nSecLen = strlen(lpSectionName);
            int nKeyLen = strlen(lpKeyName);
            int nValLen = (NULL == lpStr) ? 0 : ((-1 == len) ? strlen(lpStr) : len);

            bool bFindSec = false;
            bool bDone = false;
            LINKNODE *pNode = (LINKNODE*)m_lpLink;
            char *lpszPos = (NULL == pNode) ? NULL : pNode->iX1;
            while (NULL != pNode)
            {
                if (false == bFindSec)
                {
                    if (('[' == pNode->iX1[0]) && (pNode->iX2 - pNode->iX1 - 1 == nSecLen) &&
                        (0 == _strcmpi(lpSectionName, pNode->iX1 + 1, nSecLen)))
                    {
                        bFindSec = true;
                    }
                }
                else if (false == bDone)
                {
                    if ('[' == pNode->iX1[0])
                    {
                        while (0x0A != (--lpszPos)[0]);
                        fseek(pf, ++lpszPos - m_lpDataBuf, SEEK_SET);
                        fwrite(lpKeyName, nKeyLen, 1, pf);
                        if (true == bSplit)
                        {
                            fwrite(" = ", 3, 1, pf);
                        }
                        if (0 != nValLen)
                        {
                            fwrite(lpStr, nValLen, 1, pf);
                        }
                        fwrite("\r\n", 2, 1, pf);
                        bDone = true;
                    }
                    else
                    {
                        if ((pNode->iX2 - pNode->iX1 == nKeyLen) && (0 == _strcmpi(lpKeyName, pNode->iX1, nKeyLen)))
                        {
                            fwrite(lpszPos, nKeyLen, 1, pf);
                            if (true == bSplit)
                            {
                                fwrite(" = ", 3, 1, pf);
                            }
                            if (0 != nValLen)
                            {
                                fwrite(lpStr, nValLen, 1, pf);
                            }
                            lpszPos = (0 == pNode->iV2) ? pNode->iX2 : pNode->iV2;
                            bDone = true;
                        }
                    }
                }

                if (NULL != pNode->pNext)
                {
                    fwrite(lpszPos, pNode->pNext->iX1 - lpszPos, 1, pf);
                    lpszPos = pNode->pNext->iX1;
                }
                else
                {
                    if (0 == pNode->iV2)
                    {
                        fwrite(lpszPos, pNode->iX2 - lpszPos, 1, pf);
                        lpszPos = pNode->iX2;
                    }
                    else
                    {
                        fwrite(lpszPos, pNode->iV2 - lpszPos, 1, pf);
                        lpszPos = pNode->iV2;
                    }
                }

                pNode = pNode->pNext;
            }

            if (false == bDone) //�����궼û���ҵ���Ҫ���õ�����, ���
            {
                if ((NULL != lpszPos) && (0x0A != (--lpszPos)[0]))
                {
                    fwrite("\r\n", 2, 1, pf);
                }
                if (false == bFindSec) //�ж��Ƿ��Ѿ�����section, ���ھͲ���д��
                {
                    fwrite("[", 1, 1, pf);
                    fwrite(lpSectionName, nSecLen, 1, pf);
                    fwrite("]", 1, 1, pf);
                    fwrite("\r\n", 2, 1, pf);
                }
                fwrite(lpKeyName, nKeyLen, 1, pf);
                if (true == bSplit)
                {
                    fwrite(" = ", 3, 1, pf);
                }
                if (0 != nValLen)
                {
                    fwrite(lpStr, nValLen, 1, pf);
                }
                bDone = true;
            }

            nResult = (int)bDone;

            fclose(pf);

            if (bDone) InitFile(m_szFileName);
        }
    }

    return nResult;
}

//////////////////////////////////////////////////////////////////////////
int CIniFile::SetValueInt(const char *lpSectionName, const char *lpKeyName, int nValue)
{
    char buf[100];
    sprintf(buf, "%d", nValue);

    return SetValueStr(lpSectionName, lpKeyName, buf);
}

//////////////////////////////////////////////////////////////////////////
int CIniFile::DeleteSection(const char *lpSectionName)
{
    int nResult = 0;

    if ((0 != m_szFileName[0]) && (NULL != lpSectionName))
    {
        FILE *pf = fopen(m_szFileName, lpszWrite);

        if (NULL != pf)
        {
            int nSecLen = strlen(lpSectionName);

            bool bFindSec = false;
            bool bDone = false;
            LINKNODE *pNode = (LINKNODE*)m_lpLink;
            char *lpszPos = (NULL == pNode) ? NULL : pNode->iX1;
            while (NULL != pNode)
            {
                if (false == bFindSec)
                {
                    if (('[' == pNode->iX1[0]) && (pNode->iX2 - pNode->iX1 - 1 == nSecLen) &&
                        (0 == _strcmpi(lpSectionName, pNode->iX1 + 1, nSecLen)))
                    {
                        bDone = true;
                        bFindSec = true;
                    }
                }
                else
                {
                    if ('[' == pNode->iX1[0])
                    {
                        bFindSec = false;
                        char *lpszTmp = lpszPos;
                        while (0x0A != (--lpszPos)[0]);
                        fseek(pf, ++lpszPos - lpszTmp + 1, SEEK_END);
                    }
                }

                if (NULL != pNode->pNext)
                {
                    if (false == bFindSec)
                    {
                        fwrite(lpszPos, pNode->pNext->iX1 - lpszPos, 1, pf);
                    }
                    lpszPos = pNode->pNext->iX1;
                }
                else
                {
                    if (0 == pNode->iV2)
                    {
                        if (false == bFindSec)
                        {
                            fwrite(lpszPos, pNode->iX2 - lpszPos, 1, pf);
                        }
                        lpszPos = pNode->iX2;
                    }
                    else
                    {
                        if (false == bFindSec)
                        {
                            fwrite(lpszPos, pNode->iV2 - lpszPos, 1, pf);
                        }
                        lpszPos = pNode->iV2;
                    }
                }

                pNode = pNode->pNext;
            }

            nResult = (int)bDone;

            fclose(pf);

            if (bDone) InitFile(m_szFileName);
        }
    }

    return nResult;
}

//////////////////////////////////////////////////////////////////////////
int CIniFile::DeleteKey(const char *lpSectionName, const char *lpKeyName)
{
    int nResult = 0;

    if ((0 != m_szFileName[0]) && (NULL != lpSectionName) && (NULL != lpKeyName))
    {
        FILE *pf = fopen(m_szFileName, lpszWrite);

        if (NULL != pf)
        {
            int nSecLen = strlen(lpSectionName);
            int nKeyLen = strlen(lpKeyName);

            bool bFindSec = false;
            bool bFindKey = false;
            bool bDone = false;
            LINKNODE *pNode = (LINKNODE*)m_lpLink;
            char *lpszPos = (NULL == pNode) ? NULL : pNode->iX1;
            while (NULL != pNode)
            {
                if (false == bFindSec)
                {
                    if (('[' == pNode->iX1[0]) && (pNode->iX2 - pNode->iX1 - 1 == nSecLen) &&
                        (0 == _strcmpi(lpSectionName, pNode->iX1 + 1, nSecLen)))
                    {
                        bFindSec = true;
                    }
                }
                else if (false == bFindKey)
                {
                    if ('[' == pNode->iX1[0])
                    {
                        bDone = true;
                        bFindKey = true;
                    }
                    else
                    {
                        if ((pNode->iX2 - pNode->iX1 == nKeyLen) && (0 == _strcmpi(lpKeyName, pNode->iX1, nKeyLen)))
                        {
                            char *lpszTmp = lpszPos;
                            while (0x0A != (--lpszPos)[0]);
                            fseek(pf, ++lpszPos - lpszTmp, SEEK_END);
                            lpszPos = (0 == pNode->iV2) ? pNode->iX2 : pNode->iV2;
                            if (NULL != pNode->pNext)
                            {
                                while (0x0A != (lpszPos++)[0]);
                            }
                            bDone = true;
                        }
                    }
                }

                if (NULL != pNode->pNext)
                {
                    fwrite(lpszPos, pNode->pNext->iX1 - lpszPos, 1, pf);
                    lpszPos = pNode->pNext->iX1;
                }
                else
                {
                    if (0 == pNode->iV2)
                    {
                        fwrite(lpszPos, pNode->iX2 - lpszPos, 1, pf);
                        lpszPos = pNode->iX2;
                    }
                    else
                    {
                        fwrite(lpszPos, pNode->iV2 - lpszPos, 1, pf);
                        lpszPos = pNode->iV2;
                    }
                }

                pNode = pNode->pNext;
            }

            nResult = (int)bDone;

            fclose(pf);

            if (bDone) InitFile(m_szFileName);
        }
    }

    return nResult;
}

vector<string> split(const string &s, const string &seperator)
{
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()){
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0){
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
			if (s[i] == seperator[x]){
				++i;
				flag = 0;
				break;
			}
		}

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0){
			for (string_size x = 0; x < seperator.size(); ++x)
			if (s[j] == seperator[x]){
				flag = 1;
				break;
			}
			if (flag == 0)
				++j;
		}
		if (i != j){
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}