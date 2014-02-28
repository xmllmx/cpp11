#include <ntnative/zw_user_mode.hpp>

using namespace std;

int main()
{
    auto p = Contains(AnsiString("Hello"), AnsiStringRef("llO"), false);
    Log("ddd");
}