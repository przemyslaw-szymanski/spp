#pragma once

#include "core/Core.h"

namespace SPP
{
    namespace Bin
    {
        struct ValueTypes
        {
            enum TYPE : uint8_t
            {
                UINT8,
                UINT16,
                UINT32,
                UINT64,
                INT8,
                INT16,
                INT32,
                INT64,
                F32,
                F64,
                PTR,
                _COUNT
            };
        };
        using VALUE_TYPE = ValueTypes::TYPE;

        struct Opcodes
        {
            enum OPCODE : uint8_t
            {
                UNKNOWN,

                MOV,
                MOV_I8,
                MOV_I16,
                MOV_I32,
                MOV_I64,
                MOV_U8,
                MOV_U16,
                MOV_U32,
                MOV_U64,
                MOV_F32,
                MOV_F64,
                MOVP,

                INC,
                INC_I8,
                INC_I16,
                INC_I32,
                INC_I64,
                INC_U8,
                INC_UU6,
                INC_U32,
                INC_U64,
                
                DEC,
                DEC_I8,
                DEC_I16,
                DEC_I32,
                DEC_I64,
                DEC_U8,
                DEC_UU6,
                DEC_U32,
                DEC_U64,

                ADDA,
                ADDA_I8,
                ADDA_I16,
                ADDA_I32,
                ADDA_I64,
                ADDA_U8,
                ADDA_U16,
                ADDA_U32,
                ADDA_U64,
                ADDA_F32,
                ADDA_F64,

                SUB,
                SUB_I8,
                SUB_I16,
                SUB_I32,
                SUB_I64,
                SUB_U8,
                SUB_U16,
                SUB_U32,
                SUB_U64,
                SUB_F32,
                SUB_F64,

                SUBA,
                SUBA_I8,
                SUBA_I16,
                SUBA_I32,
                SUBA_I64,
                SUBA_U8,
                SUBA_U16,
                SUBA_U32,
                SUBA_U64,
                SUBA_F32,
                SUBA_F64,

                MUL,
                MUL_I8,
                MUL_I16,
                MUL_I32,
                MUL_I64,
                MUL_U8,
                MUL_U16,
                MUL_U32,
                MUL_U64,
                MUL_F32,
                MUL_F64,

                MULA,
                MULA_I8,
                MULA_I16,
                MULA_I32,
                MULA_I64,
                MULA_U8,
                MULA_U16,
                MULA_U32,
                MULA_U64,
                MULA_F32,
                MULA_F64,

                DIV,
                DIV_I8,
                DIV_I16,
                DIV_I32,
                DIV_I64,
                DIV_U8,
                DIV_U16,
                DIV_U32,
                DIV_U64,
                DIV_F32,
                DIV_F64,

                DIVA_I8,
                DIVA_I16,
                DIVA_I32,
                DIVA_I64,
                DIVA_U8,
                DIVA_U16,
                DIVA_U32,
                DIVA_U64,
                DIVA_F32,
                DIVA_F64,

                LOGIC_AND,
                LOGIC_OR,
                LOGIC_NOT,

                AND,
                OR,
                XOR,
                NOT,

                JMP,
                COND,

                PUSH,
                POP,
                LOAD,

                CALL,
                PRNT,


                _COUNT
            };
        };
        using OPCODE = Opcodes::OPCODE;

        struct SValue
        {
            union
            {
                int8_t      i8;
                int16_t     i16;
                int32_t     i32;
                int64_t     i64;

                uint8_t     u8;
                uint16_t    u16;
                uint32_t    u32;
                uint64_t    u64;

                float       f32;
                double      f64;

                const void* ptr;
            };
        };

        struct RegDataTypes
        {
            enum TYPE
            {
                INT_8,
                INT_16,
                INT_32,
                INT_64,
                UINT_8,
                UINT_16,
                UINT_32,
                UINT_64,
                F_32,
                F_64,
                PTR,
                _COUNT
            };
        };
        using REG_DATA_TYPE = RegDataTypes::TYPE;

        struct SHeader
        {
            uint32_t    constantSize;
            uint32_t    codeSize;
            uint32_t    constantStartOffset;
            uint32_t    codeStartOffset;
        };

        struct SStack
        {
            static const uint32_t REGISTER_COUNT = 128;
            SValue      aRegisters[REGISTER_COUNT];
            SValue      aRegStack[REGISTER_COUNT];
            uint16_t    currStackPtr = 0;
        };

        struct SHeap
        {

        };

        struct SOpcode
        {
            struct Bitmask
            {
                uint8_t arg1Constant : 1;
                uint8_t arg1Type : 4;
                uint8_t arg2Constant : 1;
                uint8_t arg2Type : 4;
                uint8_t arg3Constant : 1;
                uint8_t arg3Type : 4;
            };

            OPCODE      op : 8;
            Bitmask     mask;
        };

        struct SInstruction
        {
            SOpcode Op;
        };

        struct SArgument
        {
            SValue  Value;
        };

        struct SConstant
        {
            uint32_t    offset;
            SValue      Value;
        };

        struct SLoadConstant
        {
            uint8_t     reg1;
            uint32_t    offset;
        };

        

        struct SInstruction1
        {
            SOpcode     Op;
            uint8_t     reg1;
        };

        struct Instructions
        {
            struct SBase
            {
                OPCODE  Opcode;
            };
            
            struct SInstr1 : public SBase
            {
                union
                {
                    uint8_t     arg1;
                    uint8_t     dstRegIdx;
                };
            };

            struct SInstr2 : public SInstr1
            {
                union
                {
                    uint8_t     arg2;
                    uint8_t     srcRegIdx1;
                };
            };

            struct SInstr3 : public SInstr2
            {
                union
                {
                    uint8_t     arg3;
                    uint8_t     srcRegIdx2;
                };
            };

            struct SLoad : public SBase
            {
                struct
                {
                    uint8_t     isConstant  : 1;
                    uint8_t     type        : 4;

                } Mask;
                uint8_t     dstRegIdx;
                uint32_t    offset;
            };

            struct SJmp : public SBase
            {
                uint32_t    offset;
            };
        };

        template<typename T>
        struct TSInstruction
        {
        };

        struct SCodeBuilder
        {

        };

        class CVirtualMachine
        {
            using ArgumentType = uint8_t;
            using InstrRetType = const uint8_t*;

            struct SInstrArguments
            {
                const uint8_t*  pCode;
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
                void    Execute(const uint8_t* pCode, const size_t codeSize);

            protected:  

                InstrRetType   _ExecuteInstruction(const SInstructionDesc& Instr);

                InstrRetType    _Prnt(const SInstrArguments&);
                InstrRetType    _NoOP(const SInstrArguments&) { return 0; }
                InstrRetType    _Push(const SInstrArguments&);
                InstrRetType    _Pop(const SInstrArguments&);
                InstrRetType    _Load( const SInstrArguments& );
                InstrRetType    _Jmp(const SInstrArguments&);
                InstrRetType    _Mov(const SInstrArguments&);
                InstrRetType    _Add(const SInstrArguments&);
                InstrRetType    _Sub(const SInstrArguments&);
                InstrRetType    _Mul(const SInstrArguments&);
                InstrRetType    _Div(const SInstrArguments&);

            protected:

                OpcodeFunc  m_aOpcodeFunctions[ Opcodes::_COUNT ];
                SStack          m_Stack;
                const uint8_t*  m_pCode;
                const uint8_t*  m_pConstants;
        };
    } // Bin
} // SPP