#include <ntnative/zw_user_mode.hpp>

using namespace std;

int main()
{
    auto n = STATUS_SUCCESS;
    {
        ZwFile f(true);
        f.Create(L"\\??\\C:\\123", FILE_CREATE, true, SYNCHRONIZE);        
        f.OpenDirectoryToDeleteOrRename(L"\\??\\C:\\123");
        auto nr = f.Rename(L"\\??\\C:\\abc");
    }
}