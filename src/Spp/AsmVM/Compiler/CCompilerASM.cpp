#include "CCompilerASM.h"
#include "../VMTypes.h"

namespace SPP
{
	namespace ASM
	{
		using ParseFunc = int32_t(*)(const TextTokenVec&, uint32_t, SByteCodeBuffer*);

		enum INSTR_TYPE
		{
			ALU1,
			ALU2,
			ALU3,
			_MAX_INSTR_COUNT
		};

		static ParseFunc g_aParseFuncs[_MAX_INSTR_COUNT];

		struct SOpcodeParse
		{
			ParseFunc Func;
			OP_CODE opCode;
		};

		using ParseMap = std::map<std::string, SOpcodeParse>;
		static ParseMap g_mOpcodeParsers;

		int32_t ParseAlu3(const TextTokenVec& vTokens, uint32_t currTokenId, SByteCodeBuffer* pOut)
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

			if (!pOut->AppendData(Instr.raw))
			{
				return -1;
			}
			return operandId;
		}

		int32_t ParseAlu2(const TextTokenVec& vTokens, uint32_t currTokenId, SByteCodeBuffer* pOut)
		{
			int32_t operandId = 0;
			SInstrReg2 Instr;

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

			if (!pOut->AppendData(Instr.raw))
			{
				return -1;
			}
			return operandId;
		}

		int32_t ParseAlu1(const TextTokenVec& vTokens, uint32_t currTokenId, SByteCodeBuffer* pOut)
		{
			int32_t operandId = 0;
			SInstrReg1 Instr;

			{
				const auto& strTableId = vTokens[currTokenId + operandId++];
				const auto& strRegId = vTokens[currTokenId + operandId++];
				Instr.reg1TypeBits = atoi(strTableId.c_str());
				Instr.reg1IdBits = atoi(strRegId.c_str());
			}

			if (!pOut->AppendData(Instr.raw))
			{
				return -1;
			}
			return operandId;
		}

		int32_t ParseOpcode(const SOpcodeParse& Parse, const TextTokenVec& vTokens, uint32_t currTokenId,
			SByteCodeBuffer* pOut)
		{
			if (!pOut->AppendData<uint8_t>(Parse.opCode))
			{
				return -1;
			}
			currTokenId++;
			return Parse.Func(vTokens, currTokenId, pOut) + 1;
		}

		CCompilerASM::CCompilerASM()
		{
			Init();
		}

#define DEF_OPCODE_PARSER(_opName, _opFunc) g_mOpcodeParsers[#_opName] = { _opFunc, OpCodes::_opName }

		bool CCompilerASM::Compile(const TextTokenVec& vTokens, SByteCodeBuffer* pOut)
		{
			size_t currTokenId = 0;
			const auto tokenCount = vTokens.size();
			
			if (!pOut->Create(1024 * 1024))
			{
				return false;
			}

			while (currTokenId < tokenCount)
			{
				const auto& currToken = vTokens[currTokenId];
				auto Itr = g_mOpcodeParsers.find(currToken);
				if (Itr != g_mOpcodeParsers.end())
				{
					auto res = ParseOpcode(Itr->second, vTokens, currTokenId, pOut);
					if (res > 0)
					{
						currTokenId += res;
					}
					else
					{
						break;
					}
				}
			}
			return true;
		}

