#include <iostream>
#include <functional>
#include "../CatLog/CatLog_Sington.h"

#define NUM 10000

using namespace CATLOG;

int main()
{
    CatLog::Delete();
    CatLog::Instance();
    std::thread thread_test_0([]{
        for(int i = 0; i < NUM; i++)
        {
            CATLOG::CatLog::__Write_Log("./test0",__DEBUG_LOG("log: " + std::to_string(i)));
        }
    });
    thread_test_0.detach();
    std::thread thread_test_1([]{
        for(int i = 0; i < NUM; i++)
        {
            CATLOG::CatLog::__Write_Log("./test1",__INFO_LOG("log: " + std::to_string(i)));
        }
    });
    std::thread thread_test_2([]{
        for(int i = 0; i < NUM; i++)
        {
            CATLOG::CatLog::__Write_Log("./test2",_DEBUG_LOG("log: " + std::to_string(i)));
        }
    });
    std::thread thread_test_3([]{
        for(int i = 0; i < NUM; i++)
        {
            CATLOG::CatLog::__Write_Log(__WARN_LOG("log: " + std::to_string(i)));
        }
    });
    std::thread thread_test_4([]{
        for(int i = 0; i < NUM; i++)
        {
            CATLOG::CatLog::__Write_Log("./test4", __ALARM_LOG("log: " + std::to_string(i)));
        }
    });
    std::thread thread_test_5([]{
        for(int i = 0; i < NUM; i++)
        {
            CATLOG::CatLog::__Write_Log("./test5", __ALARM_LOG("log: " + std::to_string(i)));
        }
    });
    for(int i = 0; i < NUM; i++)
    {
        CATLOG::CatLog::__Write_Log(_INFO_LOG("log: " + std::to_string(i)));
    }
    //thread_test_0.join();
    thread_test_1.join();
    thread_test_2.join();
    thread_test_3.join();
    thread_test_4.join();
    thread_test_5.join();
    std::thread thread_test_6([]{
        for(int i = 0; i < NUM; i++)
        {
            CATLOG::CatLog::__Write_Log("./test6", __ALARM_LOG("log: " + std::to_string(i)));
        }
        CatLog::Delete();
    });
    thread_test_6.detach();
    //CATLOG::__Write_Log(_INFO_LOG("log: " + std::to_string(0)));
    return 0;
}
