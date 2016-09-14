#include "CParserISA.h"
#include <sstream>
#include <iterator>

namespace SPP
{
	namespace ASM
	{
		using SOperand = CParserISA::SOperand;

		bool CParserISA::Parse(const uint8_t* pData, uint32_t dataSize, TextTokenVec* pOut)
		{
			std::string strData = reinterpret_cast<const char*>(pData);
			static const std::string strSearch = ";";
			// Find all ; and add ' ' before
			for (size_t pos = 0; pos < strData.length(); ++pos)
			{
				if (strData[pos] == ';')
				{
					strData.insert(pos++, " ");
				}
			}

			std::istringstream iss(strData);
			std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(*pOut));
			return true;
		}

		void ParseOperands(const TextTokenVec& vTokens, uint32_t currTokenId, CParserISA::OperandVec* pOut)
		{
			// Read until ;
			uint32_t tokenCount = vTokens.size();
			for (uint32_t i = currTokenId; i < tokenCount; ++i)
			{
				const auto& token = vTokens[i];
				if (token == ";")
					break;

				SOperand Op;
				if (token[0] == 'r')
				{
					Op.type = SOperand::OPERAND_TYPE::REGISTER_0;
				}
				else if (token[0] == 'p')
				{
					Op.type = SOperand::OPERAND_TYPE::REGISTER_1;
				}
				else if (token[0] == 'c')
				{
					Op.type = SOperand::OPERAND_TYPE::REGISTER_2;
				}
				if (Op.IsRegister())
				{
					char regType, regId, dataType[3];
					auto res = sscanf(token.c_str(), "%c%d.%s", &regType, &regId, dataType);
					Op.SetDataType(dataType);
					Op.id = regId;
				}

				pOut->push_back(Op);
			}
		}

		int32_t ParseAlu(const TextTokenVec& vTokens, uint32_t currTokenId,
			uint32_t minTokenCount, uint32_t maxTokenCount, TextTokenVec* pOut)
		{
			// Read until ;
			CParserISA::OperandVec vOps;
			ParseOperands(vTokens, currTokenId, &vOps);
			return -1;
		}

		bool CParserISA::Parse(const TextTokenVec& vTokens, InstrVec* pvOut)
		{
			size_t currTokenId = 0;
			const auto tokenCount = vTokens.size();
			TextTokenVec vTmpTokens;

			while (currTokenId < tokenCount)
			{
				vTmpTokens.clear();
				const auto& currToken = vTokens[currTokenId++];
				if (currToken == "add")
				{
					currTokenId += ParseAlu(vTokens, currTokenId, 2, 3, &vTmpTokens);
				}
			}
			return true;
		}
	} // ASM
} // SPP