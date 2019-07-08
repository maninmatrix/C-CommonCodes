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
	//����ַ���
	void AddString(char *szName, char *szValue);
	//���������ֵ
	//void AddNumber(char *szName, int iNumber);
	//���double������ֵ
	void AddNumber(char *szName, double dNumber);
	//���bool������ֵ
	void AddBool(char *szName, bool bValue);
	//���json����
	void AddJsonItem(char *szName, CJsonPack *pJsonItem);
	//���json����
	void AddJsonItem(char *szName, CJsonPack &JsonItem);
	//json��ʽת�ַ���
	char* ToString();
	//json��ʽת�ַ���
	char* ToUnfmtString();
	//��ȡ�ַ���
	char* GetString();
	//��ȡ��ֵ����
	bool GetValueInt(char *szName, int& iNumber);
	bool GetValueDouble(char *szName, double& dNumber);
	//��ȡ�ַ�������
	bool GetValueStr(char *szName, char* szStrValue, int iLength);
	//��ȡJson����
	bool GetJsonObj(char *szName, CJsonPack** pJsonObj);
	
	
private:
	CJsonPack(const CJsonPack& Temp);
	
private:
	cJSON* m_pRoot;
	
	//���ڵ㣬���ΪNULL����������ʱ�ڴ��ͷ���
	CJsonPack *m_pParent;
	
	char *m_szJson;
};
#endif