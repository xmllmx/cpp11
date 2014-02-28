#pragma once

template<class CharType>
size_t GetLength(const CharType* sz)
{
    FOR(i, Max<size_t>())
    {
        if (0 == sz[i])
        {
            return i;
        }
    }

    return 0;
}

template<class CharType>
CharType* Copy(CharType* dst, const CharType* src, size_t count = Max<size_t>())
{
    FOR(i, count)
    {
        dst[i] = src[i];

        if (0 == src[i])
        {
            break;
        }
    }

    return dst;
}

template<class CharType>
class String;

template<class CharType>
class StringRef final
{
public:
    StringRef()
        : _sz(), _length()
    {}

    StringRef(const CharType* sz, size_t length = 0)
        : _sz(const_cast<CharType*>(sz)), _length(length ? length : GetLength(sz))
    {}

    StringRef(const String<CharType>& str)
        : _sz(const_cast<CharType*>(str.c_str())), _length(str.size())
    {}

    template<size_t t_capacity>
    StringRef(const CharType(&sz)[t_capacity])
        : StringRef(_sz, GetLength(_sz))
    {}

    explicit operator bool() const
    {
        return !!_sz && !!_length;
    }

    size_t size() const
    {
        return _length;
    }

    size_t length() const
    {
        return _length;
    }

    CharType* c_str()
    {
        return _sz;
    }

    const CharType* c_str() const
    {
        return _sz;
    }

    CharType* data()
    {
        return _sz;
    }

    const CharType* data() const
    {
        return _sz;
    }

    CharType& operator [](size_t idx)
    {
        return _sz[idx];
    }

    const CharType& operator [](size_t idx) const
    {
        return _sz[idx];
    }

    CharType& front()
    {
        return _sz[0];
    }

    const CharType& front() const
    {
        return _sz[0];
    }

    CharType& back()
    {
        return _sz[_length - 1];
    }

    const CharType& back() const
    {
        return _sz[_length - 1];
    }

    void pop_back()
    {
        if (*this)
        {
            _sz[--_length] = 0;
        }
    }

private:
    CharType* _sz;
    size_t    _length;
};

typedef StringRef<char>    AnsiStringRef;
typedef StringRef<wchar_t> WideStringRef;

template<class CharType>
class String final
{
public:
    DEFAULT_MOVE(String);

    String(const String& other)
        : _buf(other._buf.duplicate())
    {}

    DEFINE_COPY_ASSIGNER(String);
    void swap(String& other)
    {
        if (this != &other)
        {
            Swap(_buf, other._buf);
        }
    }

    explicit operator bool() const
    {
        return _buf;
    }

    bool operator <(const String& other) const
    {
        return IsLessThan<String>(_buf.c_str(), this->size(), other.c_str(), other.size());
    }

    bool operator >(const String& other) const
    {
        return IsGreaterThan<String>(_buf.c_str(), this->size(), other.c_str(), other.size());
    }

    bool operator ==(const String& other) const
    {
        return IsEqualTo<String>(_buf.c_str(), this->size(), other.c_str(), other.size());
    }

    bool operator !=(const String& other) const
    {
        return IsNotEqualTo<String>(_buf.c_str(), this->size(), other.c_str(), other.size());
    }

    bool operator <=(const String& other) const
    {
        return IsLessThanOrEqualTo<String>(_buf.c_str(), this->size(), other.c_str(), other.size());
    }

    bool operator >=(const String& other) const
    {
        return IsGreaterThanOrEqualTo<String>(_buf.c_str(), this->size(), other.c_str(), other.size());
    }

public:
    String()
        : _buf()
    {}

    String(size_t reserved_size)
        : _buf(MakeUnique<CharType[]>(count + 1))
    {
        _buf.resize(0);
    }

    String(size_t count, CharType c)
        : String(count + 1)
    {
        FOR(i, count)
        {
            _buf[i] = c;
        }

        _buf[count] = 0;
        _buf.resize(count);
    }

    String(const CharType* sz)
        : _buf(sz ? MakeUnique<CharType[]>(GetLength(sz) + 1) : UniquePtr<CharType[]>())
    {
        if (sz)
        {
            Copy(_buf.get(), sz);
            _buf.resize(_buf.capacity() - 1);
        }
    }

    String(const CharType* sz, size_t length)
        : _buf(length > 0 ? MakeUnique<CharType[]>(length + 1) : UniquePtr<CharType[]>())
    {
        if (length > 0)
        {
            Copy(_buf.get(), sz, length);
            _buf[length] = 0; // enforce the string is null-terminated
            _buf.resize(_buf.capacity() - 1);
        }
    }

    template<size_t t_capacity>
    String(const CharType (&sz)[t_capacity])
        : String(sz, GetLength(sz))
    {}

