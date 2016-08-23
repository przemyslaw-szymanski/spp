#include "CScript.h"

#if SPP_USE_MONO
namespace SPP
{
    namespace CSharp
    {
        CScript::CScript(uint32_t id) :
            m_id(id)
        {

        }
    } // Csharp
} // SPP
#endif // SPP_USE_MONO
