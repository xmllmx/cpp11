#pragma once

template<class CharType>
class BasicString
{
public:
    BasicString()
    {}

    size_t length() const
    {
        if (0 == _buf.size())
        {
            return 0;
        }
        else
        {
            return _buf.size() - 1;
        }
    }

    const T& operator [](size_t idx) const
    {
        return _buf[idx];
    }

    T& operator [](size_t idx)
    {
        return _buf[idx];
    }

protected:
    BasicBuffer<CharType> _buf;
};

typedef BasicString<char>    AnsiString;
typedef BasicString<wchar_t> WideString;