#include "CDynLibVM.h"
#include "PlatformAPI.h"
#include "CScript.h"
#include <regex>

namespace SPP
{
    namespace DynamicLibrary
    {

        CScript* FindScript(CDynLibVM::ScriptMap& mMap, uint32_t id)
        {
            auto Itr = mMap.find(id);
            if (Itr == mMap.end())
                return nullptr;
            return Itr->second;
        }

        CDynLibVM::~CDynLibVM()
        {
            for (auto& Pair : m_mScripts)
            {
                auto pScript = Pair.second;
                Platform::API::Library::Unload(pScript->GetLibHandle());
                delete pScript;
            }
            m_mScripts.clear();
        }

        CScript* CDynLibVM::_FindScript(uint32_t id)
        {
            if (m_lastUsedId == id)
                return m_pLastUsedScript;
            m_lastUsedId = id;
            m_pLastUsedScript = FindScript(m_mScripts, id);
            return m_pLastUsedScript;
        }

        uint32_t CDynLibVM::LoadScript(const void* pData, size_t dataSize)
        {
            assert(pData);
            (void)dataSize;
            char dirPath[512];
            Platform::API::File::GetCurrentDirectoryPath(dirPath, 512);
            const char* filePath = reinterpret_cast<const char*>(pData);
            if (!Platform::API::File::Exists(filePath))
            {
                return 0;
            }

            // If there are any free scripts
            CScript* pScript = nullptr;
            uint32_t scriptId = 0;
            if (!m_vFreeScripts.empty())
            {
                auto id = m_vFreeScripts.back();
                pScript = FindScript(m_mScripts, id);
                scriptId = id;
                pScript = new(pScript)CScript(scriptId, filePath);
                m_vFreeScripts.pop_back();
            }
            else
            {
                scriptId = ++m_currId;
                pScript = new CScript(scriptId, filePath);
                m_mScripts.insert(ScriptMap::value_type(scriptId, pScript));
            }
            
            return scriptId;
        }

        bool ParseCompilerOutput(const std::string& strCompilerOutput, CDynLibVM::CompilationLogVec* pOutput)
        {
            static const std::regex err("^(.*?)\\s*(fatal)?\\s*error\\s*C(.*?)\\s*:(.*?)\n");      
            auto begin = std::sregex_iterator(strCompilerOutput.begin(), strCompilerOutput.end(), err);
            auto end = std::sregex_iterator();
            auto dist = std::distance(begin, end);
            std::string strMatch;
            SCompilationLog Log;

            for (auto i = begin; i != end; ++i)
            {
                auto Match = *i;
                Log.msg = Match.str();
                pOutput->push_back(Log);
            }
            return dist == 0;
        }

        bool LoadLib(const char* pLibName, uint64_t* pLibHandle)
        {
            uint64_t hLib = 0;
            auto err = Platform::API::Library::Load(&hLib, pLibName);
            if (!hLib || err != 0)
            {
                return false;
            }
            *pLibHandle = hLib;
            return true;
        }

        void LoadFuncPointers(uint64_t hLib, CScript::pfnEntryPoint* pPtrs)
        {
            using PfnType = CScript::pfnEntryPoint;
            PfnType pfn = nullptr;
            using Library = Platform::API::Library;
            pfn = reinterpret_cast<PfnType>(Library::GetProcAddr(hLib, "OnLoad"));
            pPtrs[CScript::FuncTypes::ON_LOAD] = pfn;

            pfn = reinterpret_cast<PfnType>(Library::GetProcAddr(hLib, "OnUnload"));
            pPtrs[CScript::FuncTypes::ON_UNLOAD] = pfn;

            void* pPtr = Library::GetProcAddr(hLib, "OnRun");
            pfn = reinterpret_cast<PfnType>(pPtr);
            pPtrs[CScript::FuncTypes::ON_RUN] = pfn;
        }

        bool CDynLibVM::CompileScript(uint32_t scriptId, CompilationLogVec* pvErrors)
        {
            // TMP Config
            //const char* pBatchPath = "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\bin\\vcvars32.bat";
            //const char* pCompilerPath = "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\bin\\cl.exe";
            
            CScript* pScript = _FindScript(scriptId);
            if (!pScript)
            {
                return false;
            }

            return CompileScript(&pScript, pvErrors);
        }

