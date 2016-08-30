#pragma once

#include "VMTypes.h"

namespace SPP
{
	namespace ASM
	{
		

		template<typename _T1_>
		struct TSRegisterValues1
		{
			_T1_* pReg1;
		};

		template<typename _T1_, typename _T2_>
		struct TSRegisterValues2
		{
			_T1_* pReg1;
			_T2_* pReg2;
		};

		template<typename _T1_, typename _T2_, typename _T3_>
		struct TSRegisterValues3
		{
			_T1_* pReg1;
			_T2_* pReg2;
			_T3_* pReg3;
		};

		template<typename _INSTR_REG_>
		inline uint32_t GetInstrReg(const uint8_t* pMem, _INSTR_REG_* pOut)
		{
			pOut->raw = *reinterpret_cast<const _INSTR_REG_::RAW_TYPE*>(pMem);
			return sizeof(_INSTR_REG_::RAW_TYPE);
		}

		inline SRegister& GetRegister(RegisterTable aaRegisters, const SInstrReg1& Reg)
		{
			auto aRegisters = aaRegisters[Reg.reg1TypeBits];
			return aRegisters[Reg.reg1IdBits];
		}

		template<typename _T1_, typename _T2_, typename _T3_>
		inline void GetRegisterValues(RegisterTable aaRegisters, SInstrReg3 Instr, 
			TSRegisterValues3<_T1_, _T2_, _T3_>& Vals)
		{
			Vals.pReg1 = reinterpret_cast<_T1_*>(&aaRegisters[Instr.reg1TypeBits][Instr.reg1IdBits].Value);
			Vals.pReg2 = reinterpret_cast<_T2_*>(&aaRegisters[Instr.reg2TypeBits][Instr.reg2IdBits].Value);
			Vals.pReg3 = reinterpret_cast<_T3_*>(&aaRegisters[Instr.reg3TypeBits][Instr.reg3IdBits].Value);
		}

		template<typename _T1_, typename _T2_>
		void GetRegisterValues(RegisterTable aaRegisters, SInstrReg2 Instr, TSRegisterValues2<_T1_, _T2_>& Vals)
		{
			Vals.pReg1 = reinterpret_cast<_T1_*>(&aaRegisters[Instr.reg1TypeBits][Instr.reg1IdBits].Value);
			Vals.pReg2 = reinterpret_cast<_T2_*>(&aaRegisters[Instr.reg2TypeBits][Instr.reg2IdBits].Value);
		}

		template<typename _T1_>
		void GetRegisterValues(RegisterTable aaRegisters, SInstrReg1 Instr, TSRegisterValues1<_T1_>& Vals)
		{
			Vals.pReg1 = reinterpret_cast<_T1_*>(&aaRegisters[Instr.reg1TypeBits][Instr.reg1IdBits].Value);
		}

		template<typename _T1_, typename _T2_, typename _T3_>
		int32_t Alu(const uint8_t* pMem, RegisterTable aaRegisters, TSRegisterValues3<_T1_, _T2_, _T3_>& Values)
		{
			SInstrReg3 Instr(*reinterpret_cast<const SInstrReg3::RAW_TYPE*>(pMem));
			SRegister& Reg1 = aaRegisters[Instr.reg1TypeBits][Instr.reg1IdBits];
			SRegister& Reg2 = aaRegisters[Instr.reg2TypeBits][Instr.reg2IdBits];
			SRegister& Reg3 = aaRegisters[Instr.reg3TypeBits][Instr.reg3IdBits];
			Reg1.Info.SetDataType<_T1_>();
			Values.pReg1 = reinterpret_cast<_T1_*>(&Reg1.Value);
			Values.pReg2 = reinterpret_cast<_T2_*>(&Reg2.Value);
			Values.pReg3 = reinterpret_cast<_T3_*>(&Reg3.Value);
			return sizeof(SInstrReg2::RAW_TYPE);
		}

