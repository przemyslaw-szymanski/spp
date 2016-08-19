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

            public:

                CScript(uint32_t id, const std::string& filePath);

                void Init(uint64_t hLib);

                const
                std::string& GetName() const { return m_filePath; }

                uint64_t GetModificationTime() const { return m_modTime; }
                void SetModificationTime(uint64_t tm) { m_modTime = tm; }

                uint64_t GetLibHandle() const { return m_hHandle; }

                pfnEntryPoint* GetFuncTable() { return m_aFuncTable; }

                uint32_t GetID() const { return m_id; }

            protected:

                std::string     m_filePath;
                uint64_t        m_hHandle = 0;
                uint64_t        m_modTime = 0;
                pfnEntryPoint   m_aFuncTable[FuncTypes::_ENUM_COUNT];
                uint32_t        m_id = 0;
        };
    }
} // SPP