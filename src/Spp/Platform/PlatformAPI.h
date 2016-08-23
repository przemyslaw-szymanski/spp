#pragma once

#include "spp/SppCommon.h"

namespace SPP
{
    namespace Platform
    {
        class SPP_API API
        {
            public:

                struct File
                {
                    static const bool OVERRIDE = true;
                    static const bool NO_OVERRIDE = false;
                    static bool Exists(const char* filePath);                  
                    static uint64_t Open(const char* pFilePath, const char* options);
                    static void Close(uint64_t hFile);
                    static uint64_t GetSize(uint64_t hFile);
                    static size_t Read(uint64_t hFile, void* pBuff, size_t buffSize, size_t elemSize,
                                       size_t elemCount);
                    static uint64_t GetModificationTime(const char* pFilePath);
                    static bool IsFileModified(uint64_t lastModTime, uint64_t currModTime);
                    static bool Copy(const char* srcFilePath, const char* dstFilePath);
                    static bool Delete(const char* pFilePath);
                    static bool Move(const char* pSrcFilePath, const char* pDstFilePath, bool overwrite);
                };

                struct Directory
                {
                    static bool Exists(const char* dirPath);
                    static bool GetCurrentDirectoryPath(char* pOut, uint32_t buffSize);
                    static bool Create(const char* dirPath);
                    static bool Delete(const char* dirPath);
                    static bool Copy(const char* srcDirPath, const char* dstDirPath);
                };

                struct Library
                {
                    static int32_t Load(uint64_t* pLibHandle, const char* libPath);
                    static void Unload(uint64_t libHandle);
                    static void* GetProcAddr(uint64_t libHandle, const char* pProcName);
                };

                struct Process
                {
                    static bool Start(const char* name, const char* cmdLine);
                };
        };
    } // Platform
} // SPP