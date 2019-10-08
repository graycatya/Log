# Log version: CatLog_0.1

```
#include "CatLog.h"
#include <iostream>
#include <thread>
int main()
{
    CatLog::Log::Instance();
    std::thread th([](){
        for(int i = 0; i < 1000; i++)
        {
            CATLOG(CatLog::Log::LEVEL_DEBUG, "Thread this log %d %s", i, "...");
        }
    });
    for(int i = 0; i < 1000; i++)
    {
        CATLOG_DETAILED(CatLog::Log::LEVEL_DEBUG, "CATLOG this log %d %s", i, "...");
    }
    
    th.join();

    CatLog::Log::SetLog_Print(true);
    for(int i = 0; i < 1000; i++)
    {
        CATLOG_DETAILED(CatLog::Log::LEVEL_DEBUG, "PRINTF this log %d %s", i, "...");
    }
    CatLog::Log::Delete();
    return 0;
}
```
