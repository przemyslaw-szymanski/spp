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

int MainDLL()
{
    CApi Api;
    SPP::CSPP* pSpp;
    if (!CreateSPP(&pSpp))
        return 1;

    SPP::IVirtualMachine::SConfig Cfg;
    Cfg.compileCmdLine = "compile.cmd";
    Cfg.sharedLibraryExt = "dll";
    Cfg.sharedLibraryDebugSymbolExt = "pdb";
    auto pVM = pSpp->CreateVirtualMachine(SPP::VMTypes::DYNAMIC_LIBRARY_CPP, Cfg);

    Cfg.compilerDir = pSpp->GetWorkDirectoryPath();
    char buff[1024];
    SPP_SPRINTF(buff, sizeof(buff), "%s\\mono\\4.5\\mcs.exe {FILES} /target:module /out:{OUTPUT}",
                Cfg.compilerDir.c_str());
    Cfg.compileCmdLine = buff;
    auto pCS = pSpp->CreateVirtualMachine(SPP::VMTypes::CSHARP, Cfg);
    SPP::IVirtualMachine::CompilationLogVec vErrors;
    uint32_t id = 0;
    {
        
        SPP::IVirtualMachine::SScriptInfo ScriptInfo;
        ScriptInfo.name = "test";
        ScriptInfo.vFiles.push_back("test.cpp");
        id = pVM->LoadScript(ScriptInfo);
    }
    {
        SPP::IVirtualMachine::SScriptInfo ScriptInfo;
        ScriptInfo.name = "Test";
        ScriptInfo.vFiles.push_back("Test.cs");
        id = pCS->LoadScript(ScriptInfo);
        pCS->CompileScript(id, &vErrors);
    }

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
            if (strInput == "compile" || strInput == "c")
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

int MainCSharp()
{
    return 0;
}

int main()
{
    int ret = MainDLL();
    _CrtDumpMemoryLeaks();
    return ret;
}