#ifndef _REDIS_SERVER_H_
#define _REDIS_SERVER_H_

#include "Runnable.h"
#include "Thread.h"
#include <string>
#include <map>
#include <deque>
#include "xRedisClient.h"
#include "cJson/cJSON.h"
#include "BaseMsg.h"
using namespace std;
using namespace xrc;
struct RedisReq
{
	std::string key;
	std::string data;
};


enum {
	CACHE_TYPE_1, 
	CACHE_TYPE_2,
	CACHE_TYPE_MAX,
};
//玩家的分数存储 分数.时间差        分数：玩家的分数，  时间差：START_TIME-记录时间 分数相同时进入越早，排行越向前
//开始的时间戳
#define END_TIME                         9999999999



class CRedisServer : public SGLib::IRunnable
{
public:
	CRedisServer();
	~CRedisServer();

    bool Start(char *szIP, unsigned int port, char  *passwd, int timeout);
	void Stop();
	
	virtual void Run();

public:
	static void PushRedisRequest(const std::string &key, const std::string &data);
	static void PushRedisRequestUserID(int userid);
	static bool PopRedisRequest(RedisReq &req);
public:
	//严格排行榜
	//记录或覆盖玩家的分数
	void  ZaddPlayerSCore(int iUserID, int iScore, int iCreateTime);
	//更新玩家的最高分数
	void UpdatePlayerHighScore(int iUserID, int iScore, int iCreateTime);
	//获取前N名的玩家的排行榜
	void  PrintfRanklist(int iNum, vector<PlayerInfo>& m_vRankList);
	//获取某个玩家排名
	bool	GetPlayerRank(int iUserID, int64_t& m_iRank);
	//获取排在某个玩家前N名的玩家
	bool  GetPlayersBefore(int iUserID, int N);
	//获取排在某个玩家后N名的玩家
	bool  GetPlayersAfter(int iUserID, int N);
	string ConvertScoreToStr(int iScore, int iTime);
private:
	static xRedisClient xRedis;

private:
	SGLib::CThread *m_thread;
	bool m_isStop;

	static SGLib::CLock m_redisReqQueueLock;
	static std::deque<RedisReq> m_redisReqQueue;

};

#endif

