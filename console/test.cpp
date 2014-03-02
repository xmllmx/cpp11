#include <ucpp.hpp>

using namespace std;

void test_unique_ptr()
{
    auto p1 = MakeUnique<int>(8);
    Assert(*p1 == 8);

    auto p2 = MakeUnique<int[]>(8);
    Assert(8 == p2.capacity());
    p2[7] = 0;
}

void test()
{
    test_unique_ptr();
}