        bool CDynLibVM::CompileScript(CScript** ppScript, CompilationLogVec* pvErrors)
        {
            CScript* pScript = *ppScript;
            UnloadScript(&pScript);

            char pArgs[1024];
            const char* pScriptName = pScript->GetName().c_str();
            char pLogName[512];
            char pLibName[512];
#if _MSC_VER
            sprintf_s(pLogName, sizeof(pLogName), "%s.log", pScriptName);
            sprintf_s(pLibName, sizeof(pLibName), "%s.dll", pScriptName);
            sprintf_s(pArgs, sizeof(pArgs), "%s %s %s > %s", m_Config.compileCmdLine.c_str(), pScriptName, pLibName, pLogName);
#else
            sprintf(pLogName, "%s.log", pScriptName);
            sprintf(pLibName, "%s.dll", pScriptName);
            sprintf(pArgs, "%s %s %s > %s", m_Config.compileCmdLine.c_str(), pScriptName, pLibName, pLogName);
#endif
            auto result = Platform::API::Process::Start(nullptr, pArgs);
            if (!result)
            {
                return false;
            }
            auto hFile = Platform::API::File::Open(pLogName, "r");
            if (!hFile)
            {
                return false;
            }
            auto fileSize = Platform::API::File::GetSize(hFile);
            std::string strFileData;
            strFileData.resize(fileSize);
            Platform::API::File::Read(hFile, &strFileData[0], fileSize, fileSize, 1);
            Platform::API::File::Close(hFile);

            auto res = ParseCompilerOutput(strFileData, pvErrors);
            if (!res)
            {
                return false;
            }

            uint64_t hLib = pScript->GetLibHandle();

            if (!LoadLib(pLibName, &hLib))
            {
                return false;
            }

            pScript->Init(hLib);
            LoadFuncPointers(hLib, pScript->GetFuncTable());
            
            auto pfn = pScript->GetFuncTable()[CScript::FuncTypes::ON_LOAD];
            if (pfn)
                pfn(0);
            return true;
        }

        bool CDynLibVM::RunScript(CScript** ppScript, void* pPtr)
        {
            (*ppScript)->GetFuncTable()[CScript::FuncTypes::ON_RUN](pPtr);
            return true;
        }

        bool CDynLibVM::RunScript(uint32_t scriptId, void* pPtr)
        {          
            CScript* pScript = _FindScript(scriptId);
            if (pScript)
                return RunScript(&pScript, pPtr);
            return false;
        }

        bool CDynLibVM::UnloadScript(uint32_t scriptId)
        {
            CScript* pScript = _FindScript(scriptId);
            if (!pScript)
            {
                return false;
            }
            auto pfn = pScript->GetFuncTable()[CScript::FuncTypes::ON_UNLOAD];
            if (pfn)
                pfn(0);
            return UnloadScript(&pScript);
        }

        bool CDynLibVM::UnloadScript(CScript** ppScript)
        {
            CScript* pScript = *ppScript;
            if (pScript->GetLibHandle())
            {
                auto pfn = pScript->GetFuncTable()[CScript::FuncTypes::ON_UNLOAD];
                if (pfn)
                    pfn(0);
                Platform::API::Library::Unload(pScript->GetLibHandle());
                pScript->Init(0);
                m_vFreeScripts.push_back(pScript->GetID());
                return true;
            }
            return false;
        }

        void CDynLibVM::WatchScripts(UintVec* pvScripts)
        {
            using File = Platform::API::File;
            for (auto& Pair : m_mScripts)
            {
                auto pScript = Pair.second;
                auto ModTime = File::GetModificationTime(pScript->GetName().c_str());
                if (ModTime)
                {
                    auto CurrModTime = pScript->GetModificationTime();
                    if (File::IsFileModified(CurrModTime, ModTime))
                    {
                        pScript->SetModificationTime(ModTime);
                        pvScripts->push_back(pScript->GetID());
                    }
                }
            }
        }

    } // DynamicLibrary
} // SPP