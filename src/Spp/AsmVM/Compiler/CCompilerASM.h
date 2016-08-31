#pragma once

#include "../ParserTypes.h"

namespace SPP
{
	namespace ASM
	{
		class CCompilerASM
		{
			public:

				CCompilerASM();

				static void Init();

				bool Compile(const TextTokenVec& vTokens, SByteCodeBuffer* pOut);
		};
	} // ASM
} // SPP