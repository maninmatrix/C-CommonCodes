#include "work_thread.h"

Thread::Thread()
{
	pthread_attr_init(&thread_attr);
	/* Set thread to detached */
	if(pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED)!=0)
	{
		//todo: Add error handle;
	}
	/* Set thread to bounded to a LWP */
	if(pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM) != 0)
	{
		//todo: add error handle;
	}
	/* Set thread schedule */
	if(pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER) != 0)
	{
		//todo: add error handle;
	}
	
}

Thread::Thread(PriorityDef priority)
{
	sched_param thread_sched_param;
	pthread_attr_init(&thread_attr);
	switch(priority)
	{
		case NORMAL:
			//thread_sched_param.sched_priority = sched_get_priority_min(SCHED_OTHER);
			thread_sched_param.sched_priority = 0;
			pthread_attr_setschedparam(&thread_attr,&thread_sched_param);
			break;
		case HIGH:
			//thread_sched_param.sched_priority = sched_get_priority_max(SCHED_OTHER);
			thread_sched_param.sched_priority =50;
			pthread_attr_setschedparam(&thread_attr,&thread_sched_param);
			break;
	}
	/* Set thread to detached */
	if(pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED)!=0)
	{
		//todo: Add error handle;
	}
	/* Set thread to bounded to a LWP */
	if(pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM) != 0)
	{
		//todo: add error handle;
	}
	/* Set thread schedule */
	if(pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER) != 0)
	{
		//todo: add error handle;
	}
}

Thread::Thread(DetachStateDef detach_state)
{
	pthread_attr_init(&thread_attr);
	if(detach_state == JOINABLE)
	{
		if(pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_JOINABLE)!=0)
			{
				//todo: Add error handle;
			}
	}
	else
	{
		if(pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED)!=0)
			{
				//todo: Add error handle;
			}	
	}
	/* Set thread to bounded to a LWP */
	if(pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM) != 0)
	{
		//todo: add error handle;
	}
	/* Set thread schedule */
	if(pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER) != 0)
	{
		//todo: add error handle;
	}
}

Thread::Thread(DetachStateDef detach_state,PriorityDef priority)
{
	sched_param thread_sched_param;
	pthread_attr_init(&thread_attr);
	if(detach_state == JOINABLE)
	{
		if(pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_JOINABLE)!=0)
			{
				//todo: Add error handle;
			}
	}
	else
	{
		if(pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED)!=0)
			{
				//todo: Add error handle;
			}	
	}
	switch(priority)
	{
		case NORMAL:
			//thread_sched_param.sched_priority = sched_get_priority_min(SCHED_OTHER);
			thread_sched_param.sched_priority = 0;
			pthread_attr_setschedparam(&thread_attr,&thread_sched_param);
			break;
		case HIGH:
			//thread_sched_param.sched_priority = sched_get_priority_max(SCHED_OTHER);
			thread_sched_param.sched_priority = 50;
			pthread_attr_setschedparam(&thread_attr,&thread_sched_param);
			break;
	}
	/* Set thread to bounded to a LWP */
	if(pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM) != 0)
	{
		//todo: add error handle;
	}
	/* Set thread schedule */
	if(pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER) != 0)
	{
		//todo: add error handle;
	}
}

Thread::~Thread()
{
	pthread_attr_destroy(&thread_attr);	
}

void Thread::Start()
{
	pthread_create(&thread_id,&thread_attr,(void *(*)(void *))ThreadMain,this);	
}

void Thread::ReStart()
{
	pthread_cancel(thread_id);	
	pthread_create(&thread_id,&thread_attr,(void *(*)(void *))ThreadMain,this);	
}

void Thread::ThreadMain(void* arg)
{
	Thread* thread = (Thread *)arg;
	/* Set cancel state */
	if(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0) != 0)
	{
		//todo: add error handle;
	}
	if(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0) != 0)
	{
		//todo: add error handle;
	}
	/* Start execute function */
	thread->Run();
}

void Thread::Stop()
{
	pthread_cancel(thread_id);	
}

pthread_t Thread::GetThreadId()
{
	return thread_id;
}
