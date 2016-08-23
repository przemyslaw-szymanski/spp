#pragma once
#include "spp/IVirtualMachine.h"

namespace SPP
{
    namespace CSharp
    {
        

        class CMonoVM : public IVirtualMachine
        {
            struct SInternal;
            
            public:

            CMonoVM();
            ~CMonoVM();

            bool Init();

            virtual
                uint32_t    LoadScript(const SScriptInfo&) override;

            virtual
                bool        CompileScript(uint32_t scriptId, CompilationLogVec* pvErrors);

            virtual
                bool        RunScript(uint32_t scriptId, void*);

            virtual
                bool        UnloadScript(uint32_t scriptId);

            protected:

                SInternal*  m_pInternal = nullptr;
        };
    } // CSharp
} // CSharp
