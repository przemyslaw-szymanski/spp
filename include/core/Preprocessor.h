#pragma once

#if SPP_API_EXPORT
#   if WIN32
#       define SPP_API __declspec(dllexport)
#   else
#       error "undefined os"
#   endif // WIN32
#else // SPP_API_EXPORT
#   if WIN32
#       define SPP_API __declspec(dllimport)
#   endif // WIN32
#endif // SPP_API_EXPORT