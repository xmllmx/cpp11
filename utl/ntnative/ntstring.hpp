#pragma once

inline constexpr size_t GetLength(const UNICODE_STRING& str)
{
    return str.Length / sizeof(wchar_t);
}

inline constexpr size_t GetLength(const UNICODE_STRING* str)
{
    return str->Length / sizeof(wchar_t);
}

inline WideStringRef MakeStringRef(const UNICODE_STRING* str)
{
    return WideStringRef(str->Buffer, GetLength(str));
}

inline WideStringRef MakeStringRef(const UNICODE_STRING& str)
{
    return WideStringRef(str.Buffer, GetLength(str));
}

inline WideString MakeString(const UNICODE_STRING* str)
{
    return WideString(str->Buffer, GetLength(str));
}

inline WideString MakeString(const UNICODE_STRING& str)
{
    return WideString(str.Buffer, GetLength(str));
}

inline WideStringRef MakeStringRef(const FILE_DIRECTORY_INFORMATION* p)
{
    return WideStringRef(p->FileName, p->FileNameLength / sizeof(wchar_t));
}

inline WideStringRef MakeStringRef(const FILE_BOTH_DIR_INFORMATION* p)
{
    return WideStringRef(p->FileName, p->FileNameLength / sizeof(wchar_t));
}

inline WideStringRef MakeStringRef(const FILE_FULL_DIR_INFORMATION* p)
{
    return WideStringRef(p->FileName, p->FileNameLength / sizeof(wchar_t));
}

inline WideStringRef MakeStringRef(const FILE_ID_BOTH_DIR_INFORMATION* p)
{
    return WideStringRef(p->FileName, p->FileNameLength / sizeof(wchar_t));
}

inline WideStringRef MakeStringRef(const FILE_ID_FULL_DIR_INFORMATION* p)
{
    return WideStringRef(p->FileName, p->FileNameLength / sizeof(wchar_t));
}

inline WideStringRef MakeStringRef(const FILE_NAMES_INFORMATION* p)
{
    return WideStringRef(p->FileName, p->FileNameLength / sizeof(wchar_t));
}

inline WideStringRef MakeStringRef(const FILE_NAME_INFORMATION* p)
{
    return WideStringRef(p->FileName, p->FileNameLength / sizeof(wchar_t));
}

inline WideStringRef MakeStringRef(const FILE_RENAME_INFORMATION* p)
{
    return WideStringRef(p->FileName, p->FileNameLength / sizeof(wchar_t));
}

inline WideStringRef MakeStringRef(const FILE_STREAM_INFORMATION* p)
{
    return WideStringRef(p->StreamName, p->StreamNameLength / sizeof(wchar_t));
}

inline WideStringRef MakeStringRef(const FILE_OBJECT* file_obj)
{
    return MakeStringRef(file_obj->FileName);
}