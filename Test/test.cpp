#include <iostream>
#include <functional>
#include "CatLog_Sington.h"

int main()
{
    CatLog::Instance();
    std::string str = __DEBUG_HEAD;
    LOG_PRINTF(str);
    CatLog::Delete();
    return 0;
}