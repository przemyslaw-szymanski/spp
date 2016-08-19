#pragma once

#include "spp/SppCommon.h"
#include "spp/IVirtualMachine.h"

namespace SPP
{
    class IVirtualMachine;
    class SPP_API CSPP
    {
        struct SInternal;
        public:

            CSPP();
            ~CSPP();

            IVirtualMachine* CreateVirtualMachine(VM_TYPE eType);

        protected:

            SInternal*  m_pInternal = nullptr;
    };
} // SPP

extern "C"
{
    bool SPP_API CreateSPP(SPP::CSPP** ppOut);
    void SPP_API DestroySPP(SPP::CSPP** ppOut);
}