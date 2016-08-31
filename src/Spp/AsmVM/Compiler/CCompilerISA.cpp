#include "CCompilerISA.h"
#include "../VMTypes.h"

namespace SPP
{
	namespace ASM
	{
		CCompilerISA::CCompilerISA()
		{
			Init();
		}	

		bool CCompilerISA::Compile(const CParserISA::InstrVec& vISA, TextTokenVec* pOut)
		{
			size_t instrCount = vISA.size();
			size_t currInstrId = 0;
			return true;
		}

		void CCompilerISA::Init()
		{}
	} // ASM
} // SPP