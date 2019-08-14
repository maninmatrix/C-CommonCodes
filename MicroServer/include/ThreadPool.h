#pragma once  
#include <deque>  
#include <string>  
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "work_thread.h"
// 使用C++98 语言规范实现的线程池： 面向对象做法，每一个job都是Task继承类的对象
//实现线程池 需要的三个类
//线程池类
//功能基类
//功能类
template <typename T>
class ThreadPool{
public:
	ThreadPool(size_t Thread_num = 6);
	~ThreadPool();
public:
	//size_t addTask(T *task);
	void   stop();
	int    size();
	

public:	// add task args	template<typename... Args>
	int AddTaskArgs(Args&&... args);
	// gei one thread back
	T*  take();
private:
	ThreadPool& operator=(const ThreadPool&);
	ThreadPool(const ThreadPool&);

private:
	volatile  bool              isRunning_;
	int                         threadsNum_;
	pthread_t*                  threads_;

	std::deque<T*>           taskQueue_;	CMutex						m_mutMapThread;
	pthread_mutex_t             mutex_;
	pthread_cond_t              condition_;
};