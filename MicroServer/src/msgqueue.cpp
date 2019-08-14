#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/errno.h>
#include "msgqueue.h"
#include "Common/Common.h"

MsgQueue:: MsgQueue(int size, int len)
{	
	/* initizlize int variable*/ 
	queue_size = size;
	max_msg_len = len+1;
	head = 0;               	
	tail = 0;				
	get_count = 0;
	put_count = queue_size;      	
	get_wait = 0;				
	put_wait = 0;			
	msg_items = new MsgItemDef[queue_size];
	for(int i = 0; i < queue_size; i++)
	{
		msg_items[i].len = 0;
		msg_items[i].data = new char[max_msg_len];
		msg_items[i].type = 0;
		memset(msg_items[i].data, 0, max_msg_len);
	}
	
	/* initialize  mutex and condition object */ 	
	pthread_cond_init(&cond_get, NULL);
	pthread_cond_init(&cond_put, NULL);
	pthread_mutex_init(&mt_common, NULL);
	pthread_mutex_init(&mt_get, NULL);
	pthread_mutex_init(&mt_put, NULL);	
	
	strcpy(m_szQueName,"Default");
	
	m_bLogFull = false;
}

MsgQueue:: ~MsgQueue()
{	
	for(int i = 0; i < queue_size; i++)
	{
		delete[] msg_items[i].data;
	}
		
	delete[] msg_items;
	pthread_mutex_destroy(&mt_get);
	pthread_mutex_destroy(&mt_put);
	pthread_mutex_destroy(&mt_common);
	pthread_cond_destroy(&cond_put);
	pthread_cond_destroy(&cond_get);
}

int MsgQueue::DeQueue(void **node, unsigned short &type, void *msg, int len)
{
	return DeQueue(node, type, msg, len, -1);
}

int MsgQueue::DeQueue(void **node, unsigned short &type, void *msg, int len, long outtime)
{
	int 		sig_flag = 0;
	int			msg_len;
	int			ret;
	static int 	count = 0;

	count++;

	if(msg == 0) return -1;	
	if (len > max_msg_len) return -1;
	if(len == 0) return -1;
	
	pthread_mutex_lock(&mt_get);   //这里锁住防止多个线程同时调用DeQueue函数导致冲突
	while(get_count <= 0)         //可供出列的元素没有了(队列里是空的，没有元素)
	{		
		get_wait++;              //正在等待的出列消息（线程）数+1
		if (outtime < 0)
		{
			//无限时等待EnQueue的完成信号，只要等到了当然就有元素可以取了，可以继续往下出列了,注意这个函数在进入的时候会解锁，这样就可以允许多个线程等待，然后执行到最后再重新加锁
			//也就是这里的get_wait变量其实等待DeQueue的线程数....
			pthread_cond_wait(&cond_get, &mt_get );
		}
		else
		{
			//限时等待了	
			struct timeval tp;
			struct timespec atime;
			gettimeofday(&tp,NULL);
			atime.tv_sec = tp.tv_sec + outtime;
			atime.tv_nsec = 0 ;
			ret = pthread_cond_timedwait(&cond_get, &mt_get,&atime);
			if (ret == ETIMEDOUT)
			{
				get_wait--;
				pthread_mutex_unlock(&mt_get); 
				return -2;
			}
		}
		get_wait--;   //正在等待的入列消息（线程）数-1,等待完成了当然要减掉了
	}
	
	get_count--;      //可供取的队列元素数-1
	pthread_mutex_unlock(&mt_get); 
	
	pthread_mutex_lock(&mt_common);  //锁住，防止多个线程操作队列元素
	msg_len =(len < msg_items[head].len)? len : msg_items[head].len;
	memcpy(msg, msg_items[head].data, msg_len);
	type = msg_items[head].type;
	*node = msg_items[head].node;
	msg_items[head].len = 0;
	memset(msg_items[head].data, 0, max_msg_len);
	head++;
	if(head >= queue_size) head = 0;
	pthread_mutex_unlock(&mt_common);
	
	
	pthread_mutex_lock(&mt_put);
	put_count++;                //队列可入的空间数目+1，
	if(put_wait > 0)            //EnQueue操作有等待数，
	{
		sig_flag = 1;
	}
	pthread_mutex_unlock(&mt_put);		
	if(sig_flag)
		pthread_cond_signal(&cond_put);  //给正在等待消息的EnQueue函数(线程)发送信号，通知可以继续执行函数取元素了,这里只会发送一次信号到最优先的线程
	return msg_len;
}

