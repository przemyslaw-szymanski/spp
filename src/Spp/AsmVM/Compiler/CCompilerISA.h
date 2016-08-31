#pragma once

#include "../Parser/CParserISA.h"

namespace SPP
{
	namespace ASM
	{
		class CCompilerISA
		{
			public:

				CCompilerISA();

				static void Init();

				bool Compile(const CParserISA::InstrVec& vTokens, TextTokenVec* pOut);
		};
	} // ASM
} // SPP