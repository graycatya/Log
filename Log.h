/**
 * version： CatLog_0.2
 * 作者:greycatya
 * email: greycatya@163.com
 * 日志模块
 * 此版本功能：
 * 1. 日志标准输出设备
 * 2. 日志输出到文件
 * 3. 支持多线程
 * 4. 可设置文件路径
 * 5. 支持详细，普通信息输出(发布版本使用普通，调试版本使用详细)
 * 6. 模块分文件输出日志到设备
 * 
 */
#ifndef LOG_H
#define LOG_H

#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<iostream>
#include <fstream>
#include<string>
#include<queue>
#include<map>
#include<chrono>

//using namespace std;

class Log
{
    public:
        enum MODEL{
            PRINTF,
            WRITE_FILE,
            NETWORK
        };
        enum LEVEL{
            LEVEL_DEBUG,
            LEVEL_INFO,
            LEVEL_WARN,
            LEVEL_ERROR,
            LEVEL_ALARM,
            LEVEL_FATAL
        };
        struct MODULE_NEWS{
            MODEL model;                                          //日志处理模式
            std::thread* thread;                                  //模块线程
            std::queue<std::string> msg;                          //消息队列
            std::mutex* mutex_producer;                           //生产者lock
            std::condition_variable* condition_producer;          //生产者条件变量
            std::mutex* mutex_condition;                          //消费者lock
            std::condition_variable* condition_condition;         //消费者条件变量
            std::condition_variable* condition_msg;               //消息条件变量
            bool threadstop;                                      //线程停止标志
        };
        static void SetPath(std::string path)
        {
            m_sPath = path;
        }
        /* 
        * 
        */
        static Log* Instance()
        {
            if(_instance == nullptr)
            {
                std::unique_lock<std::mutex>lock(*m_pMutex);
                if(_instance == nullptr)
                {
                    _instance = new Log();
                }
            }
            return _instance;
        }
        static void Delete()
        {
            if(!m_pLogMsg->empty())
            {
                for(auto it = m_pLogMsg->begin(); it != m_pLogMsg->end(); it++)
                {
                    Delete_Consumer_Thread(it->first);
                }
                delete m_pLogMsg;
                m_pLogMsg = nullptr;
            }
            if(_instance != nullptr)
            {
                delete _instance;
                _instance = nullptr;
            }
            if(m_pMutex != nullptr)
            {
                delete m_pMutex;
                m_pMutex = nullptr;
            }
        }
        /*
        * 消费者线程
        */
        static void InIt_Consumer_Thread(MODEL model = PRINTF, std::string modul_name = "main")
        {
            std::unique_lock<std::mutex> lock(*m_pMutex);
            if(m_pLogMsg->count(modul_name) != 0)
            {
                return;
            }
            MODULE_NEWS* module = new MODULE_NEWS;
            module->model = model;
            module->mutex_producer = new std::mutex;
            module->condition_producer = new std::condition_variable;
            module->mutex_condition = new std::mutex;
            module->condition_condition = new std::condition_variable;
            module->condition_msg = new std::condition_variable;
            module->threadstop = false;
            m_pLogMsg->insert(std::make_pair(modul_name, module));
            module->thread = new std::thread([modul_name]{
                for(;;)
                {
                    //日志类消费者
                    {
                        std::map<std::string, MODULE_NEWS*>::iterator it = m_pLogMsg->find(modul_name);
                        if (it == m_pLogMsg->end())
                        {
                            return;
                        }
                        std::unique_lock<std::mutex> lock(*it->second->mutex_condition);
                        if(!it->second->msg.empty())
                        {
                            std::string msg = it->second->msg.front();
                            switch(it->second->model)
                            {
                                case MODEL::PRINTF:
                                {
                                    std::cout << msg << std::endl;
                                    std::cout.flush();
                                    break;
                                }
                                case MODEL::WRITE_FILE:
                                {
                                    std::string file_path = m_sPath + modul_name + ".log";
                                    std::ofstream outfile;
                                    outfile.open(file_path, std::ios::out | std::ios::app );
                                    outfile << msg << std::endl;
                                    outfile.close();
                                    break;
                                }
                                case MODEL::NETWORK:
                                {
                                    break;
                                }
                            }
                            it->second->msg.pop();
                            it->second->condition_producer->notify_one();
                        } else if(it->second->threadstop)
                        {
                            break;
                        } else {
                            it->second->condition_condition->wait(lock);
                        }
                    }
                }
            });
        }

