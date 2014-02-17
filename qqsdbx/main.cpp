#include <ntnative/kcmn.hpp>

class A
{
public:
    enum B
    {
        C
    };
    
    template<B b>
    void f()
    {}
};

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT drv_obj, PUNICODE_STRING reg_path)
{
    A a;
    a.f<A::C>();

    return 0;
}