		template<typename _T1_, typename _T2_>
		int32_t Alu(const uint8_t* pMem, RegisterTable aaRegisters, TSRegisterValues2<_T1_, _T2_>& Values)
		{
			SInstrReg2 Instr(*reinterpret_cast<const SInstrReg2::RAW_TYPE*>(pMem));
			SRegister& Reg1 = aaRegisters[Instr.reg1TypeBits][Instr.reg1IdBits];
			SRegister& Reg2 = aaRegisters[Instr.reg2TypeBits][Instr.reg2IdBits];
			Reg1.Info.SetDataType<_T1_>();
			Values.pReg1 = reinterpret_cast<_T1_*>(&Reg1.Value);
			Values.pReg2 = reinterpret_cast<_T2_*>(&Reg2.Value);
			return sizeof(SInstrReg2::RAW_TYPE);
		}

		template<typename _T1_>
		int32_t Alu(const uint8_t* pMem, RegisterTable aaRegisters, TSRegisterValues1<_T1_>& Values)
		{
			SInstrReg1 Instr(*reinterpret_cast<const SInstrReg1::RAW_TYPE*>(pMem));
			SRegister& Reg = GetRegister(aaRegisters, Instr);
			Reg.Info.SetDataType<_T1_>();
			Values.pReg1 = reinterpret_cast<_T1_*>(&Reg.Value);
			return sizeof(SInstrReg1::RAW_TYPE);
		}

		inline int32_t GetOperands3(const uint8_t* pMem, SFuncStack* pStack, SRegister** ppOut)
		{
			SInstrReg3 Instr(*reinterpret_cast<const SInstrReg3::RAW_TYPE*>(pMem));
			auto aRegTable = pStack->aaRegisters[Instr.reg1TypeBits];
			ppOut[0] = &aRegTable[Instr.reg1IdBits];
			aRegTable = pStack->aaRegisters[Instr.reg2TypeBits];
			ppOut[1] = &aRegTable[Instr.reg2IdBits];
			aRegTable = pStack->aaRegisters[Instr.reg3TypeBits];
			ppOut[2] = &aRegTable[Instr.reg3IdBits];
			return sizeof(SInstrReg3::RAW_TYPE);
		}

		inline int32_t GetOperands2(const uint8_t* pMem, RegisterTable aaRegisters, SRegister** ppOut)
		{
			SInstrReg2 Instr(*reinterpret_cast<const SInstrReg3::RAW_TYPE*>(pMem));
			ppOut[0] = &aaRegisters[Instr.reg1TypeBits][Instr.reg1IdBits];
			ppOut[1] = &aaRegisters[Instr.reg2TypeBits][Instr.reg2IdBits];
			return sizeof(SInstrReg2::RAW_TYPE);
		}

		inline int32_t GetOperands1(const uint8_t* pMem, RegisterTable aaRegisters, SRegister** ppOut)
		{
			SInstrReg1 Instr(*reinterpret_cast<const SInstrReg3::RAW_TYPE*>(pMem));
			ppOut[0] = &aaRegisters[Instr.reg1TypeBits][Instr.reg1IdBits];
			return sizeof(SInstrReg1::RAW_TYPE);
		}

#define DEF_OPCODE(_name) int32_t _name(SOpCodeFuncData& Data)

#define ALU3_IMPL2(_operand) \
	TSRegisterValues3< _T1_, _T2_, _T3_ > V; \
	auto bytes = Alu(Data.pCodeMemory, pStack->aaRegisters, V); \
	*V.pReg1 = static_cast<_T1_>(*V.pReg2 _operand static_cast<_T2_>(*V.pReg3)); \
	return bytes

#define ALU3_IMPL(_op) \
	SRegister* pRegs[3]; \
	auto bytes = GetOperands3(Data.pCodeMemory, Data.pStack, pRegs); \
	pRegs[0]->Info.SetDataType< _T1_ >(); \
	pRegs[0]->Value.Set(pRegs[1]->Value.Cast<_T1_>() _op pRegs[2]->Value.Cast<_T1_>()); \
	return bytes

#define ALU2_IMPL(_operand) \
	TSRegisterValues2< _T1_, _T2_ > V; \
	auto bytes = Alu(Data.pCodeMemory, Data.pStack->aaRegisters, V); \
	*V.pReg1 _operand static_cast< _T1_ >(*V.pReg2); \
	return bytes

