#pragma once

template<class T>
class Stack
{
public:
    Stack(const Stack& other)
        : _capacity(other._capacity), _size(other._size), _buf(other._capacity ? Allocate<T*>(sizeof(T) * other._capacity) : nullptr)
    {
        if (_buf)
        {
            FOR(i, _size)
            {
                new (&_buf[i]) T(other._buf[i]);
            }
        }
    }

    Stack(Stack&& other)
        : _capacity(other._capacity), _size(other._size), _buf(other._buf)
    {
        other._capacity = 0;
        other._size = 0;
        other._buf = nullptr;
    }

    DEFINE_COPY_ASSIGNER(Stack);
    DEFINE_MOVE_ASSIGNER(Stack);

    void swap(Stack& other)
    {
        if (this != &other)
        {
            Swap(_capacity, other._capacity);
            Swap(_size, other._size);
            Swap(_buf, other._buf);
        }
    }

    ~Stack()
    {
        if (_buf)
        {
            FOR(i, _size)
            {
                Destruct(&_buf[i]);
            }

            SAFE_FREE(_buf);

            _capacity = 0;
            _size = 0;
        }
    }

public:
    Stack()
        : _capacity(), _size(), _buf()
    {}

    bool empty() const
    {
        return 0 == _size;
    }

    size_t size() const
    {
        return _size;
    }

    void push(const T& obj)
    {
        if (_size < _capacity)
        {
            new (&_buf[_size]) T(obj);
            ++_size;
        }
        else
        {
            _ExtendCapacity();
            this->push(obj);
        }
    }

    void push(T&& obj)
    {
        if (_size < _capacity)
        {
            new (&_buf[_size]) T(Move(obj));
            ++_size;
        }
        else
        {
            _ExtendCapacity();
            this->push(Move(obj));
        }
    }

    void pop()
    {
        if (_size > 0)
        {
            Destruct(&_buf[--_size]);
        }
    }

    T& top()
    {
        return _buf[_size - 1];
    }

    const T& top() const
    {
        return _buf[_size - 1];
    }

    template<class... Args>
    void emplace(Args&&... args)
    {
        if (_size > 0)
        {
            _buf[_size - 1] = T(Forward<Args...>(args...));
        }
    }

private:
    void _ExtendCapacity()
    {
        _capacity = (_capacity + 1) * 2;

        auto tmp_buf = Allocate<T*>(_capacity * sizeof(T));
        FOR(i , _size)
        {
            new (&tmp_buf[i]) T(Move(_buf[i]));
            Destruct(&_buf[i]);
        }

        Swap(_buf, tmp_buf);
        SAFE_FREE(tmp_buf);
    }

private:
    size_t _capacity;
    size_t _size;
    T*     _buf;    
};