#include "CScript.h"

namespace SPP
{
    namespace DynamicLibrary
    {
        CScript::CScript(uint32_t id, const std::string& filePath) :
            m_id(id)
            , m_filePath(filePath)
        {}

        void CScript::Init(uint64_t hLib)
        {
            m_hHandle = hLib;
            memset(m_aFuncTable, 0, sizeof(m_aFuncTable));
        }
    }
} // SPP