#pragma once

#include "spp/IVirtualMachine.h"

#if SPP_USE_MONO
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace SPP
{
    namespace CSharp
    {
        class CScript
        {
            using FuncTable = MonoMethod*[FunctionTypes::_ENUM_COUNT];
            using SScriptInfo = IVirtualMachine::SScriptInfo;
            friend class CMonoVM;
            public:

            CScript(uint32_t id);

            protected:

                SScriptInfo     m_Info;
                FuncTable       m_aFunctions;
                MonoImage*      m_pImg = nullptr;
                uint32_t        m_id = 0;
        };
    } // CSharp
} // SPP

#endif // SPP_USE_MONO
