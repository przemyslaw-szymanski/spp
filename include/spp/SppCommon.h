#pragma once

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <map>
#include <cstring>
#include <cstdint>
#include <cassert>

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
            _ENUM_COUNT
        };
    };
    using VM_TYPE = VMTypes::TYPE;
} // SPP