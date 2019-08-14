#include "RedisServer.h"
#include "Common/Common.h"
#include "stdlib.h"
#include <string.h>

using namespace SGLib;
using namespace std;

CLock CRedisServer::m_redisReqQueueLock;
deque<RedisReq> CRedisServer::m_redisReqQueue;
xRedisClient CRedisServer::xRedis;

// AP Hash Function
unsigned int APHashRedis(const char *str) {
    unsigned int hash = 0;
    int i;
    for (i = 0; *str; i++) {
        if ((i & 1) == 0) {
            hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
        }
        else {
            hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
        }
    }
    return (hash & 0x7FFFFFFF);
}
CRedisServer::CRedisServer() : 
	m_thread(NULL),
	m_isStop(false)
{
	
}

CRedisServer::~CRedisServer()
{
	Stop();
}

bool CRedisServer::Start(char *szIP, unsigned int port, char  *passwd, int timeout)
{
	xRedis.Init(CACHE_TYPE_MAX);

    RedisNode RedisList1[1] =
    {
        { 0, szIP, port, passwd, 8, timeout, 0 }
    };


    __log(_DEBUG, __FUNCTION__ , "Start RedisList1 %d, %s, %d, %s, %d, %ld, %d",
					RedisList1[0].dbindex,
					RedisList1[0].host.c_str(),
					//RedisList1[0].host,
					RedisList1[0].port,
					RedisList1[0].passwd.c_str(),
					//RedisList1[0].passwd,
					RedisList1[0].poolsize,
					RedisList1[0].timeout, 
					RedisList1[0].role);
	if (xRedis.ConnectRedisCache(RedisList1, sizeof(RedisList1) / sizeof(RedisNode), 1, CACHE_TYPE_1))
	//if (xRedis.ConnectRedisCache(RedisList1, 1, CACHE_TYPE_1))
	{
		
	}
	else
	{
		//__log( _ERROR, "CRedisServer", "CRedisServer::Start ip:port[%s:%d] failed", RedisList1->host, RedisList1->port );
		return false;
	}

	m_thread = new CThread( this );

	if( !m_thread )
	{
		return false;
	}
	m_thread->Start();

	//__log(_DEBUG, "CRedisServer", "CRedisServer::Start ip:port[%s:%d]", RedisList1->host, RedisList1->port);

	return true;
}

void CRedisServer::Stop()
{
	m_isStop = true;
	if( m_thread )
	{
		m_thread->Stop();
		SAFE_DELETE( m_thread );
	}

	__log( _DEBUG, "CRedisServer", "CRedisServer::Stop" );
}

void CRedisServer::Run()
{
	//printf( "Starting on port!!!! \n" );

	while( !m_isStop )
	{
		//保持连接
		static time_t timeKeepAliva = time(NULL);
		time_t tmNow = time(NULL);
		if(tmNow - timeKeepAliva >= 30)
		{
			//printf( "CRedisServer Time Keepalive!!!!!!\n" );
			xRedis.Keepalive();
			timeKeepAliva = tmNow;
		}
		//去队列里面取
		RedisReq req;
		PopRedisRequest(req);
		sleep(3);
	}

	xRedis.Release();
}


void CRedisServer::PushRedisRequest(const std::string &key, const std::string &data)
{
	RedisReq req = { key, data };
	CGuardLock<CLock> g(m_redisReqQueueLock);
	m_redisReqQueue.push_back( req );
}

bool CRedisServer::PopRedisRequest(RedisReq &req)
{
	CGuardLock<CLock> g(m_redisReqQueueLock);
	while( !m_redisReqQueue.empty() )
	{
		req = m_redisReqQueue.front();
		
		RedisDBIdx dbi(&CRedisServer::xRedis);
		dbi.CreateDBIndex(req.key.c_str(), APHashRedis, CACHE_TYPE_1);

        //bool bRet = CRedisServer::xRedis.setex(dbi, req.key.c_str(), 60*60*10, req.data.c_str());
                bool bRet = CRedisServer::xRedis.set(dbi, req.key.c_str(), req.data.c_str());
		
		if(bRet)
		{
			__log(_DEBUG, "PopRedisRequest", "队列大小[%d], 获取key[%s], value[%s] \r\n", m_redisReqQueue.size(), req.key.c_str(), req.data.c_str());
		}
		else 
		{
			__log(_ERROR, "PopRedisRequest", "error [%s] \r\n", dbi.GetErrInfo());
		}

		m_redisReqQueue.pop_front();

		/*return bRet;*/
	}
	return false;
}

