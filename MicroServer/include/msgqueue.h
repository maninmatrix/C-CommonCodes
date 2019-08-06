#ifndef __MSGQUEUE_H_
#define __MSGQUEUE_H_

#define QUEUE_TIMEOUT	5
#include <pthread.h>
#include <string.h>

typedef struct
{
	void*			node;
	unsigned short	type;

	void*			data;

	long			len;

}MsgItemDef;

class MsgQueue
{
public:/* for test*/

	int max_msg_len;        /* max length of message */

	int queue_size;         /* size of queue */

	int head;               /* head of recle queue */

	int tail;			  	/* tail of recle queue */

	int get_count;          /* number of message able to get */

	int put_count;      	/* number of message able to get */	

	int get_wait;			/* number of thread waiting for getting message */

	int put_wait;			/*  number of thread waiting for getting message */

	pthread_cond_t cond_get ;

	pthread_cond_t cond_put;

	pthread_mutex_t  mt_common;

	pthread_mutex_t  mt_get;

	pthread_mutex_t  mt_put;

	MsgItemDef		*msg_items;

	char	m_szQueName[32];

	void SetQueName(char *szName)
	{

		strcpy(m_szQueName,szName);

	}
	char* GetQuename()
	{
		return m_szQueName;
	}
	bool m_bLogFull;
	int GetGetCount(){return get_count;}

public:



	MsgQueue(int size, int len);

	~MsgQueue();

	int DeQueue(void **node, unsigned short &type, void *msg, int len);    /* get a message into msg from queue*/

	int DeQueue(void **node, unsigned short &type, void *msg, int len, long outtime);

	//int EnQueue(void *msg, int len);    /* put a msg message into queue */

	int EnQueue(void *node, unsigned short type, void *msg, int len);

	int EnQueue(void *node, unsigned short type, void *msg, int len, long outtime,bool bImportent = true);
};
#endif  //__MSGQUEUE_H_



