#pragma once

#include <ctime>

class TickCounterDataBody_
{
protected:
    TickCounterDataBody_()
        : _start_value(std::clock()), _stop_value()
    {}

protected:
    std::clock_t _start_value;
    std::clock_t _stop_value;
};

class TickCounter final : TickCounterDataBody_
{
public:
    TickCounter()
        : TickCounterDataBody_(), _tick_count()
    {}

    std::clock_t Start()
    {
        return _start_value = std::clock();
    }

    std::clock_t Stop()
    {
        _stop_value = std::clock();

        return _tick_count = _stop_value - _start_value;
    }

    std::clock_t GetCount()
    {
        return _tick_count;
    }

private:
    std::clock_t _tick_count;    
};

#define START_COUNTING(tick_counter) TickCounter tick_counter;
#define STOP_COUNTING(tick_counter) tick_counter.Stop();