void CRedisServer::PushRedisRequestUserID(int userid)
{
	char szRedisInfo[255] = {0};

	char szUserID[255] = {0};
	sprintf(szUserID, "%d_Mobile_UserInfo", userid);

	const char *key = szUserID;
	RedisDBIdx dbi(&xRedis);
	dbi.CreateDBIndex(key, APHashRedis, CACHE_TYPE_1);

	string strValue;
	bool bRet = false;
	bRet = xRedis.get(dbi, key, strValue);
	if (bRet)
	{
		__log(_DEBUG, __FUNCTION__ ,"PushRedisRequestUserID::%s \r\n", strValue.c_str());
	}
	else 
	{
		//没有找到这个玩家的信息
		__log(_WARN, __FUNCTION__ ,"PushRedisRequestUserID::error [%s] \r\n", dbi.GetErrInfo());
	}

	//解析json
	cJSON * pJson = cJSON_Parse(strValue.c_str());
	if(NULL == pJson)                                                                                         
	{
		// parse faild, return
		__log(_WARN, __FUNCTION__, "userid[%s] value[%s] cJSON_Parse error!!", szUserID, strValue.c_str());
		//return ;
		strValue.clear();
		pJson = cJSON_CreateObject();

		if(NULL == pJson)
		{
			//error happend here
			__log(_WARN, __FUNCTION__, "userid[%s] cJSON_CreateObject error!!", szUserID);
			return;
		}
	}

	// get string from json
	cJSON * pSub = cJSON_GetObjectItem(pJson, "userID");

	if(NULL == pSub)
	{
		//get object userid faild
		__log(_WARN, __FUNCTION__, "userid[%s] get object userid faild error!!", szUserID);
		//插入 szUserid
		cJSON_AddNumberToObject(pJson, "userID", userid);
	}

	//printf("obj_1 : %s\n", cJSON_Print(pJson));

	// get szTokenName from json
	pSub = cJSON_GetObjectItem(pJson, "szTokenName");

	if(NULL == pSub)
	{
		// get szTokenName from json faild
		__log(_WARN, __FUNCTION__, "userid[%s] get object szTokenName faild error!!", szUserID);
	}
	else
	{
		cJSON_DeleteItemFromObject(pJson, "szTokenName");
	}

	//插入 szUserid
	cJSON_AddStringToObject(pJson, "szTokenName", "szTokenMD5");


	//printf("obj_2 : %s\n", cJSON_Print(pJson));

	memset(szRedisInfo, 0, sizeof(szRedisInfo));
	
	char *szJson = cJSON_Print(pJson);
	if(szJson)
	{
		strncpy(szRedisInfo, szJson, sizeof(szRedisInfo));
		free(szJson);
	}
	
	cJSON_Delete(pJson);

	PushRedisRequest(szUserID, szRedisInfo);
}

void CRedisServer::ZaddPlayerSCore(int iUserID, int iScore, int iCreateTime)
{

	char szUserID[255] = { 0 };
	sprintf(szUserID, "%d_%d", iUserID, iCreateTime);
	int64_t retVal = 0;
	VALUES vVal;
	vVal.push_back(ConvertScoreToStr(iScore, iCreateTime));
	vVal.push_back(toString(iUserID));

	
	const char *key = "ranklist";
	RedisDBIdx dbi(&xRedis);
	bool bRet = dbi.CreateDBIndex(key, APHashRedis, CACHE_TYPE_1);
	if (bRet) {
		if (xRedis.zadd(dbi, key, vVal, retVal)) {
			printf("add success \r\n");
		}
		else {
			printf("add error size[%d][%s] \r\n", vVal.size(), dbi.GetErrInfo());
		}
	}
}

void CRedisServer::PrintfRanklist(int iNum, vector<PlayerInfo>& m_vRankList)
{
	VALUES vVal;
	const char *key = "ranklist";
	RedisDBIdx dbi(&xRedis);
	bool bRet = dbi.CreateDBIndex(key, APHashRedis, CACHE_TYPE_1);
	if (bRet) {
		if (xRedis.zrevrange(dbi, key, 0, iNum, vVal)) {
			printf("get range success \r\n");
		}
		else {
			printf("add error size[%d][%s] \r\n", vVal.size(), dbi.GetErrInfo());
		}
	}
	//printf("value size: %d\n", vVal.size());
	vector<PlayerInfo> mInfo;
	for (auto i : vVal)
	{
		string iScore;
		//printf("value: %s\n", i.c_str());
		xRedis.zscore(dbi, key, i, iScore);
		//printf("iScore: %s\n", iScore.c_str());
		mInfo.emplace_back(atoi(i.c_str()), atoi(iScore.c_str()));
	}

	for (auto info : mInfo)
	{
		printf("UID %d, value: %d\n", info.iUID, info.iScore);
	}
	m_vRankList = mInfo;
}

