
#include "CMonoVM.h"
#include "../Platform/PlatformAPI.h"
#include "CScript.h"
#include "../Utils/CCompiler.h"
#if SPP_USE_MONO
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace SPP
{
    namespace CSharp
    {
        using ScriptMap = std::map<uint32_t, CScript* >;
        using UintVec = std::vector<uint32_t>;

        CScript* FindScript(ScriptMap& mMap, uint32_t id)
        {
            auto Itr = mMap.find(id);
            if (Itr == mMap.end())
                return nullptr;
            return Itr->second;
        }

        struct CMonoVM::SInternal
        {
            struct
            {
                MonoDomain* pDomain;
            } Mono;

            struct 
            {
                ScriptMap mScripts;
                UintVec vFreeScripts;
                uint32_t currId = 0;
            } Data;
        };

        CMonoVM::CMonoVM()
        {
            m_pInternal = new(std::nothrow) SInternal;
        }

        CMonoVM::~CMonoVM()
        {
            mono_jit_cleanup(m_pInternal->Mono.pDomain);
            delete m_pInternal;
            m_pInternal = nullptr;
        }

        bool CMonoVM::Init()
        {
            auto& Mono = m_pInternal->Mono;
            using Library = Platform::API::Library;
            
            mono_set_dirs(this->m_Config.compilerDir.c_str(), this->m_Config.compilerDir.c_str());          
            auto pDomain = mono_jit_init("Mono");
            Mono.pDomain = pDomain;
            return true;
        }

        uint32_t CMonoVM::LoadScript(const SScriptInfo& Info)
        {
            char dirPath[512];
            Platform::API::Directory::GetCurrentDirectoryPath(dirPath, 512);
            // Check files exist
            for (auto& strFile : Info.vFiles)
            {
                if (!Platform::API::File::Exists(strFile.c_str()))
                {
                    return 0;
                }
            }

            auto& Data = m_pInternal->Data;
            // If there are any free scripts
            CScript* pScript = nullptr;
            uint32_t scriptId = 0;
            if (!Data.vFreeScripts.empty())
            {
                auto id = Data.vFreeScripts.back();
                pScript = FindScript(Data.mScripts, id);
                scriptId = id;
                pScript = new(pScript)CScript(scriptId);
                Data.vFreeScripts.pop_back();
            }
            else
            {
                scriptId = ++Data.currId;
                pScript = new CScript(scriptId);
                Data.mScripts.insert(ScriptMap::value_type(scriptId, pScript));
            }

            pScript->m_Info = Info;
            return scriptId;
        }

        bool CMonoVM::CompileScript(uint32_t scriptId, CompilationLogVec* pvErrors)
        {
            using Process = Platform::API::Process;
            auto pScript = FindScript(m_pInternal->Data.mScripts, scriptId);
            if (pScript)
            {
                auto& Mono = m_pInternal->Mono;
                SPP::Utils::CCompiler Compiler = Utils::CCompiler(m_Config); 
                const auto& Info = pScript->m_Info;
                std::string strOut = Info.name + ".netmodule";
                
                auto res = Compiler.Run(Info.vFiles, strOut);
                MonoAssembly* pVM = mono_domain_assembly_open(Mono.pDomain, strOut.c_str());
                if (!pVM)
                {
                    return false;
                }
                MonoImage* pImg = mono_assembly_get_image(pVM);
                if (!pImg)
                    return false;
                MonoClass* pClass = mono_class_from_name(pImg, "", Info.name.c_str());
                if (!pClass)
                    return false;
                {
                    MonoMethod* pMethod = mono_class_get_method_from_name(pClass, "OnLoad", 0);
                    pScript->m_aFunctions[FunctionTypes::ON_LOAD] = pMethod;
                }
                {
                    MonoMethod* pMethod = mono_class_get_method_from_name(pClass, "OnUnload", 0);
                    pScript->m_aFunctions[FunctionTypes::ON_UNLOAD] = pMethod;
                }
                {
                    MonoMethod* pMethod = mono_class_get_method_from_name(pClass, "OnRun", 0);
                    pScript->m_aFunctions[FunctionTypes::ON_RUN] = pMethod;
                }
                {
                    MonoMethod* pMethod = mono_class_get_method_from_name(pClass, "OnUpdate", 0);
                    pScript->m_aFunctions[FunctionTypes::ON_UPDATE] = pMethod;
                }
                auto pPtr = pScript->m_aFunctions[FunctionTypes::ON_LOAD];
                if (pPtr)
                {
                    mono_runtime_invoke(pPtr, 0, 0, 0);
                }
                return true;
            }
            return false;
        }

        bool CMonoVM::RunScript(uint32_t scriptId, void*)
        {
            return true;
        }

        bool CMonoVM::UnloadScript(uint32_t scriptId)
        {
            return true;
        }
    }
}
#endif // SPP_USE_MONO