        static void Delete_Consumer_Thread(std::string modul_name = "main")
        {
            if(m_pLogMsg->count(modul_name) == 0)
            {
                return;
            }
            std::unique_lock<std::mutex> locks(*m_pMutex);
            std::map<std::string, MODULE_NEWS*>::iterator it = m_pLogMsg->find(modul_name);
            if (it == m_pLogMsg->end())
            {
                return;
            }
            if( it->second->thread != nullptr)
            {
                {
                    std::unique_lock<std::mutex> lock(*it->second->mutex_condition);
                    it->second->threadstop = true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                it->second->condition_producer->notify_all();
                it->second->condition_condition->notify_all();
                it->second->thread->join();
                delete it->second->thread;
                it->second->thread = nullptr;

            }

            if(it->second->mutex_producer != nullptr)
            {
                delete it->second->mutex_producer;
                it->second->mutex_producer = nullptr;
            }

            if(it->second->mutex_condition != nullptr)
            {
                delete it->second->mutex_condition;
                it->second->mutex_condition = nullptr;
            }

            if(it->second->condition_producer != nullptr)
            {
                delete it->second->condition_producer;
                it->second->condition_producer = nullptr;
            }

            if(it->second->condition_condition != nullptr)
            {
                delete it->second->condition_condition;
                it->second->condition_condition = nullptr;
            }

            if(it->second->condition_msg != nullptr)
            {
                delete it->second->condition_msg;
                it->second->condition_msg = nullptr;
            }

            if(it->second != nullptr)
            {
                delete it->second;
                it->second = nullptr;
            }

        }

        /*
         * function: write log
         * modul(module name), lv(LEVEL), parameter: file(__FILE__), func(__FUNCTION__), line(__LINE__)
        */
        template<typename... Args>
        void AddLog_Detailed(std::string modul, LEVEL lv, std::string file, std::string func, int line,const char* msg, Args... datas)
        {
            //日志类生产者
            std::unique_lock<std::mutex> locks(*m_pMutex);
            if(m_pLogMsg->count(modul) == 0)
            {
                return;
            }
            std::map<std::string, MODULE_NEWS*>::iterator it = m_pLogMsg->find(modul);
            if (it == m_pLogMsg->end() && it->second->threadstop == true) 
            {
                return;
            }
            std::unique_lock<std::mutex> lock(*it->second->mutex_producer);
            std::string leve = LEVE[lv];
            auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            struct tm* ptm = localtime(&tt);
            char date[60] = {0};
            sprintf(date, "%d-%02d-%02d | %02d:%02d:%02d",
                ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
                ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
            char Amsg[1024] = {0};
            sprintf(Amsg, msg, datas...);
            std::string _msg = std::string(date) + " | " + leve + " | " + "Msg: " + std::string(Amsg) + " | file: " + file + " | func: " + func + " | Lent: " + std::to_string(line);
            it->second->msg.emplace(_msg);
            it->second->condition_condition->notify_one();
            it->second->condition_producer->wait(lock);
        }

        /*
         * function: write log
         * modul(module name), lv(LEVEL)
        */
        template<typename... Args>
        static void AddLog(std::string modul, LEVEL lv, const char* msg, Args... datas)
        {
            //日志类生产者
            std::unique_lock<std::mutex> locks(*m_pMutex);
            if(m_pLogMsg->count(modul) == 0)
            {
                return;
            }
            std::map<std::string, MODULE_NEWS*>::iterator it = m_pLogMsg->find(modul);
            if (it == m_pLogMsg->end() && it->second->threadstop == true)
            {
                return;
            }
            std::unique_lock<std::mutex> lock(*it->second->mutex_producer);
            std::string leve = LEVE[lv];
            auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            struct tm* ptm = localtime(&tt);
            char date[60] = {0};
            sprintf(date, "%d-%02d-%02d | %02d:%02d:%02d",
                ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
                ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
            char Amsg[1024] = {0};
            sprintf(Amsg, msg, datas...);
            std::string _msg = std::string(date) + " | " + leve + " | " + "Msg: " + std::string(Amsg);
            it->second->msg.emplace(_msg);
            it->second->condition_condition->notify_one();
            it->second->condition_producer->wait(lock);
        }

    protected:
    private:
        ~Log(){}
        Log(){}
    private:
        static Log* _instance;
        static std::string m_sPath;
        static std::mutex* m_pMutex;
        static std::condition_variable* m_pCondition;
        static std::map<std::string, MODULE_NEWS*> *m_pLogMsg;
        static std::string LEVE[6];
};

#endif //LOG_H"