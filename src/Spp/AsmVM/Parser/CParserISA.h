#pragma once

#include "../ParserTypes.h"
#include "../VMTypes.h"

namespace SPP
{
	namespace ASM
	{
		class CParserISA
		{
			public:

				struct SOperand
				{
					enum class OPERAND_TYPE : uint8_t
					{
						REGISTER_0,
						REGISTER_1,
						REGISTER_2,
					};

					OPERAND_TYPE	type;
					uint8_t			id;
					DATA_TYPE		dataType;

					bool IsRegister() const { return type == OPERAND_TYPE::REGISTER_0 || type == OPERAND_TYPE::REGISTER_1 || type == OPERAND_TYPE::REGISTER_2; }

					bool SetDataType(const char pType[3])
					{
						if (strcmp(pType, "i32") == 0)
						{
							dataType = DataTypes::INT32;
						}
						else if (strcmp(pType, "u32") == 0)
						{
							dataType = DataTypes::UINT32;
						}
						else if (strcmp(pType, "f32") == 0)
						{
							dataType = DataTypes::FLOAT32;
						}
						else if (strcmp(pType, "ptr") == 0)
						{
							dataType = DataTypes::PTR;
						}
						else if (strcmp(pType, "str") == 0)
						{
							dataType = DataTypes::STRING;
						}
						else
						{
							return false;
						}
						return true;
					}
				};

				struct SInstruction
				{
					OP_CODE		op;
					uint8_t		operandCount = 0;
					SOperand	aOperands[16];
				};

				using OperandVec = std::vector< SOperand >;
				using InstrVec = std::vector< SInstruction >;

			public:

				bool Parse(const uint8_t* pData, uint32_t dataSize, TextTokenVec* pOut);
				bool Parse(const TextTokenVec& vTokens, InstrVec* pvOut);
		};
	} // ASM
} // SPP