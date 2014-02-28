#include <iostream>

bool g_enable_log = false;

using namespace std;

void LogAnsiString(const char* sz)
{
    cout << sz;
}

void LogWideString(const wchar_t* sz)
{
    wcout << sz;
}