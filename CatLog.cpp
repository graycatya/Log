#include "CatLog.h"

CatLog* CatLog::_instance = nullptr;
std::string CatLog::m_sPath = "./";
std::mutex* CatLog::m_pMutex = new std::mutex;
std::condition_variable * CatLog::m_pCondition = new std::condition_variable;
std::mutex* CatLog::m_pMutex_Consumer = new std::mutex;
std::condition_variable *CatLog::m_pCondition_Consumer = new std::condition_variable;
std::thread* CatLog::m_pConsumer_Thread = nullptr;
std::map<std::string, CatLog::MODEL>  *CatLog::m_pModule = new std::map<std::string, CatLog::MODEL> ;
std::queue<CatLog::MODULE_NEWS*> *CatLog::m_pLogMsg = new std::queue<CatLog::MODULE_NEWS*>;
std::string CatLog::LEVE[6] = {"DEBUG", "INFO", "WARN", "ERROR", "ALARM", "FATAL"};
bool CatLog::m_bThreadStop = false;