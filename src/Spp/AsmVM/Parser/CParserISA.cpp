#include "CParserISA.h"
#include <sstream>
#include <iterator>

namespace SPP
{
	namespace ASM
	{
		using SOperand = CParserISA::SOperand;
		using OperandVec = CParserISA::OperandVec;
		using InstrVec = CParserISA::InstrVec;

		struct InstrKeywords
		{
			enum INSTR
			{
				UNKNOWN,
				MOV, ADD, SUB, MUL, DIV, INC, DEC,
				PUSH, POP,
				CALL,
				JMP, RET,
				_ENUM_COUNT
			};
		};
		using INSTR_KEYWORD = InstrKeywords::INSTR;
		using ParseFuncPtr = int32_t(*)(const TextTokenVec&, uint32_t, OperandVec*, InstrVec*);

		struct SParseFunc
		{
			ParseFuncPtr Parse = nullptr;
		};

		using ParseFuncMap = std::map<std::string, SParseFunc>;
		static ParseFuncMap g_mParserFuncs;

		static void Init();

		CParserISA::CParserISA()
		{
			Init();
		}

		bool CParserISA::Parse(const uint8_t* pData, uint32_t dataSize, TextTokenVec* pOut)
		{
			std::string strData = reinterpret_cast<const char*>(pData);
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
			uint32_t minTokenCount, uint32_t maxTokenCount, CParserISA::OperandVec* pOut)
		{
			// Read until ;
			CParserISA::OperandVec vOps;
			ParseOperands(vTokens, currTokenId, &vOps);
			if (vOps.size() < minTokenCount || vOps.size() > maxTokenCount)
			{
				return -1;
			}
			return vOps.size();
		}

		int32_t ParseAlu(const TextTokenVec& vTokens, uint32_t currTokenId,
			uint32_t minTokenCount, uint32_t maxTokenCount, TextTokenVec* pOut)
		{
			// Read until ;
			CParserISA::OperandVec vOps;
			ParseOperands(vTokens, currTokenId, &vOps);
			if (vOps.size() < minTokenCount || vOps.size() > maxTokenCount)
			{
				return -1;
			}
			return vOps.size();
		}

		int32_t ParseInstr(const CParserISA::OperandVec& vOps, CParserISA::SInstruction* pOut)
		{

		}

		bool CreateInstruction(INSTR_KEYWORD keyword, const CParserISA::OperandVec& vOps,
			CParserISA::SInstruction* pOut)
		{
			

			return false; // unknown instruction
		}

		bool CParserISA::Parse(const TextTokenVec& vTokens, InstrVec* pvOut)
		{
			size_t currTokenId = 0;
			const auto tokenCount = vTokens.size();
			TextTokenVec vTmpTokens;
			OperandVec vOperands;

			while (currTokenId < tokenCount)
			{
				vTmpTokens.clear();
				vOperands.clear();
				const auto& currToken = vTokens[currTokenId++];
				auto res = g_mParserFuncs[currToken].Parse(vTokens, currTokenId, &vOperands, pvOut);
				if (res < 0)
				{
					break;
				}
				currTokenId += res;
			}
			return true;
		}

		namespace ParseFunc
		{
			int32_t ParseAlu(const TextTokenVec& vTokens, uint32_t currTokenId, OperandVec* pOps, InstrVec* pInstr)
			{
				ParseOperands(vTokens, currTokenId, pOps);
				return -1;
			}
		} // ParseFunc

		void Init()
		{
			g_mParserFuncs["add"] = { ParseFunc::ParseAlu };
		}
	} // ASM
} // SPP