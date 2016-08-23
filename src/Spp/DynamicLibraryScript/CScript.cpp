#include "CScript.h"

namespace SPP
{
    namespace DynamicLibrary
    {
        CScript::CScript(uint32_t id, const char* pCurrDir, const char* pName) :
            m_id(id)
        {
            Init(0, pCurrDir);
            auto len = strlen(pName);
            SPP_MEMCPY(m_pFileName, sizeof(m_pFileName), pName, len);
            len = len > 511 ? 511 : len;
            m_pFileName[len] = '\0';
        }

        void CScript::Init(uint64_t hLib, const char* pCurrDir)
        {
            m_hHandle = hLib;
            memset(m_aFuncTable, 0, sizeof(m_aFuncTable));
            if (pCurrDir)
            {
                auto len = strlen(pCurrDir);
                SPP_MEMCPY(m_pCurrFileDir, sizeof(m_pCurrFileDir), pCurrDir, len);
                len = len > 511 ? 511 : len;
                m_pCurrFileDir[len] = '\0';
            }
        }

        void CScript::SetFuncTable(FuncPtrTable aTable)
        {
            for (uint32_t i = 0; i < FuncTypes::_ENUM_COUNT; ++i)
            {
                m_aFuncTable[i] = aTable[i];
            }
        }
    }
} // SPP
