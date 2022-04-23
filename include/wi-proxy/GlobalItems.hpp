#pragma once

#include <mutex>

class GFD
{
public:
    inline static std::mutex fdMutex;
};
