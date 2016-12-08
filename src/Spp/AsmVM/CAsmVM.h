#pragma once

#include "spp/IVirtualMachine.h"

namespace SPP
{
	namespace ASM
	{
		class CAsmVM : public IVirtualMachine
		{
		public:

			CAsmVM();

			virtual
				bool        Init() { return true; }

			virtual
				void        SetConfig(const SConfig& Cfg) { m_Config = Cfg; }

			virtual
				uint32_t    LoadScript(const SScriptInfo&) override;

			virtual
				bool        CompileScript(uint32_t scriptId, CompilationLogVec* pvErrors) override;

			virtual
				bool        RunScript(uint32_t scriptId, void*) override;

			virtual
				bool        UnloadScript(uint32_t scriptId) override;

			virtual
				void        WatchScripts(UintVec* pScripts) { pScripts->clear(); }
		};
	} // ASM
} // SPP