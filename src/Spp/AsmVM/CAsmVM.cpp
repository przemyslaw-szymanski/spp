#include "CAsmVM.h"
#include <functional>

namespace SPP
{
	namespace ASM
	{
		using StringToken = char[16];
		using StringVec = std::vector< StringToken >;

		struct SRegister
		{
			uint16_t	tableId;
			uint16_t	regId;
		};

		struct OpCodes
		{
			enum OP_CODE
			{
				NOOP,
				
				MOVS, MOVU, MOVF,
				ADDS, ADDU, ADDF,
				SUBS, SUBU, SUBF,
				MULS, MULU, MULF,
				DIVS, DIVU, DIVF,
				INCS, INCU, INCF,
				DECS, DECU, DECF,

				JMP,
				CALL,

				PRNT,

				CODE_END,
				_ENUM_COUNT
			};
		};
		typedef OpCodes::OP_CODE OP_CODE;

		struct TokenTypes
		{
			enum TYPE
			{
				INT8,
				UINT8,
				INT16,
				UINT16,
				INT32,
				UINT32,
				INT64,
				UINT64,
				FLOAT32,
				STRING,
				_ENUM_COUNT
			};
		};
		typedef TokenTypes::TYPE TOKEN_TYPE;

		struct SRegisterValue
		{
			union
			{
				int8_t		int8;
				uint8_t		uint8;
				int16_t		int16;
				uint16_t	uint16;
				int32_t		int32;
				uint32_t	uint32;
				int64_t		int64;
				uint64_t	uint64;
				float		float32;
				void*		ptr;
			};
		};

		struct RegisterTypes
		{
			enum TYPE
			{
				NORMAL,
				POINTER,
				CONSTANT,
				TEMPORARY,
				_ENUM_COUNT
			};
		};
		typedef RegisterTypes::TYPE REG_TYPE;
		static const uint32_t MAX_REG_COUNT = 128;

		using RegisterTable = SRegisterValue[RegisterTypes::_ENUM_COUNT][MAX_REG_COUNT];
		template<uint32_t _COUNT_>
		using RegisterValues = SRegisterValue*[_COUNT_];
		//using OpcodeFunc = std::function<int32_t(const uint64_t*, RegisterTable)>;
		using OpcodeFunc = int32_t(*)(const uint8_t*, RegisterTable);
		using AluFunc = std::function<void(SRegisterValue**)>;

		struct SInstrReg3
		{
			SInstrReg3() {}
			SInstrReg3(uint64_t data) : raw(data) {}
			
			union
			{
				struct
				{
					uint32_t	reg1TypeBits	: 2;
					uint32_t	reg1IdBits		: 8;
					uint32_t	reg2TypeBits	: 2;
					uint32_t	reg2IdBits		: 8;
					uint32_t	reg3TypeBits	: 2;
					uint32_t	reg3IdBits		: 8;
				};
				uint32_t raw;
			};
		};

		struct SInstrReg2
		{
			SInstrReg2() {}
			SInstrReg2(uint32_t data) : raw(data) {}

			union
			{
				struct
				{
					uint32_t	reg1TypeBits : 2;
					uint32_t	reg1IdBits : 8;
					uint32_t	reg2TypeBits : 2;
					uint32_t	reg2IdBits : 8;
				};
				uint32_t raw;
			};
		};

		struct SInstrReg1
		{
			SInstrReg1() {}
			SInstrReg1(uint16_t data) : raw(data) {}

			union
			{
				struct
				{
					uint32_t	reg1TypeBits : 2;
					uint32_t	reg1IdBits : 8;
				};
				uint16_t raw;
			};
		};

		struct SInstrFuncCall
		{
			SInstrFuncCall() {}
			SInstrFuncCall(uint32_t data) : raw(data) {}

			union
			{
				struct
				{
					uint16_t funcAddr;
					uint8_t pushCount;
					uint8_t retCount;
				};
				uint32_t raw;
			};
		};

		uint32_t AluToInstr( 
			uint32_t reg1Type, uint32_t reg1Id, 
			uint32_t reg2Type, uint32_t reg2Id,
			uint32_t reg3Type, uint32_t reg3Id)
		{
			SInstrReg3 Instr;
			Instr.reg1TypeBits = reg1Type;
			Instr.reg1IdBits = reg1Id;
			Instr.reg2TypeBits = reg2Type;
			Instr.reg2IdBits = reg2Id;
			Instr.reg3TypeBits = reg3Type;
			Instr.reg3IdBits = reg3Id;
			return Instr.raw;
		}