		void CCompilerASM::Init()
		{
			g_aParseFuncs[ALU1] = ParseAlu1;
			g_aParseFuncs[ALU2] = ParseAlu2;
			g_aParseFuncs[ALU3] = ParseAlu3;

			DEF_OPCODE_PARSER(MOVII, ParseAlu2);
			DEF_OPCODE_PARSER(MOVIU, ParseAlu2);
			DEF_OPCODE_PARSER(MOVIF, ParseAlu2);
			DEF_OPCODE_PARSER(MOVUU, ParseAlu2);
			DEF_OPCODE_PARSER(MOVUI, ParseAlu2);
			DEF_OPCODE_PARSER(MOVUF, ParseAlu2);
			DEF_OPCODE_PARSER(MOVFF, ParseAlu2);
			DEF_OPCODE_PARSER(MOVFU, ParseAlu2);
			DEF_OPCODE_PARSER(MOVFI, ParseAlu2);
			DEF_OPCODE_PARSER(MOVP, ParseAlu2);

			DEF_OPCODE_PARSER(ADDII, ParseAlu2);
			DEF_OPCODE_PARSER(ADDIU, ParseAlu2);
			DEF_OPCODE_PARSER(ADDIF, ParseAlu2);
			DEF_OPCODE_PARSER(ADDUU, ParseAlu2);
			DEF_OPCODE_PARSER(ADDUI, ParseAlu2);
			DEF_OPCODE_PARSER(ADDUF, ParseAlu2);
			DEF_OPCODE_PARSER(ADDFF, ParseAlu2);
			DEF_OPCODE_PARSER(ADDFU, ParseAlu2);
			DEF_OPCODE_PARSER(ADDFI, ParseAlu2);

			DEF_OPCODE_PARSER(SUBII, ParseAlu2);
			DEF_OPCODE_PARSER(SUBIU, ParseAlu2);
			DEF_OPCODE_PARSER(SUBIF, ParseAlu2);
			DEF_OPCODE_PARSER(SUBUU, ParseAlu2);
			DEF_OPCODE_PARSER(SUBUI, ParseAlu2);
			DEF_OPCODE_PARSER(SUBUF, ParseAlu2);
			DEF_OPCODE_PARSER(SUBFF, ParseAlu2);
			DEF_OPCODE_PARSER(SUBFU, ParseAlu2);
			DEF_OPCODE_PARSER(SUBFI, ParseAlu2);

			DEF_OPCODE_PARSER(MULII, ParseAlu2);
			DEF_OPCODE_PARSER(MULIU, ParseAlu2);
			DEF_OPCODE_PARSER(MULIF, ParseAlu2);
			DEF_OPCODE_PARSER(MULUU, ParseAlu2);
			DEF_OPCODE_PARSER(MULUI, ParseAlu2);
			DEF_OPCODE_PARSER(MULUF, ParseAlu2);
			DEF_OPCODE_PARSER(MULFF, ParseAlu2);
			DEF_OPCODE_PARSER(MULFU, ParseAlu2);
			DEF_OPCODE_PARSER(MULFI, ParseAlu2);

			DEF_OPCODE_PARSER(DIVII, ParseAlu2);
			DEF_OPCODE_PARSER(DIVIU, ParseAlu2);
			DEF_OPCODE_PARSER(DIVIF, ParseAlu2);
			DEF_OPCODE_PARSER(DIVUU, ParseAlu2);
			DEF_OPCODE_PARSER(DIVUI, ParseAlu2);
			DEF_OPCODE_PARSER(DIVUF, ParseAlu2);
			DEF_OPCODE_PARSER(DIVFF, ParseAlu2);
			DEF_OPCODE_PARSER(DIVFU, ParseAlu2);
			DEF_OPCODE_PARSER(DIVFI, ParseAlu2);

			DEF_OPCODE_PARSER(INCI, ParseAlu1);
			DEF_OPCODE_PARSER(INCU, ParseAlu1);
			DEF_OPCODE_PARSER(DECI, ParseAlu1);
			DEF_OPCODE_PARSER(DECU, ParseAlu1);

			DEF_OPCODE_PARSER(ADDIII, ParseAlu3);
			DEF_OPCODE_PARSER(ADDIIU, ParseAlu3);
			DEF_OPCODE_PARSER(ADDIUU, ParseAlu3);
			DEF_OPCODE_PARSER(ADDIUI, ParseAlu3);
			DEF_OPCODE_PARSER(ADDIIF, ParseAlu3);
			DEF_OPCODE_PARSER(ADDIFF, ParseAlu3);
			DEF_OPCODE_PARSER(ADDIFI, ParseAlu3);
			DEF_OPCODE_PARSER(ADDUUU, ParseAlu3);
			DEF_OPCODE_PARSER(ADDUUI, ParseAlu3);
			DEF_OPCODE_PARSER(ADDUII, ParseAlu3);
			DEF_OPCODE_PARSER(ADDUIU, ParseAlu3);
			DEF_OPCODE_PARSER(ADDUUF, ParseAlu3);
			DEF_OPCODE_PARSER(ADDUFF, ParseAlu3);
			DEF_OPCODE_PARSER(ADDUFU, ParseAlu3);
			DEF_OPCODE_PARSER(ADDFFF, ParseAlu3);
			DEF_OPCODE_PARSER(ADDFFU, ParseAlu3);
			DEF_OPCODE_PARSER(ADDFUU, ParseAlu3);
			DEF_OPCODE_PARSER(ADDFUF, ParseAlu3);
			DEF_OPCODE_PARSER(ADDFFI, ParseAlu3);
			DEF_OPCODE_PARSER(ADDFII, ParseAlu3);
			DEF_OPCODE_PARSER(ADDFIF, ParseAlu3);

			DEF_OPCODE_PARSER(SUBIII, ParseAlu3);
			DEF_OPCODE_PARSER(SUBIIU, ParseAlu3);
			DEF_OPCODE_PARSER(SUBIUU, ParseAlu3);
			DEF_OPCODE_PARSER(SUBIUI, ParseAlu3);
			DEF_OPCODE_PARSER(SUBIIF, ParseAlu3);
			DEF_OPCODE_PARSER(SUBIFF, ParseAlu3);
			DEF_OPCODE_PARSER(SUBIFI, ParseAlu3);
			DEF_OPCODE_PARSER(SUBUUU, ParseAlu3);
			DEF_OPCODE_PARSER(SUBUUI, ParseAlu3);
			DEF_OPCODE_PARSER(SUBUII, ParseAlu3);
			DEF_OPCODE_PARSER(SUBUIU, ParseAlu3);
			DEF_OPCODE_PARSER(SUBUUF, ParseAlu3);
			DEF_OPCODE_PARSER(SUBUFF, ParseAlu3);
			DEF_OPCODE_PARSER(SUBUFU, ParseAlu3);
			DEF_OPCODE_PARSER(SUBFFF, ParseAlu3);
			DEF_OPCODE_PARSER(SUBFFU, ParseAlu3);
			DEF_OPCODE_PARSER(SUBFUU, ParseAlu3);
			DEF_OPCODE_PARSER(SUBFUF, ParseAlu3);
			DEF_OPCODE_PARSER(SUBFFI, ParseAlu3);
			DEF_OPCODE_PARSER(SUBFII, ParseAlu3);
			DEF_OPCODE_PARSER(SUBFIF, ParseAlu3);

			DEF_OPCODE_PARSER(MULIII, ParseAlu3);
			DEF_OPCODE_PARSER(MULIIU, ParseAlu3);
			DEF_OPCODE_PARSER(MULIUU, ParseAlu3);
			DEF_OPCODE_PARSER(MULIUI, ParseAlu3);
			DEF_OPCODE_PARSER(MULIIF, ParseAlu3);
			DEF_OPCODE_PARSER(MULIFF, ParseAlu3);
			DEF_OPCODE_PARSER(MULIFI, ParseAlu3);
			DEF_OPCODE_PARSER(MULUUU, ParseAlu3);
			DEF_OPCODE_PARSER(MULUUI, ParseAlu3);
			DEF_OPCODE_PARSER(MULUII, ParseAlu3);
			DEF_OPCODE_PARSER(MULUIU, ParseAlu3);
			DEF_OPCODE_PARSER(MULUUF, ParseAlu3);
			DEF_OPCODE_PARSER(MULUFF, ParseAlu3);
			DEF_OPCODE_PARSER(MULUFU, ParseAlu3);
			DEF_OPCODE_PARSER(MULFFF, ParseAlu3);
			DEF_OPCODE_PARSER(MULFFU, ParseAlu3);
			DEF_OPCODE_PARSER(MULFUU, ParseAlu3);
			DEF_OPCODE_PARSER(MULFUF, ParseAlu3);
			DEF_OPCODE_PARSER(MULFFI, ParseAlu3);
			DEF_OPCODE_PARSER(MULFII, ParseAlu3);
			DEF_OPCODE_PARSER(MULFIF, ParseAlu3);

			DEF_OPCODE_PARSER(DIVIII, ParseAlu3);
			DEF_OPCODE_PARSER(DIVIIU, ParseAlu3);
			DEF_OPCODE_PARSER(DIVIUU, ParseAlu3);
			DEF_OPCODE_PARSER(DIVIUI, ParseAlu3);
			DEF_OPCODE_PARSER(DIVIIF, ParseAlu3);
			DEF_OPCODE_PARSER(DIVIFF, ParseAlu3);
			DEF_OPCODE_PARSER(DIVIFI, ParseAlu3);
			DEF_OPCODE_PARSER(DIVUUU, ParseAlu3);
			DEF_OPCODE_PARSER(DIVUUI, ParseAlu3);
			DEF_OPCODE_PARSER(DIVUII, ParseAlu3);
			DEF_OPCODE_PARSER(DIVUIU, ParseAlu3);
			DEF_OPCODE_PARSER(DIVUUF, ParseAlu3);
			DEF_OPCODE_PARSER(DIVUFF, ParseAlu3);
			DEF_OPCODE_PARSER(DIVUFU, ParseAlu3);
			DEF_OPCODE_PARSER(DIVFFF, ParseAlu3);
			DEF_OPCODE_PARSER(DIVFFU, ParseAlu3);
			DEF_OPCODE_PARSER(DIVFUU, ParseAlu3);
			DEF_OPCODE_PARSER(DIVFUF, ParseAlu3);
			DEF_OPCODE_PARSER(DIVFFI, ParseAlu3);
			DEF_OPCODE_PARSER(DIVFII, ParseAlu3);
			DEF_OPCODE_PARSER(DIVFIF, ParseAlu3);

			DEF_OPCODE_PARSER(JMP, ParseAlu1);
			DEF_OPCODE_PARSER(CALL, ParseAlu1);
			DEF_OPCODE_PARSER(PUSHR1, ParseAlu1);
			DEF_OPCODE_PARSER(PUSHR2, ParseAlu2);
			DEF_OPCODE_PARSER(PUSHR3, ParseAlu3);
			DEF_OPCODE_PARSER(POPR1, ParseAlu1);
			DEF_OPCODE_PARSER(POPR2, ParseAlu2);
			DEF_OPCODE_PARSER(POPR3, ParseAlu3);

			DEF_OPCODE_PARSER(PRNT, ParseAlu1);
		}
	} // ASM
} // SPP