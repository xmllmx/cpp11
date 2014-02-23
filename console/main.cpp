//#include <ntnative/zw_user_mode.hpp>
#include <ucpp.hpp>
#include <iostream>
#include <type_traits>
#include <tuple>
#include <ctime>

using namespace std;

struct A
{
    static int n;
};

int main()
{
    A::n = 0;
}