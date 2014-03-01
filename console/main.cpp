//#include <ntnative/zw_user_mode.hpp>
#include <ucpp.hpp>
#include <iostream>

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
    TraceLog(3.14159 << ", 0x" << 222 << ", dss" << EndL);
    TraceLog(3.14159 << ", 0x" << 222 << ", dss" << EndL);
    TraceLog(3.14159 << ", 0x" << 222 << ", dss" << EndL);
}