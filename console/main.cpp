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

void test();
void test_nt_file();

void CheckMemoryLeak();

int main()
{
    {
        test();
        test_nt_file();
        delete[] new char[8];
    }

    CheckMemoryLeak();
}