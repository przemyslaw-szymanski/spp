#include "S++.h"
#include <iostream>
#include <string>
#include "C:\Users\szymansk\Desktop\Projects\spp\bin\interface.h"

class CApi : public IApi
{
    public:

    void Print(const char* msg) override
    {
        std::cout << "APP Message: " << msg << std::endl;
    }
};

int Main()
{
    CApi Api;
    SPP::CSPP* pSpp;
    if (!CreateSPP(&pSpp))
        return 1;

    auto pVM = pSpp->CreateVirtualMachine(SPP::VMTypes::DYNAMIC_LIBRARY_CPP);
    
    SPP::IVirtualMachine::SConfig Cfg;
    Cfg.compileCmdLine = "compile.cmd";
    pVM->SetConfig(Cfg);

    SPP::IVirtualMachine::CompilationLogVec vErrors;
    const char* name = "test.cpp";
    auto id = pVM->LoadScript(name, strlen(name));
    bool exit = false;
    bool watchMode = false;
    uint32_t loopCount = 0;
    std::string strInput;
    SPP::IVirtualMachine::UintVec vWatchScripts;
    while (!exit)
    {
        vErrors.clear();
        if (!watchMode)
        {
            std::cout << "VM: ";
            std::cin >> strInput;
            if (strInput == "compile")
            {
                if (pVM->CompileScript(id, &vErrors))
                    pVM->RunScript(id, &Api);
            }
            else if (strInput == "watch")
            {
                watchMode = true;
            }
            else if (strInput == "exit")
            {
                exit = true;
                break;
            }
        }
        else
        {
            vWatchScripts.clear();
            pVM->WatchScripts(&vWatchScripts);
            if (!vWatchScripts.empty())
                loopCount++;
            for (auto& id : vWatchScripts)
            {
                if (pVM->CompileScript(id, &vErrors))
                    pVM->RunScript(id, &Api);
            }
            if (loopCount > 10)
                exit = true;
        }

        for (uint32_t i = 0; i < vErrors.size(); ++i)
        {
            std::cout << vErrors[i].msg << std::endl;
        }
    }
  
    DestroySPP(&pSpp);
    return 0;
}

int main()
{
    int ret = Main();
    _CrtDumpMemoryLeaks();
    return ret;
}