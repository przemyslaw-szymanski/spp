#pragma once

#include "../ParserTypes.h"

namespace SPP
{
	namespace ASM
	{
		class CCompilerASM
		{
			public:

				bool Compile(const TextTokenVec& vTokens, ByteCodeVec* pOut);
		};
	} // ASM
} // SPP