    String(const StringRef<CharType>& raw_str)
        : String(raw_str.data(), raw_str.length())
    {}

    size_t size() const
    {
        return _buf.size();
    }

    size_t length() const
    {
        return _buf.size();
    }

    bool empty() const
    {
        return 0 == _buf.size();
    }

    size_t capacity() const
    {
        return _buf.capacity();
    }

    size_t max_size() const
    {
        return _buf.capacity();
    }

    void reserve(size_t new_capacity)
    {
        if (new_capacity <= _buf.capacity())
        {
            return;
        }

        auto tmp_buf = MakeUnique<CharType[]>(new_capacity);
        
        if (_buf)
        {
            Copy(tmp_buf.get(), _buf.get());
        }

        tmp_buf.resize(_buf.size());

        _buf = Move(tmp_buf);
    }

    void shrink_to_fit()
    {
        auto new_capacity = _buf.size() + 1;
        if (new_capacity < _buf.capacity())
        {
            auto tmp_buf = MakeUnique<CharType[]>(new_capacity);
            Copy(tmp_buf.get(), _buf.get(), new_capacity);
            tmp_buf[_buf.size() - 1] = 0; // Enforce the string is null-terminated.
            tmp_buf.resize(_buf.size());

            _buf = Move(tmp_buf);
        }
    }

    const CharType& operator [](size_t idx) const
    {
        return _buf[idx];
    }

    CharType& operator [](size_t idx)
    {
        return _buf[idx];
    }

    const CharType& front() const
    {
        return _buf[0];
    }

    CharType& front()
    {
        return _buf[0];
    }

    const CharType& back() const
    {
        return _buf[_buf.size() - 1];
    }

    CharType& back()
    {
        return _buf[_buf.size() - 1];
    }

    const CharType* data() const
    {
        return _buf.get();
    }

    CharType* data()
    {
        return _buf.get();
    }

    const CharType* c_str() const
    {
        return _buf.get();
    }

    CharType* c_str()
    {
        return _buf.get();
    }

    void clear()
    {
        if (_buf.size() > 0)
        {
            _buf[0] = 0;
            _buf.resize(0);
        }
    }

    void push_back(CharType c)
    {
        if (_buf.size() + 1 < _buf.capacity())
        {
            _buf[_buf.size()] = c;
            _buf[_buf.size() + 1] = 0;
            _buf.resize(_buf.size() + 1);
        }
        else
        {
            this->reserve(2 * (_buf.size() + 2));
            this->push_back(c);
        }
    }

    void pop_back()
    {
        if (_buf.size() > 0)
        {
            _buf[_buf.size() - 1] = 0;
            _buf.resize(_buf.size() - 1);
        }
    }

    String& append(const CharType* sz, size_t cc_size = 0)
    {
        if (nullptr == sz)
        {
            return *this;
        }

        if (0 == cc_size)
        {
            cc_size = GetLength(sz);
        }

        auto new_size = _buf.size() + cc_size;

        this->reserve(new_valid_size + 1);
        memmove(&_buf[_buf.size()], sz, cc_size * sizeof(CharType));        
        _buf.resize(new_size);
        _buf[new_size] = 0;

        return *this;
    }

    String& append(const StringRef<CharType>& str)
    {
        return this->append(str.c_str(), str.length());
    }

    String& operator <<(CharType c)
    {
        this->push_back(c);

        return *this;
    }

    String& operator <<(const StringRef<CharType>& str)
    {
        return this->append(str);
    }

protected:
    UniquePtr<CharType[]> _buf;
};

typedef String<char>    AnsiString;
typedef String<wchar_t> WideString;

template<class T, ENABLE_IF(!IsCharType<T>::value && !IsIntegral<T>::value)>
inline void tolower(T& str)
{
    typedef typename RemoveConstVolatileReferencePointer<decltype((str[0]))>::type CharType;

    StringRef<CharType> str_ref = str;

    FOR(i, str_ref.size())
    {
        str_ref[i] = tolower(str_ref[i]);
    }
}

template<class T, ENABLE_IF(!IsCharType<T>::value && !IsIntegral<T>::value)>
inline void toupper(T& str)
{
    typedef typename RemoveConstVolatileReferencePointer<decltype((str[0]))>::type CharType;

    StringRef<CharType> str_ref = str;

    FOR(i, str_ref.size())
    {
        str_ref[i] = toupper(str_ref[i]);
    }
}

template<class StringType1, class StringType2>
bool Contains(const StringType1& str, const StringType2& sub_str, bool is_case_sensitive = true)
{
    return !!Find(str, sub_str, is_case_sensitive);
}

