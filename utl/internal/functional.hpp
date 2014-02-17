#pragma once

template<class GenericFunction>
class Function;

template<class ResultType, class... Args>
class Function<ResultType(Args...)> final
{
private:
    INTERFACE _IFunctor
    {
        virtual ~_IFunctor() = default;
        virtual _IFunctor* Clone() const = 0;
        virtual ResultType operator ()(Args...) = 0;
    };

    template<class GenericFunction>
    struct _Functor : _IFunctor
    {
    public:
        using _Callable = typename Conditional<IsRegularFunction<GenericFunction>::value, 
            GenericFunction*, GenericFunction>::type;

        _Functor(_Callable fn) 
            : _fn(fn)
        {}

        _IFunctor* Clone() const override
        {
            return new _Functor(_fn);
        }
        
        ResultType operator ()(Args... args) override
        {
            return _fn(Forward<Args>(args)...);
        }

    private:
        _Callable _fn;
    };    

public:
    Function()
        : _fn()
    {}

    ~Function()
    {
        SAFE_DELETE(_fn);
    }

    template<class GenericFunction>
    Function(GenericFunction fn)
        : _fn(new _Functor<GenericFunction>(fn)) 
    {}

    Function(const Function& other)
        : _fn(other._fn->Clone())
    {}

    Function(Function&& other)
        : Function(other)
    {}

    DEFINE_COPY_ASSIGNER(Function);
    DEFINE_MOVE_ASSIGNER(Function);

    ResultType operator ()(Args... args) const 
    {
        return (*_fn)(Forward<Args>(args)...);
    }
    
    explicit operator bool() const
    {
        return !!_fn;
    }    

private:
    _IFunctor* _fn;
};

class _DeferrableOperations_ final
{
public:
    _DeferrableOperations_(Function<void()> fn)
        : _fn(fn)
    {}

    ~_DeferrableOperations_()
    {
        _fn();
    }

private:
    Function<void()> _fn;
};

#define DEFER(statements) _DeferrableOperations_ _UID_([&](){ statements##; })

template<class T, class ReturnType, class... Args>
class MemberFunction final
{
    using ConstMemberFunctionPtr = ReturnType(T::*)(Args...) const;
    using MemberFunctionPtr      = ReturnType(T::*)(Args...);

public:
    MemberFunction()
        : _const_obj(), _obj(), _const_fn(), _fn()
    {}
    
    MemberFunction(const T& obj, ConstMemberFunctionPtr fn)
        : _const_obj(&obj), _obj(&const_cast<T>(obj)), _const_fn(fn), _fn()
    {}

    MemberFunction(T& obj, MemberFunctionPtr fn)
        : _const_obj(&obj), _obj(&const_cast<T&>(obj)), _const_fn(), _fn(fn)
    {}

    ReturnType operator ()(Args... args)
    {
        if (_const_fn)
        {
            return (_const_obj.*_const_fn)(args...);
        }
        else
        {
            return (_obj.*_fn)(args...);
        }
    }

    explicit operator bool() const
    {
        return _obj && _fn || _const_obj && _const_fn;
    }

private:
    const T* _const_obj;
    T*       _obj;

    ConstMemberFunctionPtr _const_fn;
    MemberFunctionPtr      _fn;
};

template<class T, class ReturnType, class... Args>
Function<ReturnType(Args...)> Bind(const T& obj, ReturnType(T::*fn)(Args...) const)
{
    return MemberFunction<T, ReturnType, Args...>(obj, fn);
}

template<class T, class ReturnType, class... Args>
Function<ReturnType(Args...)> Bind(T& obj, ReturnType(T::*fn)(Args...))
{
    return MemberFunction<T, ReturnType, Args...>(obj, fn);
}

template<class T, class ReturnType, class... Args>
Function<ReturnType(Args...)> Bind(const T* obj, ReturnType(T::*fn)(Args...) const)
{
    return MemberFunction<T, ReturnType, Args...>(*obj, fn);
}

template<class T, class ReturnType, class... Args>
Function<ReturnType(Args...)> Bind(T* obj, ReturnType(T::*fn)(Args...))
{
    return MemberFunction<T, ReturnType, Args...>(*obj, fn);
}

template<class T, class ReturnType, class... Args>
ReturnType CallMemberFunction(T&& obj, ReturnType(T::*fn)(Args...) const, Args... args)
{
    return (Forward<T>(obj).*fn)(args...);
}

template<class T, class ReturnType, class... Args>
ReturnType CallMemberFunction(T* obj, ReturnType(T::*fn)(Args...), Args... args)
{
    return (obj->*fn)(args...);
}