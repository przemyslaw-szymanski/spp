#include "CCompiler.h"
#include "../Platform/PlatformAPI.h"
#include <algorithm>

namespace SPP
{
    namespace Utils
    {
        int Replace(std::string* pInOut, const std::string& strFind, const std::string& strReplace)
        {
            int found = 0;
            for (std::string::size_type i = 0; (i = pInOut->find(strFind, i)) != std::string::npos;)
            {
                pInOut->replace(i, strFind.length(), strReplace);
                i += strReplace.length();
                found++;
            }
            return found;
        }

        CCompiler::CCompiler(IVirtualMachine::SConfig& Cfg) :
            m_Cfg(Cfg)
        {}

        CCompiler::~CCompiler()
        {}

        bool CCompiler::Run(const std::vector< std::string >& vFiles, const std::string& strOut)
        {
            static const std::string sFiles = "{FILES}";
            static const std::string sOut = "{OUTPUT}";

            using Process = Platform::API::Process;
            std::string strFiles;
            for (auto& strFile : vFiles)
            {
                strFiles += strFile;
                strFiles += " ";
            }
            std::string strCmdLine(m_Cfg.compileCmdLine);
            Utils::Replace(&strCmdLine, sFiles, strFiles);
            Utils::Replace(&strCmdLine, sOut, strOut);
            auto res = Process::Start(nullptr, strCmdLine.c_str());
            return res;
        }
    } // Utils
} // SPP