int MsgQueue::EnQueue(void *node, unsigned short type, void *msg, int len)
{
	return EnQueue(node, type, msg, len, 0);

}
int MsgQueue::EnQueue(void *node, unsigned short type, void *msg, int len, long outtime,bool bImportent)
{
	int sig_flag = 0;
	static int count = 0;
	int sleep_flag = 0;
	count++;
	if(len > max_msg_len) return -1;		
	if(msg == NULL) return -1;
	pthread_mutex_lock(&mt_put); //这里锁住防止多个线程同时调用EnQueue函数导致冲突
	
	if(put_count <= queue_size/2 && bImportent == false) //剩余空间只有一半了，非重要消息不入
	{
		pthread_mutex_unlock(&mt_put);	
		char *p = (char*)msg;		
		__log(_ERROR,"MsgQueue","EnQueue no importent space to in  Full [%s][%x][%x][%x]",m_szQueName,p[5],p[9],p[13]);
		m_bLogFull = true;
			
		return -2;
	}
	
	while(put_count <= 0)      //可供入列的空间没有了(队列满了)
	{
		pthread_mutex_unlock(&mt_put);	

		char *p = (char*)msg;		
		__log(_ERROR,"MsgQueue","EnQueue no space to in  Full [%s][%x][%x][%x]",m_szQueName,p[5],p[9],p[13]);
		m_bLogFull = true;
			
		return -2;
		
		/*袀摩原锟斤拷骚锟斤拷煤聥锟侥伙拷穴要狮變褏锟狡侊拷锟饺达拷ue锟节楋拷刹偶蛺锟絜ue锟斤拷私锟斤拷锟斤拷藝锟斤拷藝锟侥呈堢夯煤聥锟轿栵拷锟接凤拷锟截搭彸锟侥佀ｏ拷锟斤拷然硬为一锟斤拷锟斤拷艌敕撀愶拷锟叫燂拷炭锟阶★拷锟斤拷衻雸?put_wait++;		//纸諝锟饺达拷褟锟叫燂拷蹋锟剿?1
		if (outtime == 0)
		{
			//蠟袨时锟饺达拷ue锟脚嶊磯褏锟脚ｏ拷只要锟饺碉拷聥锟斤拷然锟轿撔恐撪秳锟秸间，锟绞捲纪愶拷色聬聥,注英症锟斤拷锟诫秳时锟津肌姐尭锟斤拷症夜锟酵渴捳斔愴窢锟斤拷痰却锟斤拷械锟截拷锟劫惵荚嬶拷//也锟轿娙曘伅锟矫皍t_wait锟藉伩扦实锟饺达拷ue锟脚忂惩婏拷			pthread_cond_wait(&cond_put, &mt_put);
		}
		else
		{
			//袨时锟饺达拷			struct timeval tp;
			struct timespec atime;
			int ret;
			gettimeofday(&tp,NULL);
			atime.tv_sec = tp.tv_sec + outtime;
			atime.tv_nsec = 0 ;
			ret = pthread_cond_timedwait(&cond_put, &mt_put,&atime);
			if (ret == ETIMEDOUT)
			{
				put_wait--;
				pthread_mutex_unlock(&mt_put); 
				return -2;
			}
		}
		put_wait--;//纸諝锟饺达拷褟锟叫燂拷蹋锟剿?1,锟饺达拷说锟饺灰拷锟缴?	*/
	}		
	put_count--;   //队列空间-1，
	pthread_mutex_unlock(&mt_put);	
	
	pthread_mutex_lock(&mt_common);		   //锁住，防止多个线程操作队列元素
	memcpy(msg_items[tail].data, msg, len);
	msg_items[tail].len = len;
	msg_items[tail].type = type;
	msg_items[tail].node = node;
	tail++;
	if(tail >= queue_size) tail = 0;
	pthread_mutex_unlock(&mt_common);
	
	pthread_mutex_lock(&mt_get);
	get_count++;                         //队列可取数目+1，
	printf("队列 %s数量: %d\n", GetQuename(), get_count);
	if(get_wait > 0)                     //DeQueue操作有等待数，
		sig_flag = 1;
	
	pthread_mutex_unlock(&mt_get);
	
	if (sig_flag)
	{
		pthread_cond_signal(&cond_get);   //给正在等待消息的DeQueue函数(线程)发送信号，通知可以继续执行函数取元素了。,这里只会发送一次信号到最优先的线程
	}
		
	return 0;
}