bool CRedisServer::GetPlayerRank(int iUserID, int64_t& m_iRank)
{
	const char *key = "ranklist";
	RedisDBIdx dbi(&xRedis);
	bool bRet = dbi.CreateDBIndex(key, APHashRedis, CACHE_TYPE_1);
	if (bRet) {
		int64_t iRank = 0;
		if (xRedis.zrank(dbi, key, toString(iUserID), iRank)) {
			printf("get palyer %d rank[%d] success \r\n", iUserID, iRank);
			 m_iRank = iRank;
			 return true;
		}
		else {
			printf("get palyer %d rank failed\r\n", iUserID);
		}
	}
	return false;
}

void CRedisServer::UpdatePlayerHighScore(int iUserID, int iScore, int iCreateTime)
{
	const char *key = "ranklist";
	RedisDBIdx dbi(&xRedis);
	bool bRet = dbi.CreateDBIndex(key, APHashRedis, CACHE_TYPE_1);
	string S_Score;
	if (bRet) {
		int64_t iRank = 0;
		if (xRedis.zscore(dbi, key, toString(iUserID), S_Score)) {
			printf("get palyer %d score[%s] success \r\n", iUserID, S_Score.c_str());
			if (iScore > atoi(S_Score.c_str()))
			{
				ZaddPlayerSCore(iUserID, iScore, iCreateTime);
			}
			else
			{
				printf("get palyer %d score[%d] higher than iScore[%d]\r\n", iUserID, atoi(S_Score.c_str()), iScore);
			}
			
		}
		else {
			printf("get palyer %d score failed\r\n", iUserID);
			ZaddPlayerSCore(iUserID, iScore, iCreateTime);
		}
	}
}

bool CRedisServer::GetPlayersAfter(int iUserID, int N)
{
	int64_t mRank = 0;
	if (GetPlayerRank(iUserID, mRank))
	{
		VALUES vVal;
		const char *key = "ranklist";
		RedisDBIdx dbi(&xRedis);
		bool bRet = dbi.CreateDBIndex(key, APHashRedis, CACHE_TYPE_1);
		if (bRet) {
			int64_t iRank = 0;
			if (xRedis.zrange(dbi, key, mRank, mRank + N,  vVal)) {
				vector<PlayerInfo> mInfo;
				printf("playerlist after\n");
				for (auto i : vVal)
				{
					string iScore;
					//printf("value: %s\n", i.c_str());
					xRedis.zscore(dbi, key, i, iScore);
					//printf("iScore: %s\n", iScore.c_str());
					mInfo.emplace_back(atoi(i.c_str()), atoi(iScore.c_str()));
				}

				for (auto info : mInfo)
				{
					printf("UID %d, after value: %d\n", info.iUID, info.iScore);
				}
				return true;
			}
		}
		
	}
	else
	{
		return false;
	}
	
}

bool CRedisServer::GetPlayersBefore(int iUserID, int N)
{
	int64_t mRank = 0;
	if (GetPlayerRank(iUserID, mRank))
	{
		VALUES vVal;
		const char *key = "ranklist";
		RedisDBIdx dbi(&xRedis);
		bool bRet = dbi.CreateDBIndex(key, APHashRedis, CACHE_TYPE_1);
		if (bRet) {
			int64_t iRank = 0;
			if (xRedis.zrange(dbi, key, mRank - N, mRank, vVal)) {
				vector<PlayerInfo> mInfo;
				printf("palyer list before\n");
				for (auto i : vVal)
				{
					string iScore;
					//printf("value: %s\n", i.c_str());
					xRedis.zscore(dbi, key, i, iScore);
					//printf("iScore: %s\n", iScore.c_str());
					mInfo.emplace_back(atoi(i.c_str()), atoi(iScore.c_str()));
				}

				for (auto info : mInfo)
				{
					printf("UID %d, before value: %d\n", info.iUID, info.iScore);
				}
				return true;
			}
		}

	}
	else
	{
		return false;
	}
}

std::string CRedisServer::ConvertScoreToStr(int iScore, int iTime)
{
	stringstream  m_strScore;
	m_strScore << iScore;
	m_strScore << ".";
	m_strScore << END_TIME - iTime;
	string strResult;
	 m_strScore >> strResult;
	return strResult;
}