template<class CharType, ENABLE_IF(IsCharType<CharType>::value)>
inline bool Equals(CharType a, CharType b, bool is_case_sensitive = true)
{
    if (is_case_sensitive)
    {
        return a == b;
    }

    if (IsAnsiChar(a) && IsAnsiChar(b))
    {
        return tolower(a) == tolower(b);
    }

    return false;
}

template<class CharType, ENABLE_IF(IsCharType<CharType>::value)>
inline bool Equals(const CharType* a, const CharType* b, bool is_case_sensitive = true)
{
    if (a == b)
    {
        return true;
    }

    if (!a || !b)
    {
        return false;
    }

    if (is_case_sensitive)
    {
        FOR(i, Max<size_t>())
        {
            if (a[i] != b[i])
            {
                return false;
            }

            if (0 == a[i])
            {
                return true;
            }
        }
    }
    else
    {
        FOR(i, Max<size_t>())
        {
            if (IsAnsiChar(a[i]) && IsAnsiChar(b[i]))
            {
                if (tolower(a[i]) != tolower(b[i]))
                {
                    return false;
                }
            }
            else
            {
                if (a[i] != b[i])
                {
                    return false;
                }
            }

            if (0 == a[i])
            {
                return true;
            }
        }
    }

    return false;
}

template<class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool Equals(const StringRef<CharType>& str_a, const StringRef<CharType>& str_b, bool is_case_sensitive = true)
{
    if (str_a.length() == str_b.length())
    {
        FOR(i, str_a.length())
        {
            if (!Equals(str_a[i], str_b[i], is_case_sensitive))
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

inline bool Equals(const AnsiStringRef& str_a, const AnsiStringRef& str_b, bool is_case_sensitive = true)
{
    return Equals<char>(str_a, str_b, is_case_sensitive);
}

inline bool Equals(const WideStringRef& str_a, const WideStringRef& str_b, bool is_case_sensitive = true)
{
    return Equals<wchar_t>(str_a, str_b, is_case_sensitive);
}

template<class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool BeginsWith(const StringRef<CharType>& str, const StringRef<CharType>& sub_str, bool is_case_sensitive = true)
{
    if (str.length() < sub_str.length())
    {
        return false;
    }

    return Equals(StringRef<CharType>(str.data(), sub_str.length()), sub_str, is_case_sensitive);
}

inline bool BeginsWith(const AnsiStringRef& str, const AnsiStringRef& sub_str, bool is_case_sensitive = true)
{
    return BeginsWith<char>(str, sub_str, is_case_sensitive);
}

inline bool BeginsWith(const WideStringRef& str, const WideStringRef& sub_str, bool is_case_sensitive = true)
{
    return BeginsWith<wchar_t>(str, sub_str, is_case_sensitive);
}

template<class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool EndsWith(const StringRef<CharType>& str, const StringRef<CharType>& sub_str, bool is_case_sensitive = true)
{
    auto str_len     = str.length();
    auto sub_str_len = sub_str.length();

    if (str_len < sub_str_len)
    {
        return false;
    }

    return Equals(StringRef<CharType>(&str[str_len - sub_str_len], sub_str_len), sub_str, is_case_sensitive);
}

inline bool EndsWith(const AnsiStringRef& str, const AnsiStringRef& sub_str, bool is_case_sensitive = true)
{
    return EndsWith<char>(str, sub_str, is_case_sensitive);
}

inline bool EndsWith(const WideStringRef& str, const WideStringRef& sub_str, bool is_case_sensitive = true)
{
    return EndsWith<wchar_t>(str, sub_str, is_case_sensitive);
}

template<class CharType, ENABLE_IF(IsCharType<CharType>::value)>
CharType* Find(const StringRef<CharType>& str, const StringRef<CharType>& sub_str, bool is_case_sensitive)
{
    auto str_size = str.size();
    auto sub_str_size = sub_str.size();

    if (str_size < sub_str_size)
    {
        return nullptr;
    }

    if (str_size == sub_str_size)
    {
        return Equals(str, sub_str, is_case_sensitive) ? const_cast<CharType*>(&str[0]) : nullptr;
    }

    auto end_pos = (str_size - sub_str_size) + 1;

    FOR(i, end_pos)
    {
        if (Equals(StringRef<CharType>(&str[i], sub_str_size), sub_str, is_case_sensitive))
        {
            return const_cast<CharType*>(&str[i]);
        }
    }

    return nullptr;
}

inline char* Find(const AnsiStringRef& str, const AnsiStringRef& sub_str, bool is_case_sensitive = true)
{
    return Find<char>(str, sub_str, is_case_sensitive);
}

inline wchar_t* Find(const WideStringRef& str, const WideStringRef& sub_str, bool is_case_sensitive = true)
{
    return Find<wchar_t>(str, sub_str, is_case_sensitive);
}