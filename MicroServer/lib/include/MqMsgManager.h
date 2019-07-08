#ifndef __MQ_MSG_MANAGER_H__
#define __MQ_MSG_MANAGER_H__

#include "Common/Common.h"
#include "cJSON.h"

class CJsonPack
{
public:
	CJsonPack(CJsonPack *pJsonParent = NULL);
	//
	CJsonPack(char *szJsonStr);
	CJsonPack(cJSON *pJson);
	~CJsonPack();
	//添加字符串
	void AddString(char *szName, char *szValue);
	//添加整形数值
	//void AddNumber(char *szName, int iNumber);
	//添加double类型数值
	void AddNumber(char *szName, double dNumber);
	//添加bool类型数值
	void AddBool(char *szName, bool bValue);
	//添加json子项
	void AddJsonItem(char *szName, CJsonPack *pJsonItem);
	//添加json子项
	void AddJsonItem(char *szName, CJsonPack &JsonItem);
	//json格式转字符串
	char* ToString();
	//json格式转字符串
	char* ToUnfmtString();
	//获取字符串
	char* GetString();
	//获取数值类型
	bool GetValueInt(char *szName, int& iNumber);
	bool GetValueDouble(char *szName, double& dNumber);
	//获取字符串类型
	bool GetValueStr(char *szName, char* szStrValue, int iLength);
	//获取Json对象
	bool GetJsonObj(char *szName, CJsonPack** pJsonObj);
	
	
private:
	CJsonPack(const CJsonPack& Temp);
	
private:
	cJSON* m_pRoot;
	
	//跟节点，最顶层为NULL，仅作析构时内存释放用
	CJsonPack *m_pParent;
	
	char *m_szJson;
};
#endif