#include "ThreadPool.h" 
#include <stdio.h>
#include <assert.h>#include <algorithm>
#include <stdlib.h>template <typename T>
ThreadPool<T>::ThreadPool(size_t Thread_num) :threadsNum_(Thread_num)
{
	isRunning_ = true;
}
template <typename T>
ThreadPool<T>::~ThreadPool()
{
	stop();
	for (typename std::deque<T*>::iterator it = taskQueue_.begin(); it != taskQueue_.end(); ++it)
	{
		delete *it;
	}
	taskQueue_.clear();
}template <typename T>
template<typename... Args>
int ThreadPool<T>::AddTaskArgs(Args&&... args)
{	for (int i = 0; i < threadsNum_; i++)
	{
		T *pThread = new T((args)...);
		//加安全锁，防止主线程调用起冲突
		m_mutMapThread.lock();
		char szMsgQueName[20] = { 0 };
		sprintf(szMsgQueName, "word_thread_%d", i + 1);
		pThread->SetMsgQueName(szMsgQueName);
		taskQueue_.push_back(pThread);
		pThread->Start();
		m_mutMapThread.unlock();
		__log(_DEBUG, "CQueueServer::Start", "Create %s Success]", szMsgQueName);
	}
	return 0;
}
template <typename T>
void ThreadPool<T>::stop()
{
	if (!isRunning_)
	{
		return;
	}

	isRunning_ = false;
	pthread_cond_broadcast(&condition_);

	for (int i = 0; i < threadsNum_; i++)
	{
		pthread_join(threads_[i], NULL);
	}

	free(threads_);
	threads_ = NULL;

	pthread_mutex_destroy(&mutex_);
	pthread_cond_destroy(&condition_);
}template <typename T>
int ThreadPool<T>::size()
{
	pthread_mutex_lock(&mutex_);
	int size = taskQueue_.size();
	pthread_mutex_unlock(&mutex_);
	return size;
}template <typename T>
T* ThreadPool<T>::take()
{
	T* task = NULL;
	while (!task)
	{
		pthread_mutex_lock(&mutex_);
		while (taskQueue_.empty() && isRunning_)
		{			printf("taskQueue_.empty() isRunning_ \n");
			pthread_cond_wait(&condition_, &mutex_);
		}

		if (!isRunning_)
		{			printf("isRunning_ false\n");
			pthread_mutex_unlock(&mutex_);

			break;
		}
		else if (taskQueue_.empty())
		{			printf("taskQueue_.empty() \n");
			pthread_mutex_unlock(&mutex_);
			continue;
		}		task = *min_element(taskQueue_.begin(), taskQueue_.end(), [](T* a, T* b)->bool{return a->GetQueMsgNum() < b->GetQueMsgNum(); });
		pthread_mutex_unlock(&mutex_);
	}	printf("take success\n");
	return task;
}

