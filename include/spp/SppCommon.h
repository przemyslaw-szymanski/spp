#pragma once

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <map>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <algorithm>

namespace SPP
{
#if defined(SPP_API_EXPORT)
#   if defined SPP_WINDOWS
#       define SPP_API  __declspec(dllexport)
#   else
#error "Platform not supported"
#   endif // SPP_WINDOWS
#else // SPP_EXPORT_API
#   if defined SPP_WINDOWS
#       define SPP_API  __declspec(dllimport)
#   else
#error "Platform not supported"
#   endif // SPP_WINDOWS
#endif // SPP_EXPORT_API

    struct SCompilationLog
    {
        std::string msg;
        std::string file;
        uint32_t    line;
    };

    struct VMTypes
    {
        enum TYPE
        {
            DYNAMIC_LIBRARY_CPP,
            CSHARP,
            LUA,
            CPP,
            _ENUM_COUNT
        };
    };
    using VM_TYPE = VMTypes::TYPE;
} // SPP


#if _MSC_VER
#   define SPP_SPRINTF(_pBuff, _uBuffSize, _pFormat, ...) ::sprintf_s((_pBuff), (_uBuffSize), (_pFormat), __VA_ARGS__)
#   define SPP_MEMCPY(_pDst, _uDstSize, _pSrc, _uBytesToCopy) ::memcpy_s((_pDst), (_uDstSize), (_pSrc), (_uBytesToCopy))
#   define SPP_STRCAT(_pDst, _uDstSize, _pSrc) ::strcat_s((_pDst), (_uDstSize), (_pSrc))
#else
#   define SPP_SPRINTF(_pBuff, _uBuffSize, _pFormat, ...) ::sprintf((_pBuff), (_pFormat), _VA_ARGS_)
#   define SPP_MEMCPY(_pDst, _uDstSize, _pSrc, _uBytesToCopy) ::memcpy((_pDst), (_pSrc), (_uBytesToCopy))
#   define SPP_STRCAT(_pDst, _uDstSize, _pSrc) ::strcat((_pDst), (_pSrc))
#endif