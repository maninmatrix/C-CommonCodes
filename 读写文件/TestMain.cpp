#include <iostream>
#include <vector>
#include "IniFile.h"
using namespace std;

int main(int argc, char** argv) {
    CIniFile m_fileConifg;
    if (m_fileConifg.InitFile("test.cfg")) {
        //get int value
        int iValue = m_fileConifg.GetValueInt("Config", "intvalue", 0);
        cout << "iValue " << iValue << endl;
        iValue++;
        if (1 == m_fileConifg.SetValueInt("Config", "intvalue", iValue)) {
            cout << "set value success " << endl;
        }
    } else {
        cout << "InitFile failed" << endl;
    }

    //get str method1
    char szTmp[1024] = {0};
    m_fileConifg.GetValueStr("Config", "strvalue", szTmp, sizeof (szTmp));
    char* pToken = NULL;
    char* pTag = szTmp;
    vector<int> m_vecInt;
    m_vecInt.clear();
    while (pToken = strsep(&pTag, ",")) {
        m_vecInt.push_back(atoi(pToken));
    }
    for (auto istrvalue : m_vecInt) {
        cout << "istrvalue " << istrvalue << endl;
    }
    //get str method2
    char szTmp2[1024] = {0};
    m_fileConifg.GetValueStr("Config", "strvalue", szTmp2, sizeof (szTmp2));
    vector<int> m_vecInt2;
    m_vecInt2.clear();
    string str(szTmp2);
    vector<string> vecStr = split(str, ",");
    for (auto istrvalue : vecStr) {
        m_vecInt2.push_back(atoi(istrvalue.c_str()));
        cout << "siteid: " << istrvalue << endl;
    }
    //set str
    char szTmp3[1024] = {0};
    snprintf(szTmp3, sizeof (szTmp3), "WORLD");
    m_fileConifg.SetValueStr("Config", "hello", szTmp3);
    return 0;
}

