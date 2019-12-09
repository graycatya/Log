#include "Log.h"

Log* Log::_instance = nullptr;
std::mutex* Log::m_pMutex = new std::mutex;
std::condition_variable * Log::m_pCondition = new std::condition_variable;
std::string Log::m_sPath = "./";
std::map<std::string, Log::MODULE_NEWS*> *Log::m_pLogMsg = new std::map<std::string, Log::MODULE_NEWS*>;
std::string Log::LEVE[6] = {"DEBUG", "INFO", "WARN", "ERROR", "ALARM", "FATAL"};