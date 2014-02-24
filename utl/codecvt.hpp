#pragma once

#include <ucpp.hpp>

#include <string>
#include <locale>
#include <codecvt>
#include <iomanip>
#include <limits>
#include <sstream>

class NativeConverter
{
	using Converter = std::codecvt<wchar_t, char, mbstate_t>;

public:
	NativeConverter()
		: _converter(&std::use_facet<Converter>(std::locale("")))
	{}

	std::wstring Widen(const char* narrow_sz)
	{
		return _converter.from_bytes(narrow_sz);
	}

	std::string Narrow(const wchar_t* wide_sz)
	{
		return _converter.to_bytes(wide_sz);
	}

private:
	std::wstring_convert<Converter, wchar_t> _converter;
};

inline std::wstring NarrowToWide(const char* narrow_sz)
{
	return NativeConverter().Widen(narrow_sz);	
}

inline std::wstring NarrowToWide(const std::string& narrow_str)
{
	return NativeConverter().Widen(narrow_str.c_str());	
}

inline std::string WideToNarrow(const wchar_t* wide_sz)
{
	return NativeConverter().Narrow(wide_sz);
}

inline std::string WideToNarrow(const std::wstring& wide_str)
{
	return NativeConverter().Narrow(wide_str.c_str());
}

inline std::string WideToUtf8(const wchar_t* wide_sz)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>(new std::codecvt_utf8<wchar_t>).to_bytes(wide_sz);
}

inline std::string WideToUtf8(const std::wstring& wide_str)
{
	return WideToUtf8(wide_str.c_str());
}

inline std::string NarrowToUtf8(const char* narrow_sz)
{
	return WideToUtf8(NarrowToWide(narrow_sz));
}

inline std::string NarrowToUtf8(const std::string& narrow_str)
{
	return NarrowToUtf8(narrow_str.c_str());
}

inline std::wstring Utf8ToWide(const char* u8_sz)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>(new std::codecvt_utf8<wchar_t>).from_bytes(u8_sz);
}

inline std::wstring Utf8ToWide(const std::string& u8_str)
{
	return Utf8ToWide(u8_str.c_str());
}

inline std::string Utf8ToNarrow(const char* u8_sz)
{
	return WideToNarrow(Utf8ToWide(u8_sz));
}

inline std::string Utf8ToNarrow(const std::string& u8_str)
{
	return Utf8ToNarrow(u8_str.c_str());
}

template<class T, class CharType>
T FromStringTo(const CharType* sz)
{
    std::basic_stringstream<CharType> sstrm(sz);

    T v;
    sstrm >> v;

    return v;    
}

template<class T, class CharType>
T FromStringTo(const std::basic_string<CharType>& str)
{
    return FromStringTo<T>(str.c_str());    
}

template<class T, class CharType>
T FromStringTo(const BasicString<CharType>& str)
{
    return FromStringTo<T>(str.c_str());
}

template<class CharType, class T>
std::basic_string<CharType> ToBasicString(const T& v)
{
    std::basic_stringstream<CharType, std::char_traits<CharType>, std::allocator<CharType>> sstrm;

    if (IsFloatingPoint<T>::value)
    {
        sstrm << std::setprecision(std::numeric_limits<T>::digits10) << v;
    }
    else
    {
        sstrm << v;
    }
    
    return sstrm.str();
}

template<class T>
std::string ToAnsiString(const T& v)
{
    return ToBasicString<char, T>(v);
}

template<class T>
std::wstring ToWideString(const T& v)
{
    return ToBasicString<wchar_t, T>(v);
}