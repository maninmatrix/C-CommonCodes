#ifndef BASE_MSG_H_
#define BASE_MSG_H_
#define SERVER_NAME "TestServer"
#define CONFIG_FILE SERVER_NAME ".cfg"
#define LOG_FILE    SERVER_NAME ".log"
#define  GAME_SERVER_ID_NOTICE  0xFEFE
enum  S_S_MSG_TYPE
{
	S_S_SAYHI_INFO = 0x1901
};
#pragma pack(push,4)
//S_S_SAYHI_INFO = 0x1901
struct SS_SayHi
{
	HEADER;
	SS_SayHi()
	{
		memset(this, 0, sizeof(*this));
	}
	int iSendNum;				//
};
typedef struct tagGameServerIdNotice
{
	Header;
	unsigned int ulServerID;
}GameServerIdNotice, *PGameServerIdNotice;
#pragma pack()
#endif