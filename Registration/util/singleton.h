#ifndef _SINGLETON_H
#define _SINGLETON_H

#include <QMutex>

template<typename T>
class Singleton
{
public:
    static T* getInstance();
    static void destroy();

private:
    Singleton(Singleton const&){}
    Singleton& operator=(Singleton const&){}

protected:
    static T* m_instance;
    static QMutex m_mutex;
    Singleton(){ m_instance = static_cast <T*> (this); }
    ~Singleton(){}
};

template<typename T>
typename T* Singleton<T>::m_instance = 0;

template<typename T>
QMutex Singleton<T>::m_mutex;

template<typename T>
T* Singleton<T>::getInstance()
{
    QMutexLocker cLocker(&m_mutex);
    if(!m_instance)
    {
        Singleton<T>::m_instance = new T();
    }

    return m_instance;
}

template<typename T>
void Singleton<T>::destroy()
{
    QMutexLocker cLocker(&m_mutex);
    delete Singleton<T>::m_instance;
    Singleton<T>::m_instance = 0;
}

#endif
