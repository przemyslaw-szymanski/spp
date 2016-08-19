#include "S++.h"
#include "DynamicLibraryScript/CDynLibVM.h"

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

    IVirtualMachine* CSPP::CreateVirtualMachine(VM_TYPE eType)
    {
        IVirtualMachine* pVM = nullptr;
        switch (eType)
        {
            case VMTypes::DYNAMIC_LIBRARY_CPP:
            {
                pVM = new(std::nothrow)DynamicLibrary::CDynLibVM();
                m_pInternal->vVMs.push_back(pVM);
            }
            break;
            default:
            break;
        }
        return pVM;
    }
} // SPP