#include <ntkrnl/kcpp.hpp>

Driver* CreateDriverInstance(PDRIVER_OBJECT drv_obj, PUNICODE_STRING reg_path)
{
    int* p = new int;

    new (p) int;

    Print(Pointer(p) << EndL);

    return 0;
}