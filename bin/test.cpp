#include <iostream>
#include "interface.h"

#define EXPORT extern "C" void __declspec(dllexport)


EXPORT OnLoad(void* pPtr)
{
    std::cout << "LOAD" << std::endl;
}

EXPORT OnUnload(void* pPtr)
{
    std::cout << "UnLOAD" << std::endl;
}

EXPORT OnRun(void* pPtr)
{
    const char* msg = "hellooouuu34245345";
    IApi* pApi = (IApi*)pPtr;
    pApi->Print(msg);
    std::cout << msg << std::endl;
}
