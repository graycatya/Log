# Log version: CatLog_0.1

```
int main()
{

    Log* Logs = Log::Instance();
    Log::InIt_Consumer_Thread(Log::WRITE_FILE);
    Log::InIt_Consumer_Thread(Log::WRITE_FILE, "demo");
    Log::InIt_Consumer_Thread(Log::WRITE_FILE, "cat");
    Log::InIt_Consumer_Thread(Log::WRITE_FILE, "bbq");
    Log::InIt_Consumer_Thread(Log::WRITE_FILE, "bbc");
    std::thread th([](){
        for(int i = 0; i < 1000; i++)
        {
            Log* Logs = Log::Instance();
            Logs->AddLog_Detailed("main", Log::LEVEL_DEBUG,  __FILE__, __FUNCTION__, __LINE__, "CATLOG this thread log %d %s", i, "...");
        }
    });
    
    std::thread thdemo([](){
        for(int i = 0; i < 1000; i++)
        {
            Log* Logs = Log::Instance();
            Logs->AddLog_Detailed("demo", Log::LEVEL_DEBUG,  __FILE__, __FUNCTION__, __LINE__, "CATLOG this thread log %d %s", i, "...");
        }
    });
    
    std::thread thcat([](){
        for(int i = 0; i < 1000; i++)
        {
            Log* Logs = Log::Instance();
            Logs->AddLog_Detailed("cat", Log::LEVEL_DEBUG,  __FILE__, __FUNCTION__, __LINE__, "CATLOG this thread log %d %s", i, "...");
        }
    });

    std::thread thbbq([](){
        for(int i = 0; i < 1000; i++)
        {
            Log* Logs = Log::Instance();
            Logs->AddLog_Detailed("bbq", Log::LEVEL_DEBUG,  __FILE__, __FUNCTION__, __LINE__, "CATLOG this thread log %d %s", i, "...");
        }
    });

    std::thread thbbc([](){
        for(int i = 0; i < 1000; i++)
        {
            Log* Logs = Log::Instance();
            Logs->AddLog_Detailed("bbc", Log::LEVEL_DEBUG,  __FILE__, __FUNCTION__, __LINE__, "CATLOG this thread log %d %s", i, "...");
        }
    });
    
    for(int i = 0; i < 1000; i++)
    {
        Logs->AddLog_Detailed("main", Log::LEVEL_DEBUG,  __FILE__, __FUNCTION__, __LINE__, "CATLOG this log %d %s", i, "...");
    }
    

    th.join();
    thdemo.join();
    thcat.join();
    thbbq.join();
    thbbc.join();
    Log::Delete();

    return 0;
}

```
