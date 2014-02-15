#include <bcpp.hpp>

template<class T>
class A
{
    template<class U, ENABLE_IF(true), class = int>
    friend A<U> MakeA();

    A(T)
    {}
};

template<class T, ENABLE_IF(true), class = int>
A<T> MakeA()
{
    return A<T>(T());
}

int main()
{
    WideString s1(L"ddd");
    auto s2 = Move(s1);
}