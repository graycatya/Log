#pragma once
#include "CatLog_Message.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <stdexcept>
#include <fstream>
#include <future>
#include <queue>
#include <map>
#include <chrono>

namespace CATLOG
{

class CatLog
{
    public:
        static CatLog* Instance()
        {
            if(_instance == nullptr)
            {
                std::unique_lock<std::mutex>lock(*m_pMutex);
                if(_instance == nullptr)
                {
                    _instance = new CatLog();

                    m_pConsumer_Thread = new std::thread([]{
                        while(!m_bThreadStop)
                        {
                            std::unique_lock<std::mutex> lock(*m_pConsumer_Mutex);
                            if(!m_pLogMsg->empty())
                            {
                                std::function<void()> task;
                                task = std::move(m_pLogMsg->front());
                                m_pLogMsg->pop();
                                task();
                            } else
                            {
                                m_pCondition->wait(lock);
                            }
                        }
                    });

                }
            }
            return _instance;
        }

        static void Delete( void )
        {
            {
                std::unique_lock<std::mutex> lock(*m_pMutex);
                while(!m_pLogMsg->empty())
                {
                    m_pCondition->notify_one();
                }
                m_bThreadStop = true;
            }
            m_pCondition->notify_one();
            m_pConsumer_Thread->join();
            if(m_pConsumer_Thread != nullptr)
            {
                delete m_pConsumer_Thread;
                m_pConsumer_Thread = nullptr;
            }
            if(m_pMutex != nullptr)
            {
                delete m_pMutex;
                m_pMutex = nullptr;
            }
            if(m_pConsumer_Mutex != nullptr)
            {
                delete m_pConsumer_Mutex;
                m_pConsumer_Mutex = nullptr;
            }
            if(m_pCondition != nullptr)
            {
                delete m_pCondition;
                m_pCondition = nullptr;
            }
            if(m_pLogMsg != nullptr)
            {
                delete m_pLogMsg;
                m_pLogMsg = nullptr;
            }
            if(_instance != nullptr)
            {
                delete _instance;
                _instance = nullptr;
            }
        }

        template<class F, class... Args>
        static void enqueue(F&& f, Args&&... args)
        {
            using return_type = typename std::result_of<F(Args...)>::type;
            
            auto task = std::make_shared< std::packaged_task<return_type()> >(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                );
            {
                std::unique_lock<std::mutex>lock(*m_pConsumer_Mutex);
                m_pLogMsg->emplace([task](){ (*task)(); });
            }
            m_pCondition->notify_one();
        }

    protected:
    private:
        CatLog() {}
        ~CatLog() {}
        CatLog(const CatLog*) = delete;

        static CatLog* _instance;
        static std::mutex* m_pMutex;
        static std::mutex* m_pConsumer_Mutex;
        static std::condition_variable* m_pCondition;
        static std::thread* m_pConsumer_Thread;
        static std::queue<std::function<void()>> *m_pLogMsg;
        static bool m_bThreadStop;
};


void __Write_Log(std::string&& __FILE_PATH, std::string&& __LOG_MSG)
{
        CatLog::enqueue([__FILE_PATH, __LOG_MSG](){ 
            std::string file_path = __FILE_PATH + ".log"; 
            std::ofstream outfile; 
            outfile.open(file_path, std::ios::out | std::ios::app ); 
            outfile << __LOG_MSG << std::endl; 
            outfile.close(); 
        }); 
}

void __Write_Log(std::string&& __LOG_MSG)
{
        CatLog::enqueue([__LOG_MSG](){ 
            std::cout << __LOG_MSG << std::endl; 
            std::cout.flush(); 
        }); 
}

};