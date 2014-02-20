#pragma once

template<class T, size_t t_size>
class Array
{
    static_assert(IsStandardLayout<T>::value, "T is not standard layout");

public:
    size_t size() const
    {
        return t_size;
    }

    const T& operator [](size_t idx) const
    {
        return _buf[idx];
    }

    T& operator [](size_t idx)
    {
        return _buf[idx];
    }

    operator const T*() const
    {
        return _array;
    }

    operator T*()
    {
        return _array;
    }

protected:
    T _array[t_size];
};

template<class T>
constexpr bool IsLessThan(const T* buf_1, size_t buf_1_size, const T* buf_2, size_t buf_2_size)
{
    if (buf_1_size < buf_2_size)
    {
        return true;
    }
    else if (buf_1_size > buf_2_size)
    {
        return false;
    }

    FOR(i, buf_1_size)
    {
        if (buf_1[i] == buf_2[i])
        {
            continue;
        }
        else if (buf_1[i] < buf_2[i])
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}

template<class T>
constexpr bool IsGreaterThan(const T* buf_1, size_t buf_1_size, const T* buf_2, size_t buf_2_size)
{
    if (buf_1_size < buf_2_size)
    {
        return false;
    }
    else if (buf_1_size > buf_2_size)
    {
        return true;
    }

    FOR(i, t_size)
    {
        if (buf_1[i] == buf_2[i])
        {
            continue;
        }
        else if (buf_1[i] > buf_2[i])
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}

template<class T>
constexpr bool IsEqualTo(const T* buf_1, size_t buf_1_size, const T* buf_2, size_t buf_2_size)
{
    if (buf_1_size != buf_2_size)
    {
        return false;
    }

    FOR(i, t_size)
    {
        if (buf_1[i] == buf_2[i])
        {
            continue;
        }
        else
        {
            return false;
        }
    }

    return true;
}

template<class T>
constexpr bool IsNotEqualTo(const T* buf_1, size_t buf_1_size, const T* buf_2, size_t buf_2_size)
{
    if (buf_1_size != buf_2_size)
    {
        return true;
    }

    FOR(i, t_size)
    {
        if (buf_1[i] != buf_2[i])
        {
            return true;
        }
    }

    return false;
}

template<class T>
constexpr bool IsLessThanOrEqualTo(const T* buf_1, size_t buf_1_size, const T* buf_2, size_t buf_2_size)
{
    if (buf_1_size > buf_2_size)
    {
        return false;
    }
    else if (buf_1_size < buf_2_size)
    {
        return IsLessThan(buf_1, buf_1_size, buf_2, buf_2_size);
    }
    else
    {
        return IsEqualTo(buf_1, buf_1_size, buf_2, buf_2_size);
    }
}

template<class T>
constexpr bool IsGreaterThanOrEqualTo(const T* buf_1, size_t buf_1_size, const T* buf_2, size_t buf_2_size)
{
    if (buf_1_size < buf_2_size)
    {
        return false;
    }
    else if (buf_1_size > buf_2_size)
    {
        return IsGreaterThan(buf_1, buf_1_size, buf_2, buf_2_size);
    }
    else
    {
        return IsEqualTo(buf_1, buf_1_size, buf_2, buf_2_size);
    }
}

template<class T, size_t t_size>
class ComparableArray final : Array<T, t_size>
{
    static_assert(IsStandardLayout<ComparableArray>::value, "utl error");

public:
    constexpr bool operator <(const ComparableArray& other) const
    {
        return IsLessThan<T>(_array, t_size, other._array, t_size);
    }

    constexpr bool operator >(const ComparableArray& other) const
    {
        return IsGreaterThan<T>(_array, t_size, other._array, t_size);
    }

    constexpr bool operator ==(const ComparableArray& other) const
    {
        return IsEqualTo<T>(_array, t_size, other._array, t_size);
    }

    constexpr bool operator !=(const ComparableArray& other) const
    {
        return IsNotEqualTo<T>(_array, t_size, other._array, t_size);
    }

    constexpr bool operator <=(const ComparableArray& other) const
    {
        return IsLessThanOrEqualTo<T>(_array, t_size, other._array, t_size);
    }

    constexpr bool operator >=(const ComparableArray& other) const
    {
        return IsGreaterThanOrEqualTo<T>(_array, t_size, other._array, t_size);
    }
};