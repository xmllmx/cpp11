#pragma once

#include <stack.hpp>

class PathManager final
{	
public:
	DISABLE_COPY(PathManager);

    //
    // If the full path is L"\\Device\\HarddiskVolume1\\test.txt", then _is_begining_with_dir_separator is true.
    //
    // If the full path is L"D:\\test.txt", then _is_begining_with_dir_separator is false.
    // 
    PathManager()
        : _full_path(), _stack(), _is_begining_with_dir_separator()
    {}
    
    // !!! C++14
    PathManager(PathManager&& other)
        : _full_path(Move(other._full_path)), _stack(Move(other._stack)), _is_begining_with_dir_separator(Move(other._is_begining_with_dir_separator))
    {
        other._full_path                      = {};
        other._stack                          = {};
        other._is_begining_with_dir_separator = {};
    }

    // !!! C++14
    DEFINE_MOVE_ASSIGNER(PathManager);
	
    PathManager(const WideString& full_dir_path)
        : _full_path(MakeString(full_dir_path)), _stack(), _is_begining_with_dir_separator(IsPathSeparator(_full_path[0]))
    {}

    PathManager(WideString&& full_dir_path)
        : _full_path(Move(full_dir_path)), _stack(), _is_begining_with_dir_separator(IsPathSeparator(_full_path[0]))
    {}

    PathManager(const wchar_t* sz_full_dir_path, size_t sz_full_dir_path_len)
        : _full_path(MakeString(sz_full_dir_path, sz_full_dir_path_len)), _stack(), _is_begining_with_dir_separator(IsPathSeparator(_full_path[0]))
    {}

    PathManager(const wchar_t* sz_full_dir_path)
        : _full_path(MakeString(sz_full_dir_path)), _stack(), 
        _is_begining_with_dir_separator(IsPathSeparator(_full_path[0]))
    {}

    explicit operator bool() const
    {
        return _stack || _full_path;
    }

	bool PushFinalComponent()
	{
		auto old_len_of_full_path = _full_path.ValidSize();
		if (0 == old_len_of_full_path)
		{
			return false;
		}

		auto idx_of_last_char = old_len_of_full_path - 1;
		while (idx_of_last_char > 0)
		{
			if (IsPathSeparator(_full_path[idx_of_last_char]))
			{
				_full_path[idx_of_last_char] = 0;

				auto sz_final_component = &_full_path[idx_of_last_char + 1];
				auto len                = GetLength(sz_final_component);

				_full_path.ValidSize(old_len_of_full_path - (1 + len));
				_stack.Push(MakeString(sz_final_component));

				return true;
			}

			--idx_of_last_char;
		}

		Assert(0 == idx_of_last_char);

        if (_is_begining_with_dir_separator)
        {
            _stack.Push(MakeString(&_full_path[1]));
        }
        else
        {
            _stack.Push(MakeString(_full_path));
        }

        _full_path[0] = 0;
		_full_path.ValidSize(0);

		return true;
	}

	bool PopFinalComponent()
	{
		if (_stack.IsEmpty())
		{
			return false;
		}

		auto dir_name = _stack.MovePop();

		if (!_is_begining_with_dir_separator && 0 == _full_path.ValidSize())
		{
			_full_path = Move(dir_name);
		}
		else
		{
            _full_path = WideString() << _full_path << L"\\" << dir_name;
		}

		return true;
	}

    void ReplaceCurrentFullPath(const wchar_t* other_full_path)
    {
        _full_path = MakeString(other_full_path);
    }

    void ReplaceCurrentFullPath(WideString&& other_full_path)
    {
        _full_path = Move(other_full_path);
    }

	const wchar_t* GetCurrentFullPath() const
	{
		return _full_path;
	}

    const wchar_t* GetCurrentFinalComponent() const
    {
        if (auto p = _full_path.ReverseFind(IsPathSeparator<wchar_t>))
        {
            return ++p;
        }
        
        return nullptr;
    }

    WideString&& YieldFullPath()
    {
        return Move(_full_path);
    }

    const WideString& GetStackTop() const
    {
        return _stack.Top();
    }

    bool IsEmptyStack() const
    {
        return _stack.IsEmpty();
    }

private:
	WideString        _full_path;
	Stack<WideString> _stack;
	bool              _is_begining_with_dir_separator;

    DEFINE_SWAP_3(PathManager, _full_path, _stack, _is_begining_with_dir_separator);
};

inline WideString&& GetParentDir(WideString&& full_path)
{
    if (IsRootPath(full_path))
    {
        return Move(full_path);
    }

    if (IsPathSeparator(full_path.Back()))
    {
        Assert(full_path.ValidSize() > 1);

        full_path.Back() = 0;
        full_path.ValidSize(full_path.ValidSize() - 1);
    }

    Assert(!IsPathSeparator(full_path.Back()));

    auto idx = full_path.ValidSize();
    while (idx > 0)
    {
        --idx;

        if (IsPathSeparator(full_path[idx]))
        {
            full_path[idx] = 0;
            full_path.ValidSize(idx);

            Assert(Count(full_path, IsPathSeparator<wchar_t>) >= 2);

            if (2 == Count(full_path, IsPathSeparator<wchar_t>))
            {
                full_path << L'\\';
            }

            return Move(full_path);
        }
    }

    return {};
}

inline WideString GetParentDir(const WideString& full_path)
{
    return GetParentDir(MakeString(full_path));
}

inline WideString GetParentDir(const wchar_t* sz_full_path)
{
    return GetParentDir(MakeString(sz_full_path));
}

inline WideString GetFinalPart(const wchar_t* sz_full_path)
{
    Assert(sz_full_path);

    if (auto token = Find(sz_full_path, IsPathSeparator<wchar_t>))
    {
        return GetFinalPart(++token);
    }
    else
    {
        return MakeString(sz_full_path);
    }
}
