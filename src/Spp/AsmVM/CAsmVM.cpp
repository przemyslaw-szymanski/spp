#include "CAsmVM.h"
#include "VMInstructionImpl.h"
#include "Parser/CParserASM.h"
#include "Compiler/CCompilerASM.h"
#include "Parser/CParserISA.h"
#include "Compiler/CCompilerISA.h"
#include "LexicalAnalyzer/CLexicalAnalyzerASM.h"
#include "SyntaxAnalyzer/CSyntaxAnalyzerASM.h"

namespace SPP
{
	namespace ASM
	{
		
		void InitOpcodeFunctions(OpcodeFunc aOpFuncPtrs[OpCodes::_ENUM_COUNT]);

		uint32_t OperandToInstr( 
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

		uint32_t OperandToInstr(
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

		uint16_t OperandToInstr(
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

		uint16_t PushToInstr(DATA_TYPE eType, void* pData, uint16_t dataSize)
		{
			uint16_t elemCount = dataSize / sizeof(SFuncStack::STACK_ELEMENT_T);
			return elemCount;
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
			InitOpcodeFunctions(aOpFuncPtrs);
			SOpCodeFuncData FuncData;
			FuncData.pStack = &Stack;
			FuncData.pCodeMemory = pMem;

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
					FuncData.pCodeMemory = ++pCurrPtr;
					auto instrCount = Func(FuncData);
					eof = instrCount < 0;
					pCurrPtr += instrCount;
				}
			}
			return true;
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
			*pMem = static_cast<uint8_t>(opCode);
			return 1;
		}

		CAsmVM::CAsmVM()
		{}

		uint32_t CAsmVM::LoadScript(const SScriptInfo& Info)
		{
			uint8_t aMem[1024 * 4] = { 0 };
			uint8_t* pMem8 = aMem;
			pMem8 += AddInstruction(pMem8, OpCodes::ADDIII, OperandToInstr(0,1, 2,1, 2,2));
			pMem8 += AddInstruction(pMem8, OpCodes::ADDIII, OperandToInstr(0, 2, 2, 1, 2, 2));
			pMem8 += AddInstruction(pMem8, OpCodes::MULIII, OperandToInstr(0, 3, 0, 1, 0, 2));
			pMem8 += AddInstruction(pMem8, OpCodes::PRNT, OperandToInstr(0, 1));
			pMem8 += AddInstruction(pMem8, OpCodes::PRNT, OperandToInstr(0, 2));
			pMem8 += AddInstruction(pMem8, OpCodes::PRNT, OperandToInstr(0, 3));
			pMem8 += AddInstruction(pMem8, OpCodes::PUSHR1, OperandToInstr(1,1));
			pMem8 += AddInstruction(pMem8, OpCodes::PUSHR1, OperandToInstr(1,2));
			pMem8 += AddInstruction(pMem8, OpCodes::CALL, FuncCallToInstr(0));
			pMem8 += AddInstruction(pMem8, OpCodes::CODE_END);

			uint8_t aFuncMem[1024 * 4] = { 0 };
			pMem8 = aFuncMem;
			pMem8 += AddInstruction(pMem8, OpCodes::POPR1, OperandToInstr(0, 4));
			pMem8 += AddInstruction(pMem8, OpCodes::POPR1, OperandToInstr(0, 5));
			pMem8 += AddInstruction(pMem8, OpCodes::ADDIII, OperandToInstr(0,6, 2,1, 2,2));
			pMem8 += AddInstruction(pMem8, OpCodes::PRNT, OperandToInstr(0,6));
			pMem8 += AddInstruction(pMem8, OpCodes::PRNT, OperandToInstr(0,4));
			pMem8 += AddInstruction(pMem8, OpCodes::PRNT, OperandToInstr(0,5));
			pMem8 += AddInstruction(pMem8, OpCodes::CODE_END);

			SFuncStack Stack, FuncStack;
			Stack.aaRegisters[RegisterTypes::CONSTANT][1].Value.int32 = 3;
			Stack.aaRegisters[RegisterTypes::CONSTANT][2].Value.int32 = 4;
			Stack.aaRegisters[RegisterTypes::HEAP][1].Value.ptr = "Hello World";
			Stack.aaRegisters[RegisterTypes::HEAP][2].Value.uint32 = strlen("Hello World");

			SFuncStack tmp;
			int tmpI = 123, i32;
			float tmpF = 3.1415f, f32;
			/*uint16_t buff[4];
			tmp.Push(&tmpI, 4);
			tmp.Push(&tmpF, 4);
			tmp.Pop(buff);
			f32 = *reinterpret_cast<float*>(buff);
			tmp.Pop(buff);
			i32 = *reinterpret_cast<int*>(buff);*/

			RunOpCodes(Stack, aMem, aFuncMem);
			return 0;
		}

		bool CAsmVM::CompileScript(uint32_t scriptId, CompilationLogVec* pvErrors)
		{
			const char* pISA = SPP_TO_STRING(
				add r1.i32 c1.i32 c2.i32;
				prnt r1;
			);
			const char* pAsm = SPP_TO_STRING(
				ADDIII 0 1 2 1 2 2
				PRNT 0 1
			);

			CParserISA ParserISA;
			TextTokenVec vTextTokens, vTextTokensASM;
			ParserISA.Parse((const uint8_t*)pISA, strlen(pISA), &vTextTokens);
			CParserISA::InstrVec vISAInstr;
			ParserISA.Parse(vTextTokens, &vISAInstr);
			CCompilerISA CompilerISA;
			CompilerISA.Compile(vISAInstr, &vTextTokensASM);
			CParserASM ParserASM;
			vTextTokens.clear();
			ParserASM.Parse((const uint8_t*)pAsm, strlen(pAsm), &vTextTokens);
			CCompilerASM CompilerASM;
			SByteCodeBuffer ByteCode;
			CompilerASM.Compile(vTextTokens, &ByteCode);
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


		void InitOpcodeFunctions(OpcodeFunc aOpFuncPtrs[OpCodes::_ENUM_COUNT])
		{
			aOpFuncPtrs[OpCodes::CODE_END] = OpCodeFunctions::CodeEnd;
			aOpFuncPtrs[OpCodes::PRNT] = OpCodeFunctions::Prnt;
			aOpFuncPtrs[OpCodes::PUSHR1] = OpCodeFunctions::Pushr1;
			aOpFuncPtrs[OpCodes::POPR1] = OpCodeFunctions::Popr1;
			aOpFuncPtrs[OpCodes::PUSH] = OpCodeFunctions::Push;
			aOpFuncPtrs[OpCodes::POP] = OpCodeFunctions::Pop;

			using F = float;
			using I = int32_t;
			using U = uint32_t;
			using P = void*;

			aOpFuncPtrs[OpCodes::ADDFF] = OpCodeFunctions::ALU::Add<F, F>;
			aOpFuncPtrs[OpCodes::ADDFFF] = OpCodeFunctions::ALU::Add<F, F, F>;
			aOpFuncPtrs[OpCodes::ADDFFI] = OpCodeFunctions::ALU::Add<F, F, I>;
			aOpFuncPtrs[OpCodes::ADDFFU] = OpCodeFunctions::ALU::Add<F, F, U>;
			aOpFuncPtrs[OpCodes::ADDFI] = OpCodeFunctions::ALU::Add<F, I>;
			aOpFuncPtrs[OpCodes::ADDFIF] = OpCodeFunctions::ALU::Add<F, I, F>;
			aOpFuncPtrs[OpCodes::ADDFII] = OpCodeFunctions::ALU::Add<F, I, I>;
			aOpFuncPtrs[OpCodes::ADDFU] = OpCodeFunctions::ALU::Add<F, U>;
			aOpFuncPtrs[OpCodes::ADDFUF] = OpCodeFunctions::ALU::Add<F, U>;
			aOpFuncPtrs[OpCodes::ADDFUU] = OpCodeFunctions::ALU::Add<F, U, U>;
			aOpFuncPtrs[OpCodes::ADDIF] = OpCodeFunctions::ALU::Add<I, F>;
			aOpFuncPtrs[OpCodes::ADDIFF] = OpCodeFunctions::ALU::Add<I, F, F>;
			aOpFuncPtrs[OpCodes::ADDIFI] = OpCodeFunctions::ALU::Add<I, F, I>;
			aOpFuncPtrs[OpCodes::ADDII] = OpCodeFunctions::ALU::Add<I, I>;
			aOpFuncPtrs[OpCodes::ADDIIF] = OpCodeFunctions::ALU::Add<I, I, F>;
			aOpFuncPtrs[OpCodes::ADDIII] = OpCodeFunctions::ALU::Add<I, I, I>;
			aOpFuncPtrs[OpCodes::ADDIIU] = OpCodeFunctions::ALU::Add<I, I, U>;
			aOpFuncPtrs[OpCodes::ADDIU] = OpCodeFunctions::ALU::Add<I, U>;
			aOpFuncPtrs[OpCodes::ADDIUI] = OpCodeFunctions::ALU::Add<I, U, I>;
			aOpFuncPtrs[OpCodes::ADDIUU] = OpCodeFunctions::ALU::Add<I, U, U>;
			aOpFuncPtrs[OpCodes::ADDUF] = OpCodeFunctions::ALU::Add<U, F>;
			aOpFuncPtrs[OpCodes::ADDUFF] = OpCodeFunctions::ALU::Add<U, F, F>;
			aOpFuncPtrs[OpCodes::ADDUFU] = OpCodeFunctions::ALU::Add<U, F, U>;
			aOpFuncPtrs[OpCodes::ADDUI] = OpCodeFunctions::ALU::Add<U, I>;
			aOpFuncPtrs[OpCodes::ADDUII] = OpCodeFunctions::ALU::Add<U, I, I>;
			aOpFuncPtrs[OpCodes::ADDUIU] = OpCodeFunctions::ALU::Add<U, I, U>;
			aOpFuncPtrs[OpCodes::ADDUU] = OpCodeFunctions::ALU::Add<U, U>;
			aOpFuncPtrs[OpCodes::ADDUUF] = OpCodeFunctions::ALU::Add<U, U, F>;
			aOpFuncPtrs[OpCodes::ADDUUI] = OpCodeFunctions::ALU::Add<U, U, I>;
			aOpFuncPtrs[OpCodes::ADDUUU] = OpCodeFunctions::ALU::Add<U, U, U>;

			aOpFuncPtrs[OpCodes::SUBFF] = OpCodeFunctions::ALU::Sub<F, F>;
			aOpFuncPtrs[OpCodes::SUBFFF] = OpCodeFunctions::ALU::Sub<F, F, F>;
			aOpFuncPtrs[OpCodes::SUBFFI] = OpCodeFunctions::ALU::Sub<F, F, I>;
			aOpFuncPtrs[OpCodes::SUBFFU] = OpCodeFunctions::ALU::Sub<F, F, U>;
			aOpFuncPtrs[OpCodes::SUBFI] = OpCodeFunctions::ALU::Sub<F, I>;
			aOpFuncPtrs[OpCodes::SUBFIF] = OpCodeFunctions::ALU::Sub<F, I, F>;
			aOpFuncPtrs[OpCodes::SUBFII] = OpCodeFunctions::ALU::Sub<F, I, I>;
			aOpFuncPtrs[OpCodes::SUBFU] = OpCodeFunctions::ALU::Sub<F, U>;
			aOpFuncPtrs[OpCodes::SUBFUF] = OpCodeFunctions::ALU::Sub<F, U>;
			aOpFuncPtrs[OpCodes::SUBFUU] = OpCodeFunctions::ALU::Sub<F, U, U>;
			aOpFuncPtrs[OpCodes::SUBIF] = OpCodeFunctions::ALU::Sub<I, F>;
			aOpFuncPtrs[OpCodes::SUBIFF] = OpCodeFunctions::ALU::Sub<I, F, F>;
			aOpFuncPtrs[OpCodes::SUBIFI] = OpCodeFunctions::ALU::Sub<I, F, I>;
			aOpFuncPtrs[OpCodes::SUBII] = OpCodeFunctions::ALU::Sub<I, I>;
			aOpFuncPtrs[OpCodes::SUBIIF] = OpCodeFunctions::ALU::Sub<I, I, F>;
			aOpFuncPtrs[OpCodes::SUBIII] = OpCodeFunctions::ALU::Sub<I, I, I>;
			aOpFuncPtrs[OpCodes::SUBIIU] = OpCodeFunctions::ALU::Sub<I, I, U>;
			aOpFuncPtrs[OpCodes::SUBIU] = OpCodeFunctions::ALU::Sub<I, U>;
			aOpFuncPtrs[OpCodes::SUBIUI] = OpCodeFunctions::ALU::Sub<I, U, I>;
			aOpFuncPtrs[OpCodes::SUBIUU] = OpCodeFunctions::ALU::Sub<I, U, U>;
			aOpFuncPtrs[OpCodes::SUBUF] = OpCodeFunctions::ALU::Sub<U, F>;
			aOpFuncPtrs[OpCodes::SUBUFF] = OpCodeFunctions::ALU::Sub<U, F, F>;
			aOpFuncPtrs[OpCodes::SUBUFU] = OpCodeFunctions::ALU::Sub<U, F, U>;
			aOpFuncPtrs[OpCodes::SUBUI] = OpCodeFunctions::ALU::Sub<U, I>;
			aOpFuncPtrs[OpCodes::SUBUII] = OpCodeFunctions::ALU::Sub<U, I, I>;
			aOpFuncPtrs[OpCodes::SUBUIU] = OpCodeFunctions::ALU::Sub<U, I, U>;
			aOpFuncPtrs[OpCodes::SUBUU] = OpCodeFunctions::ALU::Sub<U, U>;
			aOpFuncPtrs[OpCodes::SUBUUF] = OpCodeFunctions::ALU::Sub<U, U, F>;
			aOpFuncPtrs[OpCodes::SUBUUI] = OpCodeFunctions::ALU::Sub<U, U, I>;
			aOpFuncPtrs[OpCodes::SUBUUU] = OpCodeFunctions::ALU::Sub<U, U, U>;

			aOpFuncPtrs[OpCodes::MULFF] = OpCodeFunctions::ALU::Mul<F, F>;
			aOpFuncPtrs[OpCodes::MULFFF] = OpCodeFunctions::ALU::Mul<F, F, F>;
			aOpFuncPtrs[OpCodes::MULFFI] = OpCodeFunctions::ALU::Mul<F, F, I>;
			aOpFuncPtrs[OpCodes::MULFFU] = OpCodeFunctions::ALU::Mul<F, F, U>;
			aOpFuncPtrs[OpCodes::MULFI] = OpCodeFunctions::ALU::Mul<F, I>;
			aOpFuncPtrs[OpCodes::MULFIF] = OpCodeFunctions::ALU::Mul<F, I, F>;
			aOpFuncPtrs[OpCodes::MULFII] = OpCodeFunctions::ALU::Mul<F, I, I>;
			aOpFuncPtrs[OpCodes::MULFU] = OpCodeFunctions::ALU::Mul<F, U>;
			aOpFuncPtrs[OpCodes::MULFUF] = OpCodeFunctions::ALU::Mul<F, U>;
			aOpFuncPtrs[OpCodes::MULFUU] = OpCodeFunctions::ALU::Mul<F, U, U>;
			aOpFuncPtrs[OpCodes::MULIF] = OpCodeFunctions::ALU::Mul<I, F>;
			aOpFuncPtrs[OpCodes::MULIFF] = OpCodeFunctions::ALU::Mul<I, F, F>;
			aOpFuncPtrs[OpCodes::MULIFI] = OpCodeFunctions::ALU::Mul<I, F, I>;
			aOpFuncPtrs[OpCodes::MULII] = OpCodeFunctions::ALU::Mul<I, I>;
			aOpFuncPtrs[OpCodes::MULIIF] = OpCodeFunctions::ALU::Mul<I, I, F>;
			aOpFuncPtrs[OpCodes::MULIII] = OpCodeFunctions::ALU::Mul<I, I, I>;
			aOpFuncPtrs[OpCodes::MULIIU] = OpCodeFunctions::ALU::Mul<I, I, U>;
			aOpFuncPtrs[OpCodes::MULIU] = OpCodeFunctions::ALU::Mul<I, U>;
			aOpFuncPtrs[OpCodes::MULIUI] = OpCodeFunctions::ALU::Mul<I, U, I>;
			aOpFuncPtrs[OpCodes::MULIUU] = OpCodeFunctions::ALU::Mul<I, U, U>;
			aOpFuncPtrs[OpCodes::MULUF] = OpCodeFunctions::ALU::Mul<U, F>;
			aOpFuncPtrs[OpCodes::MULUFF] = OpCodeFunctions::ALU::Mul<U, F, F>;
			aOpFuncPtrs[OpCodes::MULUFU] = OpCodeFunctions::ALU::Mul<U, F, U>;
			aOpFuncPtrs[OpCodes::MULUI] = OpCodeFunctions::ALU::Mul<U, I>;
			aOpFuncPtrs[OpCodes::MULUII] = OpCodeFunctions::ALU::Mul<U, I, I>;
			aOpFuncPtrs[OpCodes::MULUIU] = OpCodeFunctions::ALU::Mul<U, I, U>;
			aOpFuncPtrs[OpCodes::MULUU] = OpCodeFunctions::ALU::Mul<U, U>;
			aOpFuncPtrs[OpCodes::MULUUF] = OpCodeFunctions::ALU::Mul<U, U, F>;
			aOpFuncPtrs[OpCodes::MULUUI] = OpCodeFunctions::ALU::Mul<U, U, I>;
			aOpFuncPtrs[OpCodes::MULUUU] = OpCodeFunctions::ALU::Mul<U, U, U>;

			aOpFuncPtrs[OpCodes::DIVFF] = OpCodeFunctions::ALU::Div<F, F>;
			aOpFuncPtrs[OpCodes::DIVFFF] = OpCodeFunctions::ALU::Div<F, F, F>;
			aOpFuncPtrs[OpCodes::DIVFFI] = OpCodeFunctions::ALU::Div<F, F, I>;
			aOpFuncPtrs[OpCodes::DIVFFU] = OpCodeFunctions::ALU::Div<F, F, U>;
			aOpFuncPtrs[OpCodes::DIVFI] = OpCodeFunctions::ALU::Div<F, I>;
			aOpFuncPtrs[OpCodes::DIVFIF] = OpCodeFunctions::ALU::Div<F, I, F>;
			aOpFuncPtrs[OpCodes::DIVFII] = OpCodeFunctions::ALU::Div<F, I, I>;
			aOpFuncPtrs[OpCodes::DIVFU] = OpCodeFunctions::ALU::Div<F, U>;
			aOpFuncPtrs[OpCodes::DIVFUF] = OpCodeFunctions::ALU::Div<F, U>;
			aOpFuncPtrs[OpCodes::DIVFUU] = OpCodeFunctions::ALU::Div<F, U, U>;
			aOpFuncPtrs[OpCodes::DIVIF] = OpCodeFunctions::ALU::Div<I, F>;
			aOpFuncPtrs[OpCodes::DIVIFF] = OpCodeFunctions::ALU::Div<I, F, F>;
			aOpFuncPtrs[OpCodes::DIVIFI] = OpCodeFunctions::ALU::Div<I, F, I>;
			aOpFuncPtrs[OpCodes::DIVII] = OpCodeFunctions::ALU::Div<I, I>;
			aOpFuncPtrs[OpCodes::DIVIIF] = OpCodeFunctions::ALU::Div<I, I, F>;
			aOpFuncPtrs[OpCodes::DIVIII] = OpCodeFunctions::ALU::Div<I, I, I>;
			aOpFuncPtrs[OpCodes::DIVIIU] = OpCodeFunctions::ALU::Div<I, I, U>;
			aOpFuncPtrs[OpCodes::DIVIU] = OpCodeFunctions::ALU::Div<I, U>;
			aOpFuncPtrs[OpCodes::DIVIUI] = OpCodeFunctions::ALU::Div<I, U, I>;
			aOpFuncPtrs[OpCodes::DIVIUU] = OpCodeFunctions::ALU::Div<I, U, U>;
			aOpFuncPtrs[OpCodes::DIVUF] = OpCodeFunctions::ALU::Div<U, F>;
			aOpFuncPtrs[OpCodes::DIVUFF] = OpCodeFunctions::ALU::Div<U, F, F>;
			aOpFuncPtrs[OpCodes::DIVUFU] = OpCodeFunctions::ALU::Div<U, F, U>;
			aOpFuncPtrs[OpCodes::DIVUI] = OpCodeFunctions::ALU::Div<U, I>;
			aOpFuncPtrs[OpCodes::DIVUII] = OpCodeFunctions::ALU::Div<U, I, I>;
			aOpFuncPtrs[OpCodes::DIVUIU] = OpCodeFunctions::ALU::Div<U, I, U>;
			aOpFuncPtrs[OpCodes::DIVUU] = OpCodeFunctions::ALU::Div<U, U>;
			aOpFuncPtrs[OpCodes::DIVUUF] = OpCodeFunctions::ALU::Div<U, U, F>;
			aOpFuncPtrs[OpCodes::DIVUUI] = OpCodeFunctions::ALU::Div<U, U, I>;
			aOpFuncPtrs[OpCodes::DIVUUU] = OpCodeFunctions::ALU::Div<U, U, U>;

			aOpFuncPtrs[OpCodes::MOVFF] = OpCodeFunctions::ALU::Mov<F, F>;
			aOpFuncPtrs[OpCodes::MOVFI] = OpCodeFunctions::ALU::Mov<F, I>;
			aOpFuncPtrs[OpCodes::MOVFU] = OpCodeFunctions::ALU::Mov<F, U>;
			aOpFuncPtrs[OpCodes::MOVIF] = OpCodeFunctions::ALU::Mov<I, F>;
			aOpFuncPtrs[OpCodes::MOVII] = OpCodeFunctions::ALU::Mov<I, I>;
			aOpFuncPtrs[OpCodes::MOVIU] = OpCodeFunctions::ALU::Mov<I, U>;
			aOpFuncPtrs[OpCodes::MOVUF] = OpCodeFunctions::ALU::Mov<U, F>;
			aOpFuncPtrs[OpCodes::MOVUI] = OpCodeFunctions::ALU::Mov<U, I>;
			aOpFuncPtrs[OpCodes::MOVUU] = OpCodeFunctions::ALU::Mov<U, U>;
			aOpFuncPtrs[OpCodes::MOVP] = OpCodeFunctions::ALU::Mov<P, P>;
		}
	} // ASM
} // SPP