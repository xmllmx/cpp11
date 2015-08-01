#pragma once

#include <ucpp.hpp>
#include <ctime>

class TimeCounter
{
public:
    TimeCounter()
        : _start(), _end()
    {}

private:
    std::clock_t _start, _end;
};