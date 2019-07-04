#ifndef _SINGLE_TON_H_
#define _SINGLE_TON_H_

#include <pthread.h>
template <class T>
class singleton
{
protected:
	singleton(){};
private:
	singleton(const singleton&){};
	singleton& operator=(const singleton&){};
	static T* m_instance;
	static pthread_once_t m_once;
public:
	static void Init();
	static T* GetInstance();
};


template <class T>
void singleton<T>::Init()
{
	if (m_instance == NULL)
	{
		m_instance = new T();
	}
	
	
}

template <class T>
T* singleton<T>::GetInstance()
{
	pthread_once(&m_once, Init);
	return m_instance;
}

template <class T>
pthread_once_t singleton<T>::m_once = PTHREAD_ONCE_INIT;

template <class T>
T* singleton<T>::m_instance = NULL;
#endif
