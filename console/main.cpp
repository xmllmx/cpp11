#include <bcpp.hpp>
#include <tuple>

using namespace std;

int main()
{
    Tuple<int, int> a;
    Get<0>(a) = 9;
    Get<1>(a) = 3;
}