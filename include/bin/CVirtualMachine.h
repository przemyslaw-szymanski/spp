#pragma once

#include "Bin.h"

namespace SPP
{
    namespace Bin
    {
        

        class SPP_API CVirtualMachine
        {
            using ArgumentType = uint8_t;
            using InstrRetType = const uint8_t*;
            using EntryPointVec = std::vector< SEntryPointDesc >;
            using EntryPointMap = std::unordered_map< uint32_t, SEntryPointDesc >;

            struct SInstrArguments
            {
                const uint8_t*  pCode;
                const uint8_t*  pEnd;
                SStack*         pStack;
            };

            struct SInstructionDesc
            {
                OPCODE          op;
                SInstrArguments OpcodeDesc;
            };

            struct InstructionResults
            {
                enum RESULT
                {

                };
            };
            using InstrResult = InstructionResults::RESULT;

            using OpcodeFunc = InstrRetType(CVirtualMachine::*)(const SInstrArguments&);

            public:

                CVirtualMachine();
                void    Init(const uint8_t* pCode, const size_t codeSize);
                void    Execute(const char* pEntryPointName);

            protected:  

                void            _Execute(uint32_t hash);
                InstrRetType    _Execute(const uint8_t* pCode);
                InstrRetType    _ExecuteInstruction(const SInstructionDesc& Instr);

                InstrRetType    _Prnt(const SInstrArguments&);
                InstrRetType    _NoOP(const SInstrArguments&) { return 0; }
                InstrRetType    _Push(const SInstrArguments&);
                InstrRetType    _Pop(const SInstrArguments&);
                InstrRetType    _Load( const SInstrArguments& );
                InstrRetType    _Jmp(const SInstrArguments&);
                InstrRetType    _Mov(const SInstrArguments&);
                InstrRetType    _Ret(const SInstrArguments&);
                InstrRetType    _Call(const SInstrArguments&);
                

#define DECL_INSTR3(_Func, _Code) \
    InstrRetType _Func(const SInstrArguments& Args) \
    {\
        using InstrType = const Instructions::SInstr3;\
        const auto pInstr = reinterpret_cast<InstrType*>( Args.pCode );\
        auto& Dst = Args.pStack->aRegisters[ pInstr->dstRegIdx ];\
        auto& Src1 = Args.pStack->aRegisters[ pInstr->srcRegIdx1 ];\
        auto& Src2 = Args.pStack->aRegisters[ pInstr->srcRegIdx2 ];\
        {_Code}\
        return Args.pCode + sizeof( InstrType );\
    }

#define DECL_INSTR2(_Func, _Code) \
    InstrRetType _Func(const SInstrArguments& Args) \
    {\
        using InstrType = const Instructions::SInstr2;\
        const auto pInstr = reinterpret_cast<InstrType*>( Args.pCode );\
        auto& Dst = Args.pStack->aRegisters[ pInstr->dstRegIdx ];\
        auto& Src1 = Args.pStack->aRegisters[ pInstr->srcRegIdx1 ];\
        {_Code}\
        return Args.pCode + sizeof( InstrType );\
    }

#define DECL_INSTR1(_Func, _Code) \
    InstrRetType _Func(const SInstrArguments& Args) \
    {\
        using InstrType = const Instructions::SInstr1;\
        const auto pInstr = reinterpret_cast<InstrType*>( Args.pCode );\
        auto& Dst = Args.pStack->aRegisters[ pInstr->dstRegIdx ];\
        {_Code}\
        return Args.pCode + sizeof( InstrType );\
    }

                DECL_INSTR3( _AddI, Dst.i64 = Src1.i64 + Src2.i64; );
                DECL_INSTR3( _AddU, Dst.u64 = Src1.u64 + Src2.u64; );
                DECL_INSTR3( _AddF, Dst.f64 = Src1.f64 + Src2.f64; );
                DECL_INSTR3( _SubI, Dst.i64 = Src1.i64 - Src2.i64; );
                DECL_INSTR3( _SubU, Dst.u64 = Src1.u64 - Src2.u64; );
                DECL_INSTR3( _SubF, Dst.f64 = Src1.f64 - Src2.f64; );
                DECL_INSTR3( _MulI, Dst.i64 = Src1.i64 * Src2.i64; );
                DECL_INSTR3( _MulU, Dst.u64 = Src1.u64 * Src2.u64; );
                DECL_INSTR3( _MulF, Dst.f64 = Src1.f64 * Src2.f64; );
                DECL_INSTR3( _DivI, Dst.i64 = Src1.i64 / Src2.i64; );
                DECL_INSTR3( _DivU, Dst.u64 = Src1.u64 / Src2.u64; );
                DECL_INSTR3( _DivF, Dst.f64 = Src1.f64 / Src2.f64; );

                DECL_INSTR2( _AddAI, Dst.i64 += Src1.i64; );
                DECL_INSTR2( _AddAU, Dst.u64 += Src1.u64; );
                DECL_INSTR2( _AddAF, Dst.f64 += Src1.f64; );
                DECL_INSTR2( _SubAI, Dst.i64 -= Src1.i64; );
                DECL_INSTR2( _SubAU, Dst.u64 -= Src1.u64; );
                DECL_INSTR2( _SubAF, Dst.f64 -= Src1.f64; );
                DECL_INSTR2( _MulAI, Dst.i64 *= Src1.i64; );
                DECL_INSTR2( _MulAU, Dst.u64 *= Src1.u64; );
                DECL_INSTR2( _MulAF, Dst.f64 *= Src1.f64; );
                DECL_INSTR2( _DivAI, Dst.i64 /= Src1.i64; );
                DECL_INSTR2( _DivAU, Dst.u64 /= Src1.u64; );
                DECL_INSTR2( _DivAF, Dst.f64 /= Src1.f64; );

                DECL_INSTR1( _IncI, Dst.i64++; );
                DECL_INSTR1( _IncU, Dst.u64++; );
                DECL_INSTR1( _IncF, Dst.f64++; );
                DECL_INSTR1( _DecI, Dst.i64--; );
                DECL_INSTR1( _DecU, Dst.u64--; );
                DECL_INSTR1( _DecF, Dst.f64--; );
                

            protected:

                OpcodeFunc      m_aOpcodeFunctions[ Opcodes::_COUNT ];
                SStack          m_Stack;
                SStack*         m_pCurrStack = nullptr;
                const uint8_t*  m_pCode;
                const uint8_t*  m_pConstants;
                EntryPointVec   m_vEntryPointDescs;
                EntryPointMap   m_mEntryPoints;
        };
    } // Bin
} // SPP
