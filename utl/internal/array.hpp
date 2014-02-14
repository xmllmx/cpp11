#pragma once

template<class T, size_t t_size>
class Array
{
public:
    constexpr size_t size() const
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

template<class T, size_t t_size>
class ComparableArray final : Array<T, t_size>
{
public:
    bool operator <(const ComparableArray& other) const
    {
        FOR(i, t_size)
        {
            if (_array[i] == other._array[i])
            {
                continue;
            }
            else if (_array[i] < other._array[i])
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
};