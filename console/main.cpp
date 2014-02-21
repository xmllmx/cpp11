//#include <ntnative/zw_user_mode.hpp>
#include <ucpp.hpp>
#include <iostream>
#include <type_traits>
#include <tuple>
#include <ctime>

using namespace std;

int main()
{
    START_COUNTING(n);
    LOOP(99999999)
    {
        double b = 3.14159 * 2.718;
        b *= b;
    }
    STOP_COUNTING(n);
}