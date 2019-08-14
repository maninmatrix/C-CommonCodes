#include "ThreadPool.h" 
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
ThreadPool<T>::ThreadPool(size_t Thread_num) :threadsNum_(Thread_num)
{
	isRunning_ = true;
}

ThreadPool<T>::~ThreadPool()
{
	stop();
	for (typename std::deque<T*>::iterator it = taskQueue_.begin(); it != taskQueue_.end(); ++it)
	{
		delete *it;
	}
	taskQueue_.clear();
}
template<typename... Args>
int ThreadPool<T>::AddTaskArgs(Args&&... args)
{
	{
		T *pThread = new T((args)...);
		//�Ӱ�ȫ������ֹ���̵߳������ͻ
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
}
int ThreadPool<T>::size()
{
	pthread_mutex_lock(&mutex_);
	int size = taskQueue_.size();
	pthread_mutex_unlock(&mutex_);
	return size;
}
T* ThreadPool<T>::take()
{
	T* task = NULL;
	while (!task)
	{
		pthread_mutex_lock(&mutex_);
		while (taskQueue_.empty() && isRunning_)
		{
			pthread_cond_wait(&condition_, &mutex_);
		}
		if (!isRunning_)
		{
			pthread_mutex_unlock(&mutex_);
			break;
		}
		else if (taskQueue_.empty())
		{
			pthread_mutex_unlock(&mutex_);
			continue;
		}
		pthread_mutex_unlock(&mutex_);
	}
	return task;
}