		uint32_t AluToInstr(
			uint32_t reg1Type, uint32_t reg1Id,
			uint32_t reg2Type, uint32_t reg2Id)
		{
			SInstrReg2 Instr;
			Instr.reg1TypeBits = reg1Type;
			Instr.reg1IdBits = reg1Id;
			Instr.reg2TypeBits = reg2Type;
			Instr.reg2IdBits = reg2Id;
			return Instr.raw;
		}

		uint16_t AluToInstr(
			uint32_t reg1Type, uint32_t reg1Id)
		{
			SInstrReg1 Instr;
			Instr.reg1TypeBits = reg1Type;
			Instr.reg1IdBits = reg1Id;
			return Instr.raw;
		}

		uint16_t FuncCallToInstr(uint16_t funcAddr)
		{
			return funcAddr;
		}

		struct SFuncStack
		{
			RegisterTable aaRegisters;
			uint32_t currPtr = 0;
			uint32_t currStackPtr = 0;
		};

		SRegisterValue* GetRegisterPtr(RegisterTable aaRegisters, const SRegister& Reg)
		{
			auto aRegisters = aaRegisters[Reg.tableId];
			return &aRegisters[Reg.regId];
		}

		void GetRegisters(RegisterTable aaRegisters, SInstrReg3 Instr, SRegisterValue* apValues[3])
		{
			apValues[0] = &aaRegisters[Instr.reg1TypeBits][Instr.reg1IdBits];
			apValues[1] = &aaRegisters[Instr.reg2TypeBits][Instr.reg2IdBits];
			apValues[2] = &aaRegisters[Instr.reg3TypeBits][Instr.reg3IdBits];
		}

		void GetRegisters(RegisterTable aaRegisters, SInstrReg2 Instr, SRegisterValue* apValues[2])
		{
			apValues[0] = &aaRegisters[Instr.reg1TypeBits][Instr.reg1IdBits];
			apValues[1] = &aaRegisters[Instr.reg2TypeBits][Instr.reg2IdBits];
		}

		void GetRegisters(RegisterTable aaRegisters, SInstrReg1 Instr, SRegisterValue* apValues[1])
		{
			apValues[0] = &aaRegisters[Instr.reg1TypeBits][Instr.reg1IdBits];
		}

		int32_t Alu(const uint8_t* pMem, RegisterTable aaRegisters, RegisterValues<3> Values)
		{
			SInstrReg3 Instr(*reinterpret_cast<const uint64_t*>(pMem));
			GetRegisters(aaRegisters, Instr, Values);			
			return sizeof(Instr.raw);
		}

#define DEF_OPCODE(_name) int32_t _name(const uint8_t* pMem, RegisterTable aaRegisters)

		DEF_OPCODE(Adds)
		{
			RegisterValues<3> Values;
			auto bytes = Alu(pMem, aaRegisters, Values);
			Values[0]->int32 = Values[1]->int32 + Values[2]->int32;
			return bytes;
		}

		DEF_OPCODE(Subs)
		{
			RegisterValues<3> Values;
			auto bytes = Alu(pMem, aaRegisters, Values);
			Values[0]->int32 = Values[1]->int32 - Values[2]->int32;
			return bytes;
		}

		DEF_OPCODE(Muls)
		{
			RegisterValues<3> Values;
			auto bytes = Alu(pMem, aaRegisters, Values);
			Values[0]->int32 = Values[1]->int32 * Values[2]->int32;
			return bytes;
		}

		DEF_OPCODE(Divs)
		{
			RegisterValues<3> Values;
			auto bytes = Alu(pMem, aaRegisters, Values);
			Values[0]->int32 = Values[1]->int32 / Values[2]->int32;
			return bytes;
		}

		DEF_OPCODE(Prnt)
		{
			RegisterValues<1> Values;
			SInstrReg1 Instr(*reinterpret_cast<const uint16_t*>(pMem));
			GetRegisters(aaRegisters, Instr, Values);
			printf("%d\n", Values[0]->int32);
			//return ret;
			return sizeof(Instr.raw);
		}

		DEF_OPCODE(Call)
		{
			// Read func address = 4 bytes
			const uint8_t* pFunc = reinterpret_cast<const uint8_t*>(pMem);
			uint32_t funcAddr[4] = { pFunc[0], pFunc[1], pFunc[2], pFunc[3] };

			return 4;
		}

		DEF_OPCODE(CodeEnd)
		{
			return -1;
		}
		
		bool TokenEquals(const StringToken& left, const StringToken& right)
		{
			return strcmp(left, right) == 0;
		}

		static const std::string g_strISA = SPP_TO_STRING(
			add r1.i32 c0.i32 c1.i32
		);

		static const std::string g_strASM = SPP_TO_STRING(
			addi r1 c0 c1
		);

		bool ParseISA()
		{
			return false;
		}

