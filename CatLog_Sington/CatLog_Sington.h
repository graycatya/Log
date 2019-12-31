#pragma once
#include "CatLog_Message.hpp"
#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include <fstream>
#include<queue>
#include<map>
#include<chrono>

enum class LOG_MODEL{ PRINTF = 0, WRITE_FILE, NETWORK };

template<LOG_MODEL model>
struct LOG_CONFIG
{
    
};

template<>
class CatLog
{

};
