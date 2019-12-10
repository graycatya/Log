# Log version: CatLog_0.2

```
#include "CatLog.h"
#include <iostream>
#include <thread>
int main()
{

    CatLog* Logs = CatLog::Instance();
    CatLog::InIt_Consumer_Thread("main", CatLog::WRITE_FILE);
    CatLog::InIt_Consumer_Thread("demo", CatLog::WRITE_FILE);
    CatLog::InIt_Consumer_Thread("cat", CatLog::WRITE_FILE);
    CatLog::InIt_Consumer_Thread("bbq", CatLog::WRITE_FILE);
    CatLog::InIt_Consumer_Thread("bbc", CatLog::WRITE_FILE);
    CatLog::InIt_Consumer_Thread("break", CatLog::PRINTF);
    std::thread th([](){
        for(int i = 0; i < 1000; i++)
        {
            CatLog* Logs = CatLog::Instance();
            Logs->AddLog_Detailed("main", CatLog::LEVEL_DEBUG,  __FILE__, __FUNCTION__, __LINE__, "CATLOG this thread log %d %s", i, "...");
        }
    });
    
    std::thread thdemo([](){
        for(int i = 0; i < 1000; i++)
        {
            CatLog* Logs = CatLog::Instance();
            Logs->AddLog_Detailed("demo", CatLog::LEVEL_ALARM,  __FILE__, __FUNCTION__, __LINE__, "CATLOG this thread log %d %s", i, "...");
        }
    });
    
    std::thread thcat([](){
        for(int i = 0; i < 1000; i++)
        {
            CatLog* Logs = CatLog::Instance();
            Logs->AddLog_Detailed("cat", CatLog::LEVEL_DEBUG,  __FILE__, __FUNCTION__, __LINE__, "CATLOG this thread log %d %s", i, "...");
        }
    });

    std::thread thbbq([](){
        for(int i = 0; i < 1000; i++)
        {
            CatLog* Logs = CatLog::Instance();
            Logs->AddLog_Detailed("bbq", CatLog::LEVEL_DEBUG,  __FILE__, __FUNCTION__, __LINE__, "CATLOG this thread log %d %s", i, "...");
        }
    });

    std::thread thbbc([](){
        for(int i = 0; i < 1000; i++)
        {
            CatLog* Logs = CatLog::Instance();
            Logs->AddLog_Detailed("bbc", CatLog::LEVEL_DEBUG,  __FILE__, __FUNCTION__, __LINE__, "CATLOG this thread log %d %s", i, "...");
        }
    });

    std::thread thbreak([](){
        for(int i = 0; i < 1000; i++)
        {
            CatLog* Logs = CatLog::Instance();
            Logs->AddLog_Detailed("break", CatLog::LEVEL_DEBUG,  __FILE__, __FUNCTION__, __LINE__, "CATLOG this thread log %d %s", i, "...");
        }
    });
    
    for(int i = 0; i < 1000; i++)
    {
        Logs->AddLog_Detailed("main", CatLog::LEVEL_DEBUG,  __FILE__, __FUNCTION__, __LINE__, "CATLOG this log %d %s", i, "...");
    }
    

    th.join();
    thdemo.join();
    thcat.join();
    thbbq.join();
    thbbc.join();
    thbreak.join();
    CatLog::Delete();

    return 0;
}
```
