#pragma once

#include <mutex>
#include <thread>
#include <string>
#include <sstream>
#include <cstdarg>
#include <iostream>

class GFD
{
public:
    inline static std::mutex fdMutex;
    
    template <typename T, typename... Tetc>
    static void threadedCout(T v1, Tetc... v2)
    {
        coutMutex.lock();
        std::cout << printThread();
        threadedCout_internal(v1, v2...);
        coutMutex.unlock();
    }
    
private:
    inline static std::mutex coutMutex;
    
    template <typename T, typename... Tetc>
    static void threadedCout_internal(T v1, Tetc... v2)
    {
        std::cout << v1;
        threadedCout_internal(v2...);
    }
    
    static void threadedCout_internal()
    {
        std::cout << std::endl;
    }
    
    static std::string printThread()
    {
        std::stringstream stream;
        stream << "Thread " << std::this_thread::get_id() << ": ";
        return stream.str();
    }
};
