//#include <ntnative/zw_user_mode.hpp>
#include <ucpp.hpp>
#include <iostream>

#include <string>

using namespace std;

class MyLogger : public ILogger
{
public:
    virtual void Output(ILogger::Type type, const AnsiStringRef& u8_msg) override
    {
        std::cout << u8_msg.c_str();
    }
};

using namespace std;

int main()
{
    MemoryPool<1024> pool;
    pool.Initialize(1024);

    void* buf[10000] = {};

    START_COUNTING(n);
    FOR(i, 10000)
    {
        buf[i] = pool.GetBlock();
    }

    FOR(i, 10000)
    {
        pool.FreeBlock(buf[i]);
    }

    FOR(i, 10000)
    {
        buf[i] = pool.GetBlock();
    }
    STOP_COUNTING(n);
}