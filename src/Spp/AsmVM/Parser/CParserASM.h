#pragma once

#include "../ParserTypes.h"

namespace SPP
{
	namespace ASM
	{
		class CParserASM
		{
			public:

				bool Parse(const uint8_t* pData, uint32_t dataSize, TextTokenVec* pOut);
		};
	} // ASM
} // SPP