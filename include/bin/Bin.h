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
                INT,
                UINT,
                FLOAT,
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
                MOVI,
                MOVU,
                MOVF,
                MOVP,

                INC,
                INCI,
                INCU,
                INCF,
                INCP,

                DEC,
                DECI,
                DECU,
                DECF,

                ADD,
                ADDI,
                ADDU,
                ADDF,
                ADDP,

                ADDA,
                ADDAI,
                ADDAU,
                ADDAF,
                ADDAP,

                SUB,
                SUBI,
                SUBU,
                SUBF,
                SUBP,

                SUBA,
                SUBAI,
                SUBAU,
                SUBAF,

                MUL,
                MULI,
                MULU,
                MULF,

                MULA,
                MULAI,
                MULAU,
                MULAF,

                DIV,
                DIVI,
                DIVU,
                DIVF,

                DIVA,
                DIVAI,
                DIVAU,
                DIVAF,

                LAND,
                LOR,
                LNOT,

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
                RET,
                PRNT,


                _COUNT
            };
        };
        using OPCODE = Opcodes::OPCODE;

        struct SValue
        {
            union
            {
                int64_t     i64;
                uint64_t    u64;
                double      f64;
                const void*       ptr;
            };
        };

        struct SHeader
        {
            uint32_t    constantSize;
            uint32_t    codeSize;
            uint32_t    entryPointCount;
            uint32_t    entryPointStartOffset;
            uint32_t    constantStartOffset;
            uint32_t    codeStartOffset;
        };

        struct SEntryPointDesc
        {
            uint32_t    hash;
            uint32_t    offset;
            uint16_t    size;
            uint8_t     argCount;
            uint8_t     retType;
        };

        struct SStack
        {
            static const uint32_t REGISTER_COUNT = 128;
            SValue      aRegisters[ REGISTER_COUNT ];
            SValue      aRegStack[ REGISTER_COUNT ];
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
                uint8_t arg1Type : 3;
                uint8_t arg2Constant : 1;
                uint8_t arg2Type : 3;
                uint8_t arg3Constant : 1;
                uint8_t arg3Type : 3;
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
                    uint8_t     isConstant : 1;
                    uint8_t     type : 4;

                } Mask;
                uint8_t     dstRegIdx;
                uint32_t    offset;
            };

            struct SJmp : public SBase
            {
                uint32_t    offset;
            };

            struct SCall : public SBase
            {
                uint32_t    offset;
            };
        };

        template<typename T>
        struct TSInstruction
        {};

        struct SCodeBuilder
        {

        };
    } // Bin
} // SPP