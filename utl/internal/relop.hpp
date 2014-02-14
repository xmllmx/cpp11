#pragma once

template<class T, ENABLE_IF(sizeof(T) <= sizeof(void*))>
bool operator >(T a, T b)
{
    return b < a;
}

template<class T, ENABLE_IF(sizeof(T) <= sizeof(void*))>
bool operator ==(T a, T b)
{
    return !(a < b) && !(b < a);
}

template<class T, ENABLE_IF(sizeof(T) <= sizeof(void*))>
bool operator !=(T a, T b)
{
    return !(a == b);
}

template<class T, ENABLE_IF(sizeof(T) <= sizeof(void*))>
bool operator <=(T a, T b)
{
    return !(a > b);
}

template<class T, ENABLE_IF(sizeof(T) <= sizeof(void*))>
bool operator >=(T a, T b)
{
    return !(a < b);
}

template<class T, ENABLE_IF(sizeof(T) > sizeof(void*))>
bool operator >(const T& a, const T& b)
{
    return b < a;
}

template<class T, ENABLE_IF(sizeof(T) > sizeof(void*))>
bool operator ==(const T& a, const T& b)
{
    return !(a < b) && !(b < a);
}

template<class T, ENABLE_IF(sizeof(T) > sizeof(void*))>
bool operator !=(const T& a, const T& b)
{
    return !(a == b);
}

template<class T, ENABLE_IF(sizeof(T) > sizeof(void*))>
bool operator <=(const T& a, const T& b)
{
    return !(a > b);
}

template<class T, ENABLE_IF(sizeof(T) > sizeof(void*))>
bool operator >=(const T& a, const T& b)
{
    return !(a < b);
}