#pragma once  
#include <deque>  
#include <string>  
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "work_thread.h"
// ʹ��C++98 ���Թ淶ʵ�ֵ��̳߳أ� �������������ÿһ��job����Task�̳���Ķ���
//ʵ���̳߳� ��Ҫ��������
//�̳߳���
//���ܻ���
//������
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