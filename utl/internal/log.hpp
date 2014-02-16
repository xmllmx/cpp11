#pragma once

#if defined(_NT_NATIVE_MODE_)
/**/struct _EndLine_ {};
/**/constexpr _EndLine_ endl;
#else
/**/#include <iosfwd>
/**/#include <iostream>
#endif

#include <cctype>
#include <stdio.h>

template<class... Args>
void FormatString(char* buf, size_t buf_size, const char* fmt, Args... args)
{
#if defined(_ZW_KERNEL_MODE_)
    RtlStringCbPrintfA(buf, buf_size, fmt, args...);
#else
    sprintf_s(buf, buf_size, fmt, args...);
#endif
}

template<size_t t_n, class... Args>
void FormatString(char(&buf)[t_n], const char* fmt, Args... args)
{
#if defined(_ZW_KERNEL_MODE_)
    RtlStringCbPrintfA(buf, t_n, fmt, args...);
#else
    sprintf_s(buf, t_n, fmt, args...);
#endif
}

void PrintEx(const char* format, ...);

inline void LogAnsiString(const char* sz)
{
    PrintEx("%s", sz);
}

inline void LogWideString(const wchar_t* sz)
{
    PrintEx("%ws", sz);
}

enum class Radix
{
    HEX, hex, Dec
};

class Pointer final
{
public:
    Pointer()
        : _p(nullptr)
    {}

    Pointer(const void* p)
        : _p(p)
    {}

    const void* Get() const
    {
        return _p;
    }

    template<class Dest, ENABLE_IF(IsPointer<Dest>::value)>
    Dest Cast() const
    {
        return static_cast<Dest>(const_cast<void*>(_p));
    }

private:
    const void* _p;
};

class Logger
{
public:
    Logger()
        : _cur_radix(Radix::HEX)
    {}

    Logger& operator <<(Radix new_radix)
    {
        _cur_radix = new_radix;

        return *this;
    }

#if defined(_NT_NATIVE_MODE_)
    Logger& operator <<(_EndLine_)
#else
    typedef std::basic_ostream<char, std::char_traits<char>> _OStream_;
    Logger& operator <<(_OStream_& (__cdecl *)(_OStream_&))
#endif
    {
        LogAnsiString("\n");

        return *this;
    }

    Logger& operator <<(bool b)
    {
        char buf[8] = {};

        if (b)
        {
            FormatString(buf, "%s", "true");
        }
        else
        {
            FormatString(buf, "%s", "false");
        }

        LogAnsiString(buf);

        return *this;
    }

    Logger& operator <<(char c)
    {
        return _OutputInteger<8>("%c", "%c", "%c", c);
    }

    Logger& operator <<(wchar_t c)
    {
        return _OutputInteger<8>("%lc", "%lc", "%lc", c);
    }

    Logger& operator <<(signed char n)
    {
        return _OutputInteger<8>("%hhX", "%hhx", "%hhd", n);
    }

    Logger& operator <<(unsigned char n)
    {
        return _OutputInteger<8>("%hhX", "%hhx", "%hhu", n);
    }

    Logger& operator <<(signed short n)
    {
        return _OutputInteger<16>("%hX", "%hx", "%hd", n);
    }

    Logger& operator <<(unsigned short n)
    {
        return _OutputInteger<16>("%hX", "%hx", "%hu", n);
    }

    Logger& operator <<(signed long n)
    {
        return _OutputInteger<32>("%lX", "%lx", "%ld", n);
    }

    Logger& operator <<(unsigned long n)
    {
        return _OutputInteger<32>("%lX", "%lx", "%lu", n);
    }

    Logger& operator <<(signed int n)
    {
        return _OutputInteger<32>("%X", "%x", "%d", n);
    }

    Logger& operator <<(unsigned int n)
    {
        return _OutputInteger<32>("%X", "%x", "%u", n);
    }

    Logger& operator <<(signed long long n)
    {
        return _OutputInteger<64>("%llX", "%llx", "%lld", n);
    }

    Logger& operator <<(unsigned long long n)
    {
        return _OutputInteger<64>("%llX", "%llx", "%llu", n);
    }

    Logger& operator <<(const char* s)
    {
        LogAnsiString(s);

        return *this;
    }

    Logger& operator <<(const wchar_t* s)
    {
        LogWideString(s);

        return *this;
    }

    Logger& operator <<(Pointer p)
    {
        return _OutputInteger<64>("%p", "%p", "%p", p.Get());
    }

    Logger& operator <<(double f)
    {
        return _OutputInteger<64>("%f", "%f", "%f", f);
    }

    Logger& operator <<(long double Lf)
    {
        return _OutputInteger<128>("%Lf", "%Lf", "%Lf", Lf);
    }

    template<class CharType>
    Logger& operator <<(const RawString<CharType>& str)
    {
        FOR(i, str.length())
        {
            if (1 == sizeof(CharType))
            {
                *this << char(str[i]);
            }
            else
            {
                *this << wchar_t(str[i]);
            }
        }

        return *this;
    }

    template<class CharType>
    Logger& operator <<(const BasicString<CharType>& str)
    {
        return *this << MakeRawString<CharType>(str);
    }

private:
    template<size_t t_buf_size, class T>
    Logger& _OutputInteger(const char* fmt_HEX, const char* fmt_hex, const char* fmt_Dec, T n)
    {
        char buf[t_buf_size] = {};

        switch (_cur_radix)
        {
        case Radix::HEX:
        {
            FormatString(buf, fmt_HEX, n);
        }
        break;

        case Radix::hex:
        {
            FormatString(buf, fmt_hex, n);
        }
        break;

        case Radix::Dec:
        {
            FormatString(buf, fmt_Dec, n);
        }
        break;
        }

        LogAnsiString(buf);

        return *this;
    }

private:
    Radix _cur_radix;
};

#define Log(x) Logger() << x