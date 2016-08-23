#pragma once

#include "spp/IVirtualMachine.h"

namespace SPP
{
    namespace Utils
    {
        // Compiler helper
        class CCompiler
        {
            public:

            CCompiler(IVirtualMachine::SConfig&);
            ~CCompiler();

            bool Run(const std::vector< std::string >& vFiles, const std::string& strOut);

            protected:

                IVirtualMachine::SConfig&   m_Cfg;
        };

        int Replace(std::string* pInOut, const std::string& strFind, const std::string& strReplace);
    } // Utils
} // SPP