		bool ParseASM()
		{
			return false;
		}

		OP_CODE DecodeOpCode(uint64_t instr)
		{
			auto rawOp = (instr & 0x00000000000000FF);
			return static_cast<OP_CODE>(rawOp);
		}

		bool RunOpCodes(SFuncStack& Stack, uint8_t* pMem, uint8_t* pFuncMem)
		{
			bool eof = false;
			auto pCurrPtr = pMem;

			OpcodeFunc aOpFuncPtrs[OpCodes::_ENUM_COUNT] = { 0 };
			aOpFuncPtrs[OpCodes::CODE_END] = CodeEnd;
			aOpFuncPtrs[OpCodes::ADDS] = Adds;
			aOpFuncPtrs[OpCodes::MULS] = Muls;
			aOpFuncPtrs[OpCodes::SUBS] = Subs;
			aOpFuncPtrs[OpCodes::DIVS] = Divs;
			aOpFuncPtrs[OpCodes::PRNT] = Prnt;
			aOpFuncPtrs[OpCodes::CALL] = Call;

			while (!eof)
			{
				//OP_CODE currInstr = static_cast<OP_CODE>(*pCurrPtr);
				auto currOpcode = static_cast<OP_CODE>(*pCurrPtr);
				if (currOpcode == OpCodes::CALL)
				{
					pCurrPtr++;
					uint16_t funcPtr = *reinterpret_cast<uint16_t*>(pCurrPtr);
					pCurrPtr += sizeof(uint16_t);
					RunOpCodes(Stack, pFuncMem + funcPtr, pFuncMem);
				}
				else
				{
					OpcodeFunc Func = aOpFuncPtrs[currOpcode];
					auto instrCount = Func(++pCurrPtr, Stack.aaRegisters);
					eof = instrCount < 0;
					pCurrPtr += instrCount;
				}
			}
			return false;
		}

		template<typename _INSTR_TYPE_>
		uint32_t AddInstruction(uint8_t* pMem, OP_CODE opCode, _INSTR_TYPE_ instr)
		{
			*pMem = opCode;
			pMem++;
			_INSTR_TYPE_* pPtr = reinterpret_cast<_INSTR_TYPE_*>(pMem);
			*pPtr = instr;
			auto size = sizeof(_INSTR_TYPE_) + 1;
			return size;
		}

		uint32_t AddInstruction(uint8_t* pMem, OP_CODE opCode)
		{
			*pMem = opCode;
			return 1;
		}

		CAsmVM::CAsmVM()
		{}

		uint32_t CAsmVM::LoadScript(const SScriptInfo& Info)
		{
			uint8_t aMem[1024 * 4] = { 0 };
			uint8_t* pMem8 = aMem;
			pMem8 += AddInstruction(pMem8, OpCodes::ADDS, AluToInstr(0,1, 2,1, 2,2));
			pMem8 += AddInstruction(pMem8, OpCodes::ADDS, AluToInstr(0, 2, 2, 1, 2, 2));
			pMem8 += AddInstruction(pMem8, OpCodes::MULS, AluToInstr(0, 3, 0, 1, 0, 2));
			pMem8 += AddInstruction(pMem8, OpCodes::PRNT, AluToInstr(0, 1));
			pMem8 += AddInstruction(pMem8, OpCodes::PRNT, AluToInstr(0, 2));
			pMem8 += AddInstruction(pMem8, OpCodes::PRNT, AluToInstr(0, 3));
			pMem8 += AddInstruction(pMem8, OpCodes::CALL, FuncCallToInstr(0));
			pMem8 += AddInstruction(pMem8, OpCodes::CODE_END);

			uint8_t aFuncMem[1024 * 4] = { 0 };
			pMem8 = aFuncMem;
			pMem8 += AddInstruction(pMem8, OpCodes::ADDS, AluToInstr(0,5, 2,1, 2,2));
			pMem8 += AddInstruction(pMem8, OpCodes::PRNT, AluToInstr(0,5));
			pMem8 += AddInstruction(pMem8, OpCodes::CODE_END);

			SFuncStack Stack, FuncStack;
			Stack.aaRegisters[RegisterTypes::CONSTANT][1].int32 = 3;
			Stack.aaRegisters[RegisterTypes::CONSTANT][2].int32 = 4;

			

			RunOpCodes(Stack, aMem, aFuncMem);
			return 0;
		}

		bool CAsmVM::CompileScript(uint32_t scriptId, CompilationLogVec* pvErrors)
		{
			return true;
		}

		bool CAsmVM::RunScript(uint32_t scriptId, void*)
		{
			return true;
		}

		bool CAsmVM::UnloadScript(uint32_t scriptId)
		{
			return true;
		}
	} // ASM
} // SPP