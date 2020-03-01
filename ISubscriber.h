#pragma once

#include <ctime>
#include <deque>
#include <string>

class ISubscriber
{
public:
    ISubscriber() = default;
    virtual ~ISubscriber() = default;

    virtual void update( const std::time_t blockTime, const std::deque<std::string>& data ) = 0;
};