#ifndef __NETMESSAGE_H__
#define __NETMESSAGE_H__

static const unsigned char IDENTIFY_VER = 0x05;
static const unsigned char MESSAGE_VER  = 0x03;//probuf协议static const unsigned char IDENTIFY_VER_PB = 0x06;//probuf协议static const unsigned char MESSAGE_VER_PB   = 0x04;

static const unsigned char ENCODE_NONE  = 0x00;
static const unsigned char ENCODE_AES   = 0x01;

static const unsigned short NM_KEEP_ALIVE = 0xF0F1;

static const unsigned long NAME_LEN     = 32;     //帐号长度
static const unsigned long PASSWD_LEN   = 33;     //密码长度

typedef struct tagMsgHeader
{
    unsigned char  identity;
    unsigned char  encode;
    unsigned short length;
    unsigned char  version;
    unsigned char  reserve;
    unsigned short type;
}MsgHeader, *PMsgHeader;

#define Header  MsgHeader header
#define HEADER  MsgHeader header
#define DEFAULT_ENCODE  ENCODE_NONE

#endif // __NETMESSAGE_H__
