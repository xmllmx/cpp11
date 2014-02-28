#pragma once

#include <ntnative/kcmn.hpp>

class NtFile
{
public:
    static constexpr ULONG DEFAULT_SHARE            = FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE;

    class NtStatusChecker final
    {
    public:
        typedef NTSTATUS StatusType;
        static const NTSTATUS default_status = STATUS_SUCCESS;

        bool operator ()(NTSTATUS status) const
        {
            return NT_SUCCESS(status);
        }
    };

    typedef Optional<ULONG_PTR, NtStatusChecker> NtResult;

protected:
    NtFile(bool is_ntfs)
        : _status(), _is_ntfs(is_ntfs), _is_dir()
    {}

    virtual ~NtFile() = default;

public:
    template<class PathStringType>
    NtResult Create(const PathStringType& full_path, ULONG intent, bool is_dir, ACCESS_MASK access_mask, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = is_dir;

        return _DoCreateFile(ObjectAttributes(full_path), intent, access_mask, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NtResult OpenExistsing(const PathStringType& full_path, bool is_dir, ACCESS_MASK access_mask, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = is_dir;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, access_mask, share_access, _GetRealOptions(options));
    }
    
    template<class PathStringType>
    NtResult CreateNewFile(const PathStringType& full_path)
    {
        _is_dir = false;
        
        return _DoCreateFile(ObjectAttributes(full_path), FILE_CREATE, SYNCHRONIZE, DEFAULT_SHARE, _GetRealOptions(0));
    }

    template<class PathStringType>
    NtResult CreateNewDirectory(const PathStringType& full_path)
    {
        _is_dir = true;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_CREATE, SYNCHRONIZE, DEFAULT_SHARE, _GetRealOptions(0));
    }
    
    template<class PathStringType>
    NtResult OpenDirectoryToListContents(const wchar_t* sz_full_path, ULONG options = 0, ULONG share_access = sm_default_sa)
    {
        _is_dir = true;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, SYNCHRONIZE|FILE_READ_ATTRIBUTES|FILE_LIST_DIRECTORY, share_access, _GetRealOptions(options));
    }
    
    template<class PathStringType>
    NtResult OpenFileToReadData(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = false;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, SYNCHRONIZE|FILE_READ_ATTRIBUTES|FILE_READ_DATA, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NtResult OpenFileToReadWriteData(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {     
        _is_dir = false;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, SYNCHRONIZE|FILE_READ_ATTRIBUTES|FILE_WRITE_ATTRIBUTES|FILE_READ_DATA|FILE_WRITE_DATA, 
                             share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NtResult OpenDirectoryToDeleteOrRename(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = true;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, SYNCHRONIZE|DELETE, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NtResult OpenFileToDeleteOrRename(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = false;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, SYNCHRONIZE|DELETE, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NtResult OpenDirectoryToQueryAttributes(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = true;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, SYNCHRONIZE|FILE_READ_ATTRIBUTES, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NtResult OpenDirectoryToSetAttributes(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = true;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, SYNCHRONIZE|FILE_WRITE_ATTRIBUTES, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NtResult OpenFileToQueryAttributes(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = false;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, SYNCHRONIZE|FILE_READ_ATTRIBUTES, share_access, _GetRealOptions(options));
    }

    template<class PathStringType>
    NtResult OpenFileToSetAttributes(const PathStringType& full_path, ULONG share_access = DEFAULT_SHARE, ULONG options = 0)
    {
        _is_dir = false;

        return _DoCreateFile(ObjectAttributes(full_path), FILE_OPEN, SYNCHRONIZE|FILE_WRITE_ATTRIBUTES, share_access, _GetRealOptions(options));
    }
    
    bool IsNtfs() const
    {
        return _is_ntfs;
    }

    NTSTATUS GetStatus() const
    {
        return _status;
    }
    
    bool IsDirectory() const
    {
        return _is_dir;
    }
    
    NtResult Read(uint64_t offset, ULONG size, void* output) const
    {
        if (nullptr == output)
        {
            return NtResult(0, STATUS_INVALID_PARAMETER_3);
        }

        return _DoRead(offset, size, output);
    }

    NtResult Read(uint64_t offset, ULONG size, Buffer& output) const
    {
        if (output.capacity() < size)
        {
            return NtResult(0, STATUS_BUFFER_TOO_SMALL);
        }
        
        auto nr = this->Read(offset, size, output.get());
        if (nr)
        {
            output.resize(*nr);
            Assert(output.size() == *nr);
        }

        return nr;
    }

    NtResult Read(uint64_t offset, Buffer& output) const
    {
        return this->Read(offset, output.capacity(), output);
    }

    NtResult Write(uint64_t offset, ULONG size, const void* input)
    {
        if (nullptr == input)
        {
            return NtResult(0, STATUS_INVALID_PARAMETER_3);
        }

        return _DoWrite(offset, size, input);
    }

    NtResult Write(uint64_t offset, ULONG size, const Buffer& input)
    {
        if (input.size() < size)
        {
            return NtResult(0, STATUS_INVALID_PARAMETER_3);
        }

        return this->Write(offset, size, input.get());
    }

    NtResult Write(uint64_t offset, const Buffer& input)
    {
        return this->Write(offset, input.size(), input.get());
    }

    NtResult QueryInformation(FILE_INFORMATION_CLASS info_class, void* output, ULONG output_size) const
    {
        if (nullptr == output)
        {
            return NtResult(0, STATUS_INVALID_PARAMETER_2);
        }

        if (0 == output_size)
        {
            return NtResult(0, STATUS_INVALID_PARAMETER_3);
        }

        return _DoQueryInformation(info_class, output, output_size);
    }

    NtResult SetInformation(FILE_INFORMATION_CLASS info_class, const void* input, ULONG input_size)
    {
        if (nullptr == input)
        {
            return NtResult(0, STATUS_INVALID_PARAMETER_2);
        }

        if (0 == input_size)
        {
            return NtResult(0, STATUS_INVALID_PARAMETER_3);
        }

        return _DoSetInformation(info_class, input, input_size);
    }

    NtResult SetSize(uint64_t new_size)
    {
        FILE_END_OF_FILE_INFORMATION info = {};
        info.EndOfFile.QuadPart           = new_size;

        return this->SetInformation(FileEndOfFileInformation, &info, sizeof(info));
    }

    Optional<uint64_t, NtStatusChecker> QuerySize() const
    {
        FILE_STANDARD_INFORMATION info = {};
        this->QueryInformation(FileStandardInformation, &info, sizeof(info));

        return Optional<uint64_t, NtStatusChecker>(info.EndOfFile.QuadPart, _status);
    }

    Optional<ULONG, NtStatusChecker> QueryAttributes() const
    {
        FILE_BASIC_INFORMATION  info = {};
        this->QueryInformation(FileBasicInformation, &info, sizeof(info));

        return Optional<ULONG, NtStatusChecker>(info.FileAttributes, _status);
    }

    NtResult SetAttributes(ULONG new_attributes)
    {
        FILE_BASIC_INFORMATION info = { {}, {}, {}, {}, new_attributes };

        return this->SetInformation(FileBasicInformation, &info, sizeof(info));
    }

    Optional<bool, NtStatusChecker> HasAttributes(ULONG attributes) const
    {
        if (auto nr = this->QueryAttributes())
        {
            return Optional<bool, NtStatusChecker>(!!(*nr & attributes), _status);
        }

        return Optional<bool, NtStatusChecker>(false, _status);
    }

    NtResult Delete()
    {
        FILE_DISPOSITION_INFORMATION info = {};
        info.DeleteFile = true;

        return this->SetInformation(FileDispositionInformation, &info, sizeof(info));
    }

    NtResult Rename(const WideStringRef& target_path, bool b_replace_if_exists = false)
    {
        auto buf  = MakeBuffer(target_path.length() * sizeof(wchar_t) + sizeof(FILE_RENAME_INFORMATION) + 1);
        auto info = Cast<PFILE_RENAME_INFORMATION>(buf);

        info->ReplaceIfExists = b_replace_if_exists;
        info->RootDirectory   = {};
        info->FileNameLength  = target_path.length() * sizeof(wchar_t);

        memcpy(info->FileName, target_path.c_str(), info->FileNameLength);
        info->FileName[target_path.length()] = 0;        

        return this->SetInformation(FileRenameInformation, buf.get(), buf.size());
    }

public:
    virtual bool IsValid() const = 0;
    virtual NTSTATUS Close() = 0;

protected:
    virtual NtResult _DoCreateFile(const POBJECT_ATTRIBUTES oa, ULONG intent, ACCESS_MASK access_mask, ULONG share_access, ULONG options) = 0;
    virtual NtResult _DoRead(uint64_t offset, ULONG size, void* output) const = 0;
    virtual NtResult _DoWrite(uint64_t offset, ULONG size, const void* input) = 0;
    virtual NtResult _DoQueryInformation(FILE_INFORMATION_CLASS info_class, PVOID output, ULONG output_size) const = 0;
    virtual NtResult _DoSetInformation(FILE_INFORMATION_CLASS info_class, const void* input, ULONG input_size) = 0;

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

    virtual ~ZwFile() override
    {
        this->ZwFile::Close();
    }

public:    
    ZwFile(bool is_ntfs)
        : NtFile(is_ntfs), _h_file()
    {}
    
    HANDLE Yield()
    {
        DEFER(_h_file = {});

        return _h_file;
    }

public:
    virtual bool IsValid() const override
    {
        return _h_file && NT_SUCCESS(_status);
    }

    virtual NTSTATUS Close() override
    {
        if (_h_file)
        {
            _status = ZwClose(_h_file);
            _h_file = {};
        }

        return _status;
    }
    
protected:
    virtual NtResult _DoCreateFile(const POBJECT_ATTRIBUTES oa, ULONG intent, ACCESS_MASK access_mask, ULONG share_access, ULONG options) override
    {
        this->Close();

        IO_STATUS_BLOCK iosb = {};
        _status = ZwCreateFile(&_h_file, access_mask, oa, &iosb, 0, 0, share_access, intent, options, 0, 0);

        return NtResult(iosb.Information, iosb.Status);
    }

    virtual NtResult _DoRead(uint64_t offset, ULONG size, void* output) const override
    {
        IO_STATUS_BLOCK iosb = {};
        _status = ZwReadFile(_h_file, 0, 0, 0, &iosb, output, size, LargeInteger(offset), 0);
        
        return NtResult(iosb.Information, _status);
    }

    virtual NtResult _DoWrite(uint64_t offset, ULONG size, const void* input) override
    {
        IO_STATUS_BLOCK iosb = {};
        _status = ZwWriteFile(_h_file, 0, 0, 0, &iosb, const_cast<PVOID>(input), size, LargeInteger(offset), 0);
        
        return NtResult(iosb.Information, _status);
    }

    virtual NtResult _DoQueryInformation(FILE_INFORMATION_CLASS info_class, PVOID output, ULONG output_size) const override
    {
        IO_STATUS_BLOCK iosb = {};
        _status = ZwQueryInformationFile(_h_file, &iosb, output, output_size, info_class);

        return NtResult(iosb.Information, iosb.Status);
    }

    virtual NtResult _DoSetInformation(FILE_INFORMATION_CLASS info_class, const void* input, ULONG input_size) override
    {
        IO_STATUS_BLOCK iosb = {};
        _status = ZwSetInformationFile(_h_file, &iosb, const_cast<void*>(input), input_size, info_class);

        return NtResult(iosb.Information, iosb.Status);
    }

protected:
    HANDLE _h_file;
};