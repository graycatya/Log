#include "CatLog.h"

CatLog::Log* CatLog::Log::_instance = nullptr;
std::thread* CatLog::Log::monitor_thread = nullptr;
std::mutex* CatLog::Log::mutex_log = new std::mutex;
std::mutex* CatLog::Log::mutex_write = new std::mutex;
std::condition_variable* CatLog::Log::condition = new std::condition_variable;
std::condition_variable* CatLog::Log::condition_thread = new std::condition_variable;
std::string CatLog::Log::file_path = "./Log.log";
std::queue<std::string>* CatLog::Log::Log_msg = new std::queue<std::string>;
bool CatLog::Log::stop = false;
bool CatLog::Log::Log_print = false;
std::string CatLog::Log::LEVE[6] = {"DEBUG", "INFO", "WARN", "ERROR", "ALARM", "FATAL"};