#pragma once

#include "spp/SppCommon.h"

namespace SPP
{
    namespace DynamicLibrary
    {
        class CScript
        {
            public:

                typedef void(*pfnEntryPoint)(void*);

                struct FuncTypes
                {
                    enum FUNC_TYPE
                    {
                        ON_LOAD,
                        ON_UNLOAD,
                        ON_RUN,
                        _ENUM_COUNT
                    };
                };
                using FUNC_TYPE = FuncTypes::FUNC_TYPE;
                using FuncPtrTable = pfnEntryPoint[FuncTypes::_ENUM_COUNT];

            public:

                CScript(uint32_t id, const char* pCurrDir, const char* pName);

                void Init(uint64_t hLib, const char* pCurrDir);

                const char* GetName() const { return m_pFileName; }
                const char* GetCurrentDirectory() const { return m_pCurrFileDir; }

                uint64_t GetModificationTime() const { return m_modTime; }
                void SetModificationTime(uint64_t tm) { m_modTime = tm; }

                uint64_t GetLibHandle() const { return m_hHandle; }

                pfnEntryPoint* GetFuncTable() { return m_aFuncTable; }
                void SetFuncTable(FuncPtrTable aTable);

                uint32_t GetID() const { return m_id; }

                uint32_t GetNextBuild() { return ++m_currBuild; }

            protected:

                char            m_pCurrFileDir[512];
                char            m_pFileName[512];
                uint64_t        m_hHandle = 0;
                uint64_t        m_modTime = 0;
                FuncPtrTable    m_aFuncTable;
                uint32_t        m_id = 0;
                uint32_t        m_currBuild = 0;
        };
    }
} // SPP