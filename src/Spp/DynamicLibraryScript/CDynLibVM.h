#pragma once

#include "spp/IVirtualMachine.h"
#include <mutex>

namespace SPP
{
    namespace DynamicLibrary
    {
        class CScript;
        class SPP_API CDynLibVM : public SPP::IVirtualMachine
        {
            public:
            using ScriptVec = std::vector< CScript* >;
            using ScriptMap = std::map< uint32_t, CScript* >;

            public:

            ~CDynLibVM();

                uint32_t            LoadScript(const SScriptInfo&) override;
                bool                CompileScript(uint32_t scriptId, CompilationLogVec* pvErrors) override;
                bool                RunScript(uint32_t scriptId, void*) override;
                bool                UnloadScript(uint32_t scriptId) override;
                void                WatchScripts(UintVec* pScripts) override;

                bool                CompileScript(CScript** ppScript, CompilationLogVec* pvErrors);
                bool                RunScript(CScript** ppScript, void*);
                bool                UnloadScript(CScript** ppScript);

                bool                ReloadScript(CScript** ppScript);

            protected:

                CScript*            _FindScript(uint32_t id);

            protected:

                ScriptMap       m_mScripts;
                UintVec         m_vFreeScripts;
                uint32_t        m_currId = 0;
                uint32_t        m_lastUsedId = 0;
                CScript*        m_pLastUsedScript = nullptr;
                std::mutex      m_Mutex;
        };
    } // DynamicLibrary
} // SPP