#pragma once

#include <ntnative/kcmn.hpp>

class NtFile
{
public:
    static constexpr ULONG DEFAULT_SHARE            = FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE;

    static constexpr ULONG READ_DIR                 = FILE_LIST_DIRECTORY|FILE_TRAVERSE;
    static constexpr ULONG WRITE_DIR                = FILE_ADD_FILE|FILE_ADD_SUBDIRECTORY|FILE_LIST_DIRECTORY|FILE_TRAVERSE;

    static constexpr ULONG NTFS_READ_FILE           = FILE_GENERIC_READ|READ_CONTROL|ACCESS_SYSTEM_SECURITY;
    static constexpr ULONG NTFS_WRITE_FILE          = FILE_GENERIC_WRITE|WRITE_OWNER|WRITE_DAC|ACCESS_SYSTEM_SECURITY;

    static constexpr ULONG NTFS_READ_DIR            = READ_DIR|NTFS_READ_FILE;
    static constexpr ULONG NTFS_WRITE_DIR           = WRITE_DIR|NTFS_WRITE_FILE;
    static constexpr ULONG NON_NTFS_READ_FILE       = FILE_GENERIC_READ;
    static constexpr ULONG NON_NTFS_WRITE_FILE      = FILE_GENERIC_WRITE;
    static constexpr ULONG NON_NTFS_READ_DIR        = READ_DIR|NON_NTFS_READ_FILE;
    static constexpr ULONG NON_NTFS_WRITE_DIR       = WRITE_DIR|NON_NTFS_WRITE_FILE;

    static constexpr ULONG QUERY_ATTRIBUTES         = SYNCHRONIZE|FILE_READ_ATTRIBUTES;
    static constexpr ULONG SET_ATTRIBUTES           = QUERY_ATTRIBUTES|FILE_WRITE_ATTRIBUTES;
    static constexpr ULONG QUERY_EA                 = QUERY_ATTRIBUTES|FILE_READ_EA;
    static constexpr ULONG SET_EA                   = QUERY_ATTRIBUTES|FILE_WRITE_EA;
    static constexpr ULONG QUERY_SECURITY_INFO      = QUERY_ATTRIBUTES|READ_CONTROL|ACCESS_SYSTEM_SECURITY;    
    static constexpr ULONG SET_SECURITY_INFO        = QUERY_SECURITY_INFO|WRITE_OWNER|WRITE_DAC;

    static constexpr ULONG NTFS_FILE_ALL_RIGHTS     = NTFS_READ_FILE|NTFS_WRITE_FILE|QUERY_ATTRIBUTES|SET_ATTRIBUTES|SET_EA|QUERY_EA|SET_SECURITY_INFO|QUERY_SECURITY_INFO;
    static constexpr ULONG NTFS_DIR_ALL_RIGHTS      = NTFS_READ_DIR|NTFS_WRITE_DIR|QUERY_ATTRIBUTES|SET_ATTRIBUTES|SET_EA|QUERY_EA|SET_SECURITY_INFO|QUERY_SECURITY_INFO;
    static constexpr ULONG NON_NTFS_FILE_ALL_RIGHTS = NON_NTFS_READ_FILE|NON_NTFS_WRITE_FILE|QUERY_ATTRIBUTES|SET_ATTRIBUTES|SET_EA|QUERY_EA|SET_SECURITY_INFO|QUERY_SECURITY_INFO;
    static constexpr ULONG NON_NTFS_DIR_ALL_RIGHTS  = NON_NTFS_READ_DIR|NON_NTFS_WRITE_DIR|QUERY_ATTRIBUTES|SET_ATTRIBUTES|SET_EA|QUERY_EA|SET_SECURITY_INFO|QUERY_SECURITY_INFO;

protected:
    NtFile(bool is_ntfs)
        : _status(), _is_ntfs(is_ntfs), _is_dir()
    {}

