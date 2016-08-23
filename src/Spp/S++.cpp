#include "S++.h"
#include "DynamicLibraryScript/CDynLibVM.h"
#include "CSharpScript/CMonoVM.h"
#include "Platform/PlatformAPI.h"

extern "C"
{
    bool SPP_API CreateSPP(SPP::CSPP** ppOut)
    {
        SPP::CSPP* pPtr = new(std::nothrow) SPP::CSPP();
        *ppOut = pPtr;
        return pPtr != nullptr;
    }

    void SPP_API DestroySPP(SPP::CSPP** ppOut)
    {
        if (ppOut && *ppOut)
        {
            delete *ppOut;
            *ppOut = nullptr;
        }
    }
}

namespace SPP
{
    using VMVec = std::vector< IVirtualMachine* >;

    struct CSPP::SInternal
    {
        VMVec   vVMs;
    };

    CSPP::CSPP()
    {
        m_pInternal = new(std::nothrow) SInternal;
    }

    CSPP::~CSPP()
    {
        for (auto& pVM : m_pInternal->vVMs)
        {
            delete pVM;
            pVM = nullptr;
        }
        delete m_pInternal;
        m_pInternal = nullptr;
    }

    std::string CSPP::GetWorkDirectoryPath() const
    {
        char buff[2048];
        if (Platform::API::Directory::GetCurrentDirectoryPath(buff, sizeof(buff)))
        {
            return std::string(buff);
        }
        return "";
    }

    IVirtualMachine* CSPP::CreateVirtualMachine(VM_TYPE eType, const IVirtualMachine::SConfig& Cfg)
    {
        IVirtualMachine* pVM = nullptr;
        switch (eType)
        {
            case VMTypes::DYNAMIC_LIBRARY_CPP:
            {
                pVM = new(std::nothrow)DynamicLibrary::CDynLibVM();
            }
            break;
            case VMTypes::CSHARP:
            {
#if SPP_USE_MONO
                pVM = new(std::nothrow)CSharp::CMonoVM();             
#endif
            }
            break;
            default:
            break;
        }
        if (pVM)
        {
            pVM->SetConfig(Cfg);
            if (pVM->Init())
            {
                m_pInternal->vVMs.push_back(pVM);
            }
            else
            {
                delete pVM;
                pVM = nullptr;
            }
        }
        return pVM;
    }
} // SPP