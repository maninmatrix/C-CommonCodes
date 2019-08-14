#include "CommonTools.h"
#include "Common/Common.h"
FishCommonTool::FishCommonTool()
{

}

FishCommonTool::~FishCommonTool()
{

}

vector<string> FishCommonTool::split(const string &s, const string &seperator)
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
//从vector中随机一个值的下标出来
bool FishCommonTool::GetRandIndex(const vector<int>& m_vector, int& iIndex)
{
	int iSum = 0;
	for (auto i : m_vector)
	{
		iSum += i;
	}
	int irand = rand() % iSum;
	printf("irand %d iSum %d\n", irand, iSum);
	int m_iIndex = -1;
	int iAddSum = 0;
	for (int j = 0; j < m_vector.size(); j++)
	{
		iAddSum += m_vector[j];
		if (irand < iAddSum)
		{
			m_iIndex = j;
			break;
		}
	}
	if (m_iIndex >= 0 && m_iIndex < m_vector.size())
	{
		iIndex = m_iIndex;
		printf("****************iIndex %d\n", iIndex);
		return true;
	}
	else
	{
		return false;
		__log(_ERROR, __FUNCTION__, "iIndex error irand %d, iSum %d", irand, iSum);
	}
}