		namespace OpCodeFunctions
		{
			namespace ALU
			{
				template<typename _T1_, typename _T2_, typename _T3_>
				DEF_OPCODE(Add)
				{
					ALU3_IMPL(+);
				}

				template<typename _T1_, typename _T2_, typename _T3_>
				DEF_OPCODE(Sub)
				{
					ALU3_IMPL(-);
				}

				template<typename _T1_, typename _T2_, typename _T3_>
				DEF_OPCODE(Mul)
				{
					ALU3_IMPL(*);
				}

				template<typename _T1_, typename _T2_, typename _T3_>
				DEF_OPCODE(Div)
				{
					ALU3_IMPL(/);
				}

				template<typename _T1_, typename _T2_>
				DEF_OPCODE(Add)
				{
					ALU2_IMPL(+=);
				}

				template<typename _T1_, typename _T2_>
				DEF_OPCODE(Sub)
				{
					ALU2_IMPL(-=);
				}

				template<typename _T1_, typename _T2_>
				DEF_OPCODE(Mul)
				{
					ALU2_IMPL(*=);
				}

				template<typename _T1_, typename _T2_>
				DEF_OPCODE(Div)
				{
					ALU2_IMPL(/=);
				}

				template<typename _T1_, typename _T2_>
				DEF_OPCODE(Mov)
				{
					ALU2_IMPL(=);
				}
			} // ALU

			DEF_OPCODE(Prnt)
			{
				TSRegisterValues1<int32_t> Reg;
				SInstrReg1 Instr(*reinterpret_cast<const uint16_t*>(Data.pCodeMemory));
				GetRegisterValues(Data.pStack->aaRegisters, Instr, Reg);
				printf("%d\n", *Reg.pReg1);
				//return ret;
				return sizeof(Instr.raw);
			}

			DEF_OPCODE(Call)
			{
				// Read func address = 4 bytes
				const uint8_t* pFunc = reinterpret_cast<const uint8_t*>(Data.pCodeMemory);
				uint32_t funcAddr[4] = { pFunc[0], pFunc[1], pFunc[2], pFunc[3] };

				return 4;
			}

			DEF_OPCODE(Push)
			{
				SInstrReg1 Operand;
				auto size = GetInstrReg(Data.pCodeMemory, &Operand);
				auto pStack = Data.pStack;
				auto& Reg = GetRegister(pStack->aaRegisters, Operand);
				pStack->Push(&Reg.Value, Reg.Info.CalcDataSize());
				return size;
			}

			DEF_OPCODE(Pushr1)
			{
				SInstrReg1 Operand;
				auto size = GetInstrReg(Data.pCodeMemory, &Operand);
				auto pStack = Data.pStack;
				auto& Reg = GetRegister(pStack->aaRegisters, Operand);
				pStack->Push(&Reg.Value, Reg.Info.CalcDataSize());
				return size;
			}

			DEF_OPCODE(Pop)
			{
				SInstrReg1 Operand;
				auto size = GetInstrReg(Data.pCodeMemory, &Operand);
				auto pStack = Data.pStack;
				auto& Reg = GetRegister(pStack->aaRegisters, Operand);
				pStack->Pop();
				pStack->ReadMemory(&Reg.Value, Reg.Info.CalcDataSize());
				return size;
			}

			DEF_OPCODE(Popr1)
			{
				SInstrReg1 Operand;
				auto size = GetInstrReg(Data.pCodeMemory, &Operand);
				auto pStack = Data.pStack;
				auto& Reg = GetRegister(pStack->aaRegisters, Operand);
				pStack->Pop();
				pStack->ReadMemory(&Reg.Value, Reg.Info.CalcDataSize());
				return size;
			}

			DEF_OPCODE(CodeEnd)
			{
				return -1;
			}
		} // OpCodeFunctions
	} // ASM
} // SPP