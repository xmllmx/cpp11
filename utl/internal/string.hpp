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
CharType* Copy(CharType* dst, const CharType* src, size_t count = Infinity<size_t>())
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
class RawString
{
public:
    RawString()
        : _sz(), _length()
    {}

    RawString(const CharType* sz, size_t length)
        : _sz(const_cast<CharType*>(sz)), _length(length)
    {}

    explicit operator bool() const
    {
        return !!_sz && !!_length;
    }

    size_t length() const
    {
        return _length;
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

typedef RawString<char>    AnsiRawString;
typedef RawString<wchar_t> WideRawString;

template<class CharType>
class BasicString
{
public:
    DEFAULT_MOVE(BasicString);

    BasicString(const BasicString& other)
        : _buf(other._buf.duplicate())
    {}

    DEFINE_COPY_ASSIGNER(BasicString);
    void swap(BasicString& other)
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

    bool operator <(const BasicString& other) const
    {
        return IsLessThan<BasicString>(_buf.c_str(), this->size(), other.c_str(), other.size());
    }

    bool operator >(const BasicString& other) const
    {
        return IsGreaterThan<BasicString>(_buf.c_str(), this->size(), other.c_str(), other.size());
    }

    bool operator ==(const BasicString& other) const
    {
        return IsEqualTo<BasicString>(_buf.c_str(), this->size(), other.c_str(), other.size());
    }

    bool operator !=(const BasicString& other) const
    {
        return IsNotEqualTo<BasicString>(_buf.c_str(), this->size(), other.c_str(), other.size());
    }

    bool operator <=(const BasicString& other) const
    {
        return IsLessThanOrEqualTo<BasicString>(_buf.c_str(), this->size(), other.c_str(), other.size());
    }

    bool operator >=(const BasicString& other) const
    {
        return IsGreaterThanOrEqualTo<BasicString>(_buf.c_str(), this->size(), other.c_str(), other.size());
    }

public:
    BasicString()
        : _buf()
    {}

    BasicString(size_t count, CharType c)
        : _buf(MakeUnique<CharType[]>(count + 1, true))
    {
        FOR(i, count)
        {
            _buf[i] = c;
        }

        _buf.set_valid_size(count);
    }

    BasicString(const CharType* sz)
        : _buf(sz ? MakeUnique<CharType[]>(GetLength(sz) + 1) : UniquePtr<CharType[]>())
    {
        if (sz)
        {
            Copy(_buf.get(), sz);
            _buf.set_valid_size(_buf.get_max_size() - 1);
        }
    }

    BasicString(const CharType* sz, size_t length)
        : _buf(length > 0 ? MakeUnique<CharType[]>(length + 1) : UniquePtr<CharType[]>())
    {
        if (length > 0)
        {
            Copy(_buf.get(), sz, length);
            _buf[length] = 0; // enforce the string is null-terminated
            _buf.set_valid_size(_buf.get_max_size() - 1);
        }
    }

    BasicString(const RawString<CharType>& raw_str)
        : BasicString(raw_str.data(), raw_str.length())
    {}

    size_t size() const
    {
        return _buf.get_valid_size();
    }

    size_t length() const
    {
        return _buf.get_valid_size();
    }

    bool empty() const
    {
        return 0 == _buf.get_valid_size();
    }

    size_t max_size() const
    {
        return _buf.get_max_size();
    }

    void reserve(size_t new_max_size)
    {
        if (new_max_size <= _buf.get_max_size())
        {
            return;
        }

        auto tmp_buf = MakeUnique<CharType[]>(new_max_size);
        
        if (_buf)
        {
            Copy(tmp_buf.get(), _buf.get());
        }

        tmp_buf.set_valid_size(_buf.get_valid_size());

        _buf = Move(tmp_buf);
    }

    void shrink_to_fit()
    {
        auto new_max_size = _buf.get_valid_size() + 1;
        if (new_max_size < _buf.get_max_size())
        {
            auto tmp_buf = MakeUnique<CharType[]>(new_max_size);
            Copy(tmp_buf.get(), _buf.get(), new_max_size);
            tmp_buf[_buf.get_valid_size() - 1] = 0; // Enforce the string is null-terminated.
            tmp_buf.set_valid_size(_buf.get_valid_size());

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
        return _buf[_buf.get_valid_size() - 1];
    }

    CharType& back()
    {
        return _buf[_buf.get_valid_size() - 1];
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
        if (_buf.get_valid_size() > 0)
        {
            _buf[0] = 0;
            _buf.set_valid_size(0);
        }
    }

    void push_back(CharType c)
    {
        if (_buf.get_valid_size() + 1 < _buf.get_max_size())
        {
            _buf[_buf.get_valid_size()] = c;
            _buf[_buf.get_valid_size() + 1] = 0;
            _buf.set_valid_size(_buf.get_valid_size() + 1);
        }
        else
        {
            this->reserve(2 * (_buf.get_max_size() + 2));
            this->push_back(c);
        }
    }

    void pop_back()
    {
        if (_buf.get_valid_size() > 0)
        {
            _buf[_buf.get_valid_size() - 1] = 0;
            _buf.set_valid_size(_buf.get_valid_size() - 1);
        }
    }

    BasicString& append(const CharType* sz, size_t cc_size = 0)
    {
        if (nullptr == sz)
        {
            return *this;
        }

        if (0 == cc_size)
        {
            cc_size = GetLength(sz);
        }

        auto new_valid_size = _buf.get_valid_size() + cc_size;

        this->reserve(new_valid_size + 1);
        memmove(&_buf[_buf.get_valid_size()], sz, cc_size * sizeof(CharType));        
        _buf.set_valid_size(new_valid_size);
        _buf[new_valid_size] = 0;

        return *this;
    }

    BasicString& append(const BasicString& other)
    {
        return this->append(other.c_str(), other.length());
    }

    BasicString& operator <<(CharType c)
    {
        this->push_back(c);

        return *this;
    }

    BasicString& operator <<(const CharType* sz)
    {
        while (*sz)
        {
            *this << *sz++;
        }

        return *this;
    }

    BasicString& operator <<(const BasicString& other)
    {
        FOR (i, other.size())
        {
            *this << other[i];
        }

        return *this;
    }

protected:
    UniquePtr<CharType[]> _buf;
};

typedef BasicString<char>    AnsiString;
typedef BasicString<wchar_t> WideString;
