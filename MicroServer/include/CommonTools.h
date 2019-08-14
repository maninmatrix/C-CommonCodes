#ifndef _COMMON_TOOLS_H_
#define _COMMON_TOOLS_H_
#include <vector>
#include <string>
#include<sstream>
using namespace std;
class FishCommonTool
{
public:
	FishCommonTool();
	~FishCommonTool();
public:
	static vector<string> split(const string &s, const string &seperator);
	static bool GetRandIndex(const vector<int>& m_vector, int& iIndex);
public:
	template<class T>
	std::string toString(const T &t) {
		std::ostringstream oss;
		oss << t;
		return oss.str();
	}
private:

};
#endif