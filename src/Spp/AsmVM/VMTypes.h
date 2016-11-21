#pragma once

#include "spp/SppCommon.h"
#include <functional>

namespace SPP
{
    namespace ASM
    {
        using StringToken = char[ 16 ];
        using StringVec = std::vector< StringToken >;
        using UintVec = std::vector< uint32_t >;

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

                MOVS, MOVU, MOVF, MOVP,
                ADDS, ADDU, ADDF, ADDP,
                SUBS, SUBU, SUBF, SUBP,
                MULS, MULU, MULF,
                DIVS, DIVU, DIVF,
                INCS, INCU, INCF, INCP,
                DECS, DECU, DECF, DECP,

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

        using RegisterTable = SRegisterValue[ RegisterTypes::_ENUM_COUNT ][ MAX_REG_COUNT ];
        template<uint32_t _COUNT_>
        using RegisterValues = SRegisterValue*[ _COUNT_ ];
        //using OpcodeFunc = std::function<int32_t(const uint64_t*, RegisterTable)>;
        using OpcodeFunc = int32_t( *)( const uint8_t*, RegisterTable );
        using AluFunc = std::function<void( SRegisterValue** )>;

        struct SInstrReg3
        {
            SInstrReg3()
            {}
            SInstrReg3( uint64_t data ) : raw( data )
            {}

            union
            {
                struct
                {
                    uint32_t	reg1TypeBits : 2;
                    uint32_t	reg1IdBits : 8;
                    uint32_t	reg2TypeBits : 2;
                    uint32_t	reg2IdBits : 8;
                    uint32_t	reg3TypeBits : 2;
                    uint32_t	reg3IdBits : 8;
                };
                uint32_t raw;
            };
        };

        struct SInstrReg2
        {
            SInstrReg2()
            {}
            SInstrReg2( uint32_t data ) : raw( data )
            {}

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
            SInstrReg1()
            {}
            SInstrReg1( uint16_t data ) : raw( data )
            {}

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
            SInstrFuncCall()
            {}
            SInstrFuncCall( uint32_t data ) : raw( data )
            {}

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

        struct SFuncStack
        {
            RegisterTable aaRegisters;
            uint32_t currPtr = 0;
            uint32_t currStackPtr = 0;
        };

        struct SHeap
        {
            UintVec vConstant;
            UintVec vDynamic;
        };
    } // ASM
} // SPP