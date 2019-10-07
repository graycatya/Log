/**
 * version： CatLog_0.1
 * 作者:greycatya
 * email: greycatya@163.com
 * 日志模块
 * 此版本功能：
 * 1. 日志标准输出设备
 * 2. 日志输出到文件
 * 3. 支持多线程
 * 4. 可设置文件路径
 * 5. 支持详细，普通信息输出(发布版本使用普通，调试版本使用详细)
 */

#ifndef CATLOG_H
#define CATLOG_H

#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<iostream>
#include <fstream>
#include<string>
#include<queue>
#include<chrono>

#define CATLOG_DETAILED(lv, msg, ...) CatLog::Log::AddLog_Detailed(lv, __FILE__, __FUNCTION__, __LINE__, msg, __VA_ARGS__)
#define CATLOG(lv, msg, ...) CatLog::Log::AddLog(lv, msg, __VA_ARGS__)

namespace CatLog
{

using namespace std;

class Log
{
    public:
        enum LEVEL{
            LEVEL_DEBUG,
            LEVEL_INFO,
            LEVEL_WARN,
            LEVEL_ERROR,
            LEVEL_ALARM,
            LEVEL_FATAL
        };
        static Log* Instance()
        {
            if(_instance == nullptr)
            {
                std::unique_lock<std::mutex> lock(*mutex_log);
                if(_instance == nullptr)
                {
                    _instance = new Log();
                    monitor_thread = new std::thread([]
                    {
                        for(;;)
                        {
                            {
                                std::unique_lock<std::mutex> lock(*mutex_write);
                                if(!Log_msg->empty())
                                {
                                    std::string msg = Log_msg->front();
                                    if(!Log_print)
                                    {
                                        std::ofstream outfile;
                                        outfile.open(file_path, std::ios::out | std::ios::app );
                                        outfile << msg << std::endl;
                                        outfile.close();
                                    }
                                    else
                                    {
                                        std::cout << msg << std::endl;
                                    }
                                    Log_msg->pop();
                                    condition->notify_one();
                                } else if(stop)
                                {
                                    break;
                                } else
                                {
                                    condition_thread->wait(lock);
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
            if(monitor_thread != nullptr)
            {
                while(!Log_msg->empty()){ condition->notify_one(); };
                {
                    std::unique_lock<std::mutex> lock(*mutex_write);
                    stop=true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                condition->notify_all();
                condition_thread->notify_all();
                monitor_thread->join();
                delete monitor_thread;
                monitor_thread = nullptr;
            }
            if(mutex_log != nullptr)
            {
                delete mutex_log;
                mutex_log = nullptr;
            }
            if(mutex_write != nullptr)
            {
                delete mutex_write;
                mutex_write = nullptr;
            }
            if(Log_msg != nullptr)
            {
                delete Log_msg;
                Log_msg = nullptr;
            }
            if(condition != nullptr)
            {
                delete condition;
                Log_msg = nullptr;
            }
            if(condition_thread != nullptr)
            {
                delete condition_thread;
                condition_thread = nullptr;
            }
            if(_instance != nullptr)
            {
                delete _instance;
                Log_msg = nullptr;
            }
        }

        static void SetLogPath(std::string path)
        {
            file_path = path;
        }

        /*
         * function: write log
         * lv(LEVEL), parameter: file(__FILE__), func(__FUNCTION__), line(__LINE__)
        */
        template<typename... Args>
        static void AddLog_Detailed(LEVEL lv, string file, string func, int line,const char* msg, Args... datas)
        {
            std::unique_lock<std::mutex> lock(*mutex_write);
            std::string leve = LEVE[lv];
            auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            struct tm* ptm = localtime(&tt);
            char date[60] = {0};
            sprintf(date, "%d-%02d-%02d | %02d:%02d:%02d",
                ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
                ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
            char Amsg[1024] = {0};
            sprintf(Amsg, msg, datas...);
            std::string _msg = std::string(date) + " | " + leve + " | " + "Msg: " + string(Amsg) + " | file: " + file + " | func: " + func + " | Lent: " + std::to_string(line);
            Log_msg->emplace(_msg);
            condition_thread->notify_one();
            condition->wait(lock);
        }

        template<typename... Args>
        static void AddLog(LEVEL lv, const char* msg, Args... datas)
        {
            std::unique_lock<std::mutex> lock(*mutex_write);
            std::string leve = LEVE[lv];
            auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            struct tm* ptm = localtime(&tt);
            char date[60] = {0};
            sprintf(date, "%d-%02d-%02d | %02d:%02d:%02d",
                ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
                ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
            char Amsg[1024] = {0};
            sprintf(Amsg, msg, datas...);
            std::string _msg = std::string(date) + " | " + leve + " | " + "Msg: " + string(Amsg);
            Log_msg->emplace(_msg);
            condition_thread->notify_one();
            condition->wait(lock);
        }

        static void SetLog_Print(bool bol)
        {
            Log_print = bol;
        }

        static string GetVersion()
        {
            return string("CatLog_0.1");
        }

    protected:
    private:
        ~Log(){}
        Log(){}
    private:
        static Log* _instance;
        static std::mutex* mutex_log;
        static std::mutex* mutex_write;
        static std::condition_variable* condition;
        static std::condition_variable* condition_thread;
        static std::thread* monitor_thread;
        static std::queue<std::string>* Log_msg;
        static std::string  file_path;
        static std::string LEVE[6];
        static bool stop;
        static bool Log_print;

};


}


#endif 