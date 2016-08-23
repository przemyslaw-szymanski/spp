#pragma once

#include "SppCommon.h"

namespace SPP
{
    struct FunctionTypes
    {
        enum TYPE
        {
            ON_LOAD,
            ON_UNLOAD,
            ON_RUN,
            ON_UPDATE,
            _ENUM_COUNT
        };
    };
    using FUNCTION_TYPE = FunctionTypes::TYPE;

    class SPP_API IVirtualMachine
    {
        public:

            using CompilationLogVec = std::vector< SCompilationLog >;
            using UintVec = std::vector< uint32_t >;
            using StringVec = std::vector< std::string >;

            struct SConfig
            {
                std::string compilerDir;
                std::string compileCmdLine;
                std::string sharedLibraryExt;
                std::string sharedLibraryDebugSymbolExt;
            };

            struct SScriptInfo
            {
                std::string name;
                StringVec vFiles;
            };

        public:

            virtual
            bool        Init() { return true; }

            virtual
            void        SetConfig(const SConfig& Cfg) { m_Config = Cfg; }

            virtual
            uint32_t    LoadScript(const SScriptInfo&) = 0;

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