    virtual ~NtFile() = 0
    {}

public:
    template<class PathStringType>
    NTSTATUS Create(const PathStringType& full_path, ULONG create_intent, bool is_dir, ACCESS_MASK access_mask, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = is_dir;

        return _DoCreateFile(ObjectAttributes(full_path), create_intent, access_mask, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenExistsing(bool is_dir, ACCESS_MASK access_mask, const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = is_dir;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, access_mask, share_access, _GetRealOptions(options));
    }
    
    template<class PathStringType>
    NTSTATUS CreateNewFile(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        auto access_mask = _is_ntfs ? NTFS_FILE_ALL_RIGHTS : NON_NTFS_FILE_ALL_RIGHTS;
        _is_dir = false;
        
        return _DoCreateFile(ObjectAttributes(full_path), FILE_CREATE, access_mask, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS CreateNewDirectory(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        auto access_mask = _is_ntfs ? NTFS_DIR_ALL_RIGHTS : NON_NTFS_DIR_ALL_RIGHTS;
        _is_dir = true;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_CREATE, access_mask, share_access, _GetRealOptions(options));
    }
    
    template<class PathStringType>
    NTSTATUS OpenDirectoryToReadData(const wchar_t* sz_full_path, ULONG options = 0, ULONG share_access = sm_default_sa)
    {
        auto access_mask = _is_ntfs ? NTFS_READ_DIR|QUERY_ATTRIBUTES|QUERY_EA|QUERY_SECURITY_INFO :
            NON_NTFS_READ_DIR|QUERY_ATTRIBUTES|QUERY_EA|QUERY_SECURITY_INFO;;
        _is_dir = true;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, access_mask, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenDirectoryToWriteData(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        auto access_mask = _is_ntfs ? NTFS_DIR_ALL_RIGHTS : NON_NTFS_DIR_ALL_RIGHTS;
        _is_dir = true;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, access_mask, share_access, _GetRealOptions(options));
    }    
    
    template<class PathStringType>
    NTSTATUS OpenFileToReadData(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        auto access_mask = _is_ntfs ? NTFS_READ_FILE|QUERY_ATTRIBUTES|QUERY_EA|QUERY_SECURITY_INFO :
            NON_NTFS_READ_FILE|QUERY_ATTRIBUTES|QUERY_EA|QUERY_SECURITY_INFO;
        _is_dir = false;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, access_mask, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenFileToWriteData(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        auto access_mask = _is_ntfs ? NTFS_FILE_ALL_RIGHTS : NON_NTFS_FILE_ALL_RIGHTS;
        _is_dir = false;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, access_mask, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenDirectoryToDelete(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        auto access_mask = _is_ntfs ? NTFS_DIR_ALL_RIGHTS : NON_NTFS_DIR_ALL_RIGHTS;
        _is_dir = true;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, access_mask|DELETE, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenFileToDelete(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        auto access_mask = _is_ntfs ? NTFS_FILE_ALL_RIGHTS : NON_NTFS_FILE_ALL_RIGHTS;
        _is_dir = false;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, access_mask|DELETE, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenDirectoryToQueryAttributes(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = true;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, QUERY_ATTRIBUTES, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenDirectoryToSetAttributes(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = true;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, SET_ATTRIBUTES, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenFileToQueryAttributes(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = false;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, QUERY_ATTRIBUTES, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenFileToSetAttributes(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = false;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, SET_ATTRIBUTES, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenDirectoryToQueryEa(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = true;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, QUERY_EA, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenDirectoryToSetEa(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = true;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, SET_EA, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenFileToQueryEa(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = false;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, QUERY_EA, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenFileToSetEa(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = false;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, SET_EA, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenDirectoryToQuerySecurityInfo(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = true;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, QUERY_SECURITY_INFO, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenDirectoryToSetSecurityInfo(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = true;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, SET_SECURITY_INFO, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenFileToQuerySecurityInfo(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = false;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, QUERY_SECURITY_INFO, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NTSTATUS OpenFileToSetSecurityInfo(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = false;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, SET_SECURITY_INFO, share_access, _GetRealOptions(options));
    }

    bool IsNtfs() const
    {
        return _is_ntfs;
    }

    NTSTATUS GetLastStatus() const
    {
        return _status;
    }

    bool IsOk() const
    {
        return NT_SUCCESS(_status);
    }

    bool IsDirectory() const
    {
        return _is_dir;
    }
    
    NTSTATUS Read(uint64_t offset, Buffer& output) const
    {
        auto ret_len = ULONG();

        _status = _DoRead(offset, output.capacity(), output.get(), &ret_len);
        Assert(ret_len <= output.capacity());

        output.resize(ret_len);
        Assert(output.size() == ret_len);

        return _status;
    }

    IO_STATUS_BLOCK Write(uint64_t offset, const void* input, ULONG input_length)
    {
        auto ret_len = ULONG();
        _status      = _DoWrite(offset, input_length, input, &ret_len);

        return { {_status}, ret_len };
    }

    IO_STATUS_BLOCK Write(uint64_t offset, const Buffer& input)
    {
        return this->Write(offset, input.get(), input.size());
    }

    IO_STATUS_BLOCK QueryInformation(FILE_INFORMATION_CLASS info_class, void* info_buf, ULONG info_buf_size) const
    {
        if (!this->IsValid())
        {
            return { { STATUS_INVALID_HANDLE }, 0 };
        }

        IO_STATUS_BLOCK iosb = {};
        _status = _DoQueryInformation(info_class, &iosb, info_buf, info_buf_size);

        return iosb;
    }

    IO_STATUS_BLOCK SetInformation(FILE_INFORMATION_CLASS info_class, const void* info, ULONG info_size)
    {
        if (!this->IsValid())
        {
            return { { STATUS_INVALID_HANDLE }, 0 };
        }

        IO_STATUS_BLOCK iosb = {};
        _status = _DoSetInformation(info_class, &iosb, info, info_size);

        return iosb;
    }

    NTSTATUS SetSize(uint64_t new_size)
    {
        if (!this->IsValid())
        {
            return STATUS_INVALID_HANDLE;
        }

        FILE_END_OF_FILE_INFORMATION info = {};
        info.EndOfFile.QuadPart           = new_size;

        this->SetInformation(FileEndOfFileInformation, &info, sizeof(info));

        return _status;
    }

    uint64_t QuerySize() const
    {
        if (!this->IsValid())
        {
            return Max<uint64_t>();
        }

        FILE_STANDARD_INFORMATION info = {};
        this->QueryInformation(FileStandardInformation, &info, sizeof(info));

        return info.EndOfFile.QuadPart;
    }

public:
    virtual bool IsValid() const = 0;
    virtual NTSTATUS Close() = 0;

protected:
    virtual NTSTATUS _DoCreateFile(const POBJECT_ATTRIBUTES oa, ULONG create_intent, ACCESS_MASK access_mask, ULONG share_access, ULONG options) = 0;    
    virtual NTSTATUS _DoRead(uint64_t offset, ULONG length, void* output_buf, ULONG* num_of_bytes_read) const = 0;
    virtual NTSTATUS _DoWrite(uint64_t offset, ULONG length, const void* input_buf, ULONG* num_of_bytes_written) = 0;
    virtual NTSTATUS _DoQueryInformation(FILE_INFORMATION_CLASS info_class, PIO_STATUS_BLOCK iosb, PVOID info_buf, ULONG info_buf_size) const = 0;
    virtual NTSTATUS _DoSetInformation(FILE_INFORMATION_CLASS info_class, PIO_STATUS_BLOCK iosb, const void* info_buf, ULONG info_buf_size) = 0;

private:
    ULONG _GetRealOptions(ULONG options)
    {
        SetFlagOff(options, FILE_SYNCHRONOUS_IO_ALERT);
        SetFlagOn(options, FILE_SYNCHRONOUS_IO_NONALERT);

        if (_is_dir)
        {
            SetFlagOn(options, FILE_DIRECTORY_FILE);
            SetFlagOff(options, FILE_NON_DIRECTORY_FILE);
        }
        else
        {
            SetFlagOn(options, FILE_NON_DIRECTORY_FILE);
            SetFlagOff(options, FILE_DIRECTORY_FILE);
        }

        return options;
    }
    
protected:
    mutable NTSTATUS _status;
    bool             _is_ntfs;
    bool             _is_dir;
};

class ZwFile : public NtFile
{
public:
    DELETE_COPY(ZwFile);

    ZwFile(ZwFile&& other)
        : NtFile(Move(other)), _h_file(other._h_file)
    {
        _h_file  = {};
    }

    DEFINE_MOVE_ASSIGNER(ZwFile);
    void swap(ZwFile& other)
    {
        if (this != &other)
        {
            Swap(_status, other._status);
            Swap(_is_ntfs, other._is_ntfs);
            Swap(_is_dir, other._is_dir);
            Swap(_h_file, other._h_file);
        }
    }

    ~ZwFile() override
    {
        this->ZwFile::Close();
    }

public:    
    ZwFile(bool is_ntfs)
        : NtFile(is_ntfs), _h_file()
    {}
    
    bool IsValid() const override
    {
        return _h_file && this->IsOk();
    }

    NTSTATUS Close() override
    {
        if (_h_file)
        {
            _status = ZwClose(_h_file);
            _h_file = {};
        }

        return _status;
    }
    
protected:
    NTSTATUS _DoCreateFile(const POBJECT_ATTRIBUTES oa, ULONG create_intent, ACCESS_MASK access_mask, ULONG share_access, ULONG options) override
    {
        this->Close();

        IO_STATUS_BLOCK iosb = {};

        return _status = ZwCreateFile(&_h_file, access_mask, oa, &iosb, 0, 0, share_access, create_intent, options, 0, 0);
    }

    NTSTATUS _DoRead(uint64_t offset, ULONG length, void* output_buf, ULONG* num_of_bytes_read) const override
    {
        Assert(output_buf && num_of_bytes_read);

        IO_STATUS_BLOCK iosb = {};
        _status = ZwReadFile(_h_file, 0, 0, 0, &iosb, output_buf, length, LargeInteger(offset), 0);
        *num_of_bytes_read = iosb.Information;

        return _status;
    }

    NTSTATUS _DoWrite(uint64_t offset, ULONG length, const void* input_buf, ULONG* num_of_bytes_written) override
    {
        Assert(input_buf && num_of_bytes_written);

        IO_STATUS_BLOCK iosb = {};
        _status = ZwWriteFile(_h_file, 0, 0, 0, &iosb, const_cast<PVOID>(input_buf), length, LargeInteger(offset), 0);
        *num_of_bytes_written = iosb.Information;

        return _status;
    }

    NTSTATUS _DoQueryInformation(FILE_INFORMATION_CLASS info_class, PIO_STATUS_BLOCK iosb, PVOID info_buf, ULONG info_buf_size) const override
    {
        return _status = ZwQueryInformationFile(_h_file, iosb, info_buf, info_buf_size, info_class);
    }

    NTSTATUS _DoSetInformation(FILE_INFORMATION_CLASS info_class, PIO_STATUS_BLOCK iosb, const void* info_buf, ULONG info_buf_size) override
    {
        return _status = ZwSetInformationFile(_h_file, iosb, const_cast<void*>(info_buf), info_buf_size, info_class);
    }

protected:
    HANDLE _h_file;
};