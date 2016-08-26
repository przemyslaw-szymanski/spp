#include "../PlatformAPI.h"

#if defined(SPP_WINDOWS)
#include <windows.h>
#include <Shlwapi.h>
#include <shellapi.h>

namespace SPP
{
    namespace Platform
    {
        bool API::File::Exists(const char* filePath)
        {
            DWORD       fileAttr;

            fileAttr = GetFileAttributes(filePath);
            if (0xFFFFFFFF == fileAttr)
                return false;
            return true;
        }

        uint64_t API::File::Open(const char* pFilePath, const char* pOptions)
        {
            FILE* pFile = nullptr;
#if _MSC_VER
            auto err = fopen_s(&pFile, pFilePath, pOptions);
#else
            pFile = fopen(pFilePath, pOptions);
#endif
            return reinterpret_cast<uint64_t>(pFile);
        }

        void API::File::Close(uint64_t hFile)
        {
            FILE* pFile = reinterpret_cast<FILE*>(hFile);
            fclose(pFile);
        }

        uint64_t API::File::GetSize(uint64_t hFile)
        {
            FILE* pFile = reinterpret_cast<FILE*>(hFile);
            auto currPos = ftell(pFile);
            fseek(pFile, 0, SEEK_END);
            auto endPos = ftell(pFile);
            fseek(pFile, currPos, SEEK_SET);
            return endPos;
        }

        size_t API::File::Read(uint64_t hFile, void* pBuff, size_t buffSize, size_t elemSize,
                               size_t elemCount)
        {
            FILE* pFile = reinterpret_cast<FILE*>(hFile);
            size_t readSize;
#if _MSC_VER
            readSize = fread_s(pBuff, buffSize, elemSize, elemCount, pFile);
#else
            readSize = fread(pBuff, elemSize, elemCount, pFile);
#endif
            return readSize;
        }

        bool API::File::Copy(const char* srcFilePath, const char* dstFilePath)
        {
            return ::CopyFileA(srcFilePath, dstFilePath, FALSE) == TRUE;
        }

        bool API::File::Delete(const char* pFilePath)
        {
            return ::DeleteFileA(pFilePath) == TRUE;
        }

        bool API::File::Move(const char* pSrcFilePath, const char* pDstFilePath, bool overwrite)
        {
            DWORD flags = 0;
            if (overwrite)
            {
                flags |= MOVEFILE_COPY_ALLOWED;
            }
            auto res = ::MoveFileExA(pSrcFilePath, pDstFilePath, flags);
#if _DEBUG
            if (!res)
            {
                auto err = ::GetLastError();
            }
#endif
            return res;
        }

        bool API::Directory::GetCurrentDirectoryPath(char* pOut, uint32_t buffSize)
        {
            char* pBuff = pOut;
            auto len = ::GetCurrentDirectoryA(buffSize, pBuff);
            return len > 0;
        }

        bool API::Directory::Exists(const char* dirPath)
        {
            return API::File::Exists(dirPath);
        }

        bool API::Directory::Create(const char* dirPath)
        {
            return ::CreateDirectoryA(dirPath, nullptr) == TRUE;
        }

        bool API::Directory::Delete(const char* dirPath)
        {
            ::SHFILEOPSTRUCT file_op = {
                NULL,
                FO_DELETE,
                dirPath,
                "",
                FOF_NOCONFIRMATION |
                FOF_NOERRORUI |
                FOF_SILENT,
                false,
                0,
                "" };
            return ::SHFileOperationA(&file_op) == 0;
        }

        bool API::Directory::Copy(const char* srcDirPath, const char* dstDirPath)
        {
            ::SHFILEOPSTRUCTA s = { 0 };
            s.wFunc = FO_COPY;
            s.pFrom = srcDirPath;
            s.pTo = dstDirPath;
            s.fFlags = FOF_SILENT;
            return ::SHFileOperationA(&s) == 0;
        }

        int32_t API::Library::Load(uint64_t* pLibHandle, const char* libPath)
        {
            *pLibHandle = reinterpret_cast<uint64_t>(::LoadLibraryA(libPath));
            if (!*pLibHandle)
            {
                return ::GetLastError();
            }

            return 0;
        }

        struct FileTime
        {
            union
            {
                struct  
                {
                    uint32_t low;
                    uint32_t high;
                };
                uint64_t tm;
            };
        };

        uint64_t API::File::GetModificationTime(const char* pFilePath)
        {
            auto hFile = ::CreateFileA(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
            if (!hFile)
                return 0;
            ::FILETIME ct, lat, lwt;
            auto res = ::GetFileTime(hFile, &ct, &lat, &lwt);
            ::CloseHandle(hFile);
            if (!res)
                return 0;
            FileTime tm = { lwt.dwLowDateTime, lwt.dwHighDateTime };
            return tm.tm;
        }

        bool API::File::IsFileModified(uint64_t lastModTime, uint64_t currModTime)
        {
            FileTime tm1, tm2;
            tm1.tm = lastModTime;
            tm2.tm = currModTime;
            ::FILETIME ft1 = { tm1.low, tm1.high };
            ::FILETIME ft2 = { tm2.low, tm2.high };
            auto res = ::CompareFileTime(&ft1, &ft2);
            return res;
        }

        void API::Library::Unload(uint64_t libHandle)
        {
            ::FreeLibrary(reinterpret_cast<HMODULE>(libHandle));
        }

        void* API::Library::GetProcAddr(uint64_t libHandle, const char* pProcName)
        {
            return ::GetProcAddress(reinterpret_cast<HMODULE>(libHandle), pProcName);
        }

        bool API::Process::Start(const char* name, const char* cmdLine)
        {
            STARTUPINFO si = { 0 };
            si.cb = sizeof(si);
            PROCESS_INFORMATION pi = { 0 };
            LPSTR cmd = const_cast<LPSTR>(cmdLine);
            auto result = ::CreateProcessA(name, cmd, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
            if (result)
            {
                ::WaitForSingleObject(pi.hProcess, INFINITE);
                ::CloseHandle(pi.hProcess);
                ::CloseHandle(pi.hThread);
                return true;
            }
            return false;
        }
    } // Platform
} // SPP

#endif // SPP_WINDOWS