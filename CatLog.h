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

class CatLog
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
            MODEL model;                                //日志处理模式
            std::string module_name;         //模块名
            std::string msg;                               //日志
        };
        static void SetPath(std::string path)
        {
            m_sPath = path;
        }

        static CatLog* Instance()
        {
            if(_instance == nullptr)
            {
                std::unique_lock<std::mutex>lock(*m_pMutex);
                if(_instance == nullptr)
                {
                    _instance = new CatLog();
                    m_pModule->clear();
                    m_pConsumer_Thread = new std::thread([]{
                        for(;;)
                        {
                            //日志类消费者
                            {
                                std::unique_lock<std::mutex> lock(*m_pMutex_Consumer);
                                if(!m_pLogMsg->empty())
                                {
                                    CatLog::MODULE_NEWS* logs = m_pLogMsg->front();
                                    switch(logs->model)
                                    {
                                        case MODEL::PRINTF:
                                        {
                                            std::cout << logs->msg << std::endl;
                                            std::cout.flush();
                                            break;
                                        }
                                        case MODEL::WRITE_FILE:
                                        {
                                            std::string file_path = m_sPath + logs->module_name + ".log";
                                            std::ofstream outfile;
                                            outfile.open(file_path, std::ios::out | std::ios::app );
                                            outfile << logs->msg << std::endl;
                                            outfile.close();
                                            break;
                                        }
                                        case MODEL::NETWORK:
                                        {
                                            break;
                                        }
                                    }
                                    delete logs;
                                    logs = nullptr;
                                    m_pLogMsg->pop();
                                    m_pCondition->notify_one();
                                } else if(m_bThreadStop)
                                {
                                    break;
                                } else {
                                    m_pCondition_Consumer->wait(lock);
                                }
                            }
                        }
                    });
                }
            }
            return _instance;
        }
        static void Delete()
        {
            {
            std::unique_lock<std::mutex> lock(*m_pMutex);
            while(!m_pLogMsg->empty())
            {
                m_pCondition_Consumer->notify_one();
            }
                m_bThreadStop = true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            m_pCondition_Consumer->notify_one();
            m_pConsumer_Thread->join();
            if(m_pConsumer_Thread != nullptr)
            {
                delete m_pConsumer_Thread;
                m_pConsumer_Thread = nullptr;
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
            if(m_pCondition != nullptr)
            {
                delete m_pCondition;
                m_pCondition = nullptr;
            }
            if(m_pMutex_Consumer != nullptr)
            {
                delete m_pMutex_Consumer;
                m_pMutex_Consumer = nullptr;
            }
            if(m_pModule != nullptr)
            {
                delete m_pModule;
                m_pModule = nullptr;
            }
            if(m_pLogMsg != nullptr)
            {
                delete m_pLogMsg;
                m_pLogMsg = nullptr;
            }

        }

        static void InIt_Consumer_Thread(std::string modul_name, MODEL model = PRINTF)
        {
            if(m_pModule->count(modul_name) == 0)
            {
                m_pModule->insert(make_pair(modul_name, model));
            }
        }

        static void Delete_Consumer_Thread(std::string modul_name)
        {
            if(m_pModule->count(modul_name) != 0)
            {
                for(auto it = m_pModule->begin(); it != m_pModule->end(); it++)
                {
                    if(it->first == modul_name)
                    {
                        m_pModule->erase(it++);
                    }
                }
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
            std::unique_lock<std::mutex> lock(*m_pMutex);
            MODULE_NEWS* module_msg = new MODULE_NEWS;
            if(m_pModule->count(modul) != 0)
            {
                for(auto it = m_pModule->begin(); it != m_pModule->end(); it++)
                {
                    if(it->first ==modul)
                    {
                        module_msg->model = it->second;
                        //m_pModule->erase(it++);
                    }
                }
            } else
            {
                delete module_msg;
                module_msg = nullptr;
                return;
            }
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
            module_msg->module_name = modul;
            module_msg->msg = _msg;
            m_pLogMsg->push(module_msg);
            m_pCondition_Consumer->notify_one();
            auto now = std::chrono::system_clock::now() + std::chrono::milliseconds(500);
            m_pCondition->wait_until(lock, now);

        }

        /*
         * function: write log
         * modul(module name), lv(LEVEL)
        */
        template<typename... Args>
        void AddLog(std::string modul, LEVEL lv, const char* msg, Args... datas)
        {
            //日志类生产者
            std::unique_lock<std::mutex> lock(*m_pMutex);
            MODULE_NEWS* module_msg = new MODULE_NEWS;
            if(m_pModule->count(modul) != 0)
            {
                for(auto it = m_pModule->begin(); it != m_pModule->end(); it++)
                {
                    if(it->first ==modul)
                    {
                        module_msg->model = it->second;
                    }
                }
            } else
            {
                delete module_msg;
                module_msg = nullptr;
                return;
            }
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
            module_msg->module_name = modul;
            module_msg->msg = _msg;
            m_pLogMsg->push(module_msg);
            m_pCondition_Consumer->notify_one();
            auto now = std::chrono::system_clock::now() + std::chrono::milliseconds(500);
            m_pCondition->wait_until(lock, now);
        }

    protected:
    private:
        ~CatLog(){}
        CatLog(){}
    private:
        static CatLog* _instance;
        static std::string m_sPath;
        static std::mutex* m_pMutex;
        static std::condition_variable* m_pCondition;
        static std::mutex* m_pMutex_Consumer;
        static std::condition_variable* m_pCondition_Consumer;
        static std::thread* m_pConsumer_Thread;
        static std::map<std::string, MODEL> *m_pModule;
        static std::queue<MODULE_NEWS*> *m_pLogMsg;
        static std::string LEVE[6];
        static bool m_bThreadStop;
};

#endif 