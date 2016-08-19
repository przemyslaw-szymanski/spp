#pragma once

#include "SppCommon.h"

namespace SPP
{
    class SPP_API IVirtualMachine
    {
        public:

            using CompilationLogVec = std::vector< SCompilationLog >;
            using UintVec = std::vector< uint32_t >;

            struct SConfig
            {
                std::string compileCmdLine;
            };

        public:

            virtual
            void        SetConfig(const SConfig& Cfg) { m_Config = Cfg; }

            virtual
            uint32_t    LoadScript(const void* pData, size_t dataSize) = 0;

            virtual
            bool        CompileScript(uint32_t scriptId, CompilationLogVec* pvErrors) = 0;

            virtual
            bool        RunScript(uint32_t scriptId, void*) = 0;

            virtual
            bool        UnloadScript(uint32_t scriptId) = 0;

            virtual
            void        WatchScripts(UintVec* pScripts) { pScripts->clear(); }
        
        protected:
            SConfig m_Config;
    };
} // SPP