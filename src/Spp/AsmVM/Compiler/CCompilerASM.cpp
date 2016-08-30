#include "CCompilerASM.h"
#include "../VMTypes.h"

namespace SPP
{
	namespace ASM
	{
		int32_t ParseAlu3(const TextTokenVec& vTokens, uint32_t currTokenId, ByteCodeVec* pOut)
		{
			int32_t operandId = 0;
			SInstrReg3 Instr;

			{
				const auto& strTableId = vTokens[currTokenId + operandId++];
				const auto& strRegId = vTokens[currTokenId + operandId++];
				Instr.reg1TypeBits = atoi(strTableId.c_str());
				Instr.reg1IdBits = atoi(strRegId.c_str());
			}
			{
				const auto& strTableId = vTokens[currTokenId + operandId++];
				const auto& strRegId = vTokens[currTokenId + operandId++];
				Instr.reg2TypeBits = atoi(strTableId.c_str());
				Instr.reg2IdBits = atoi(strRegId.c_str());
			}
			{
				const auto& strTableId = vTokens[currTokenId + operandId++];
				const auto& strRegId = vTokens[currTokenId + operandId++];
				Instr.reg3TypeBits = atoi(strTableId.c_str());
				Instr.reg3IdBits = atoi(strRegId.c_str());
			}
			
			return operandId;
		}

		bool CCompilerASM::Compile(const TextTokenVec& vTokens, ByteCodeVec* pOut)
		{
			size_t currTokenId = 0;
			const auto tokenCount = vTokens.size();
			while (currTokenId < tokenCount)
			{
				const auto& currToken = vTokens[currTokenId];
				if (currToken == "addiii")
				{
					pOut->push_back(OpCodes::ADDIII);
					currTokenId += ParseAlu3(vTokens, currTokenId, pOut);
				}
			}
			return true;
		}
	} // ASM
} // SPP