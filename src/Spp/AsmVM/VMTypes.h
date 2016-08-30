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
		using Uint16Vec = std::vector< uint16_t >;

        struct OpCodes
        {
            enum OP_CODE
            {
                NOOP,

                MOVII, MOVIU, MOVIF,
				MOVUU, MOVUI, MOVUF,
				MOVFF, MOVFU, MOVFI,

				MOVP,

                ADDII, ADDIU, ADDIF,
				ADDUU, ADDUI, ADDUF,
				ADDFF, ADDFU, ADDFI,

                SUBII, SUBIU, SUBIF,
				SUBUU, SUBUI, SUBUF,
				SUBFF, SUBFU, SUBFI,

                MULII, MULIU, MULIF,
				MULUU, MULUI, MULUF,
				MULFF, MULFU, MULFI,

                DIVII, DIVIU, DIVIF,
				DIVUU, DIVUI, DIVUF,
				DIVFF, DIVFU, DIVFI,

                INCI, INCU,
                DECI, DECU,

				ADDIII, ADDIIU, ADDIUU, ADDIUI, ADDIIF, ADDIFF, ADDIFI,
				ADDUUU, ADDUUI, ADDUII, ADDUIU, ADDUUF, ADDUFF, ADDUFU,
				ADDFFF, ADDFFU, ADDFUU, ADDFUF, ADDFFI, ADDFII, ADDFIF,

				SUBIII, SUBIIU, SUBIUU, SUBIUI, SUBIIF, SUBIFF, SUBIFI,
				SUBUUU, SUBUUI, SUBUII, SUBUIU, SUBUUF, SUBUFF, SUBUFU,
				SUBFFF, SUBFFU, SUBFUU, SUBFUF, SUBFFI, SUBFII, SUBFIF,

				MULIII, MULIIU, MULIUU, MULIUI, MULIIF, MULIFF, MULIFI,
				MULUUU, MULUUI, MULUII, MULUIU, MULUUF, MULUFF, MULUFU,
				MULFFF, MULFFU, MULFUU, MULFUF, MULFFI, MULFII, MULFIF,

				DIVIII, DIVIIU, DIVIUU, DIVIUI, DIVIIF, DIVIFF, DIVIFI,
				DIVUUU, DIVUUI, DIVUII, DIVUIU, DIVUUF, DIVUFF, DIVUFU,
				DIVFFF, DIVFFU, DIVFUU, DIVFUF, DIVFFI, DIVFII, DIVFIF,

                JMP,
                CALL,
				PUSH,
				PUSHR1, PUSHR2, PUSHR3,
				POP,
				POPR1, POPR2, POPR3,

                PRNT,

                CODE_END,
                _ENUM_COUNT
            };
        };
        typedef OpCodes::OP_CODE OP_CODE;

        struct DataTypes
        {
            enum TYPE
            {
                INT32,
                UINT32,
                FLOAT32,
                STRING,
				PTR,
                _ENUM_COUNT
            };
        };
        typedef DataTypes::TYPE DATA_TYPE;

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

			template<typename _T_>
			inline void Set(const _T_& tValue)
			{
				uint64 = tValue;
			}

			template<typename _T_>
			inline _T_& Cast() { return Cast<_T_>(this); }

			template<typename _T_>
			static inline _T_& Cast(SRegisterValue& Val)
			{
				return *reinterpret_cast<_T_*>(&Val);
			}

			template<typename _T_>
			static inline _T_& Cast(SRegisterValue* pVal)
			{
				return *reinterpret_cast<_T_*>(pVal);
			}
        };

        struct RegisterTypes
        {
            enum TYPE
            {
                NORMAL,
                HEAP,
                CONSTANT,
                _ENUM_COUNT
            };
        };
        typedef RegisterTypes::TYPE REG_TYPE;
        static const uint32_t MAX_REG_COUNT = 128;

		struct SRegisterInfo
		{
			DATA_TYPE dataType = DataTypes::INT32;

			uint32_t CalcDataSize() const
			{
				static uint32_t aSizes[] =
				{
					//int32, uint32, float32, string, ptr
						4,		4,		4,		8,		8
				};
				return aSizes[dataType];
			}

			template<typename _T_>
			void SetDataType()
			{
				if (std::is_same<_T_, int32_t>::value)
				{
					dataType = DataTypes::INT32;
				}
				else if (std::is_same<_T_, uint32_t>::value)
				{
					dataType = DataTypes::UINT32;
				}
				else if (std::is_same<_T_, float>::value)
				{
					dataType = DataTypes::FLOAT32;
				}
				else if (std::is_same<_T_, void*>::value)
				{
					dataType = DataTypes::PTR;
				}
				else if (std::is_same<_T_, const char*>::value)
				{
					dataType = DataTypes::STRING;
				}
			}
		};

		struct SRegister
		{
			SRegisterValue	Value;
			SRegisterInfo	Info;
		};

		struct SFuncStack;
		struct SHeap;

		struct SOpCodeFuncData
		{
			const uint8_t* pCodeMemory;
			SFuncStack* pStack;
			SHeap* pHeap;
		};

        using RegisterTable = SRegister[ RegisterTypes::_ENUM_COUNT ][ MAX_REG_COUNT ];
        template<uint32_t _COUNT_>
        using RegisterValues = SRegisterValue*[ _COUNT_ ];
        //using OpcodeFunc = std::function<int32_t(const uint64_t*, RegisterTable)>;
        using OpcodeFunc = int32_t( *)(SOpCodeFuncData&);
        using AluFunc = std::function<void( SRegisterValue** )>;

		template<typename _RAW_TYPE_>
		struct SInstrRegBase
		{
			using RAW_TYPE = _RAW_TYPE_;
		};

        struct SInstrReg3 : public SInstrRegBase< uint32_t >
        {
            SInstrReg3()
            {}
            SInstrReg3(RAW_TYPE data) : raw( data )
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
					uint32_t	reserved : 2;
                };
				RAW_TYPE raw;
            };
        };

        struct SInstrReg2 : public SInstrRegBase< uint32_t >
        {
            SInstrReg2()
            {}
            SInstrReg2(RAW_TYPE data) : raw( data )
            {}

            union
            {
                struct
                {
                    uint32_t	reg1TypeBits : 2;
                    uint32_t	reg1IdBits : 8;
                    uint32_t	reg2TypeBits : 2;
                    uint32_t	reg2IdBits : 8;
					uint32_t	reserved : 12;
                };
				RAW_TYPE raw;
            };
        };

        struct SInstrReg1 : SInstrRegBase< uint16_t >
        {
            SInstrReg1()
            {}
            SInstrReg1(RAW_TYPE data) : raw( data )
            {}

            union
            {
                struct
                {
                    uint16_t	reg1TypeBits : 2;
                    uint16_t	reg1IdBits : 8;
					uint64_t	reserved : 6;
                };
				RAW_TYPE raw;
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
			using STACK_ELEMENT_T = uint16_t;
			using StackVec = std::vector< STACK_ELEMENT_T >;

			SFuncStack(uint32_t maxStackSize = 1024 * 1024)
			{
				vStack.resize(maxStackSize / sizeof(STACK_ELEMENT_T));
			}

			uint32_t Push(STACK_ELEMENT_T elemCount)
			{
				// TODO: check for stack overflow
				// Push data size
				auto currPtr = currStackPtr;
				currStackPtr += elemCount;
				auto pData = &vStack[currStackPtr++];
				*pData = elemCount;
				return currPtr;
			}

			uint32_t Push(const void* pData, STACK_ELEMENT_T dataSize)
			{
				auto elemCount = WriteMemory(pData, dataSize);
				return Push(elemCount);
			}

			template<typename _T_>
			uint32_t Push(const _T_& data)
			{
				return Push((&data), sizeof(_T_));
			}

			STACK_ELEMENT_T WriteMemory(const void* pData, STACK_ELEMENT_T dataSize)
			{
				auto pStack = GetStackMemory();
				SPP_MEMCPY(pStack, dataSize, pData, dataSize);
				return dataSize / sizeof(STACK_ELEMENT_T);
			}

			STACK_ELEMENT_T ReadMemory(void* pData, STACK_ELEMENT_T dataSize)
			{
				auto pStack = GetStackMemory();
				SPP_MEMCPY(pData, dataSize, pStack, dataSize);
				return dataSize / sizeof(STACK_ELEMENT_T);
			}

			uint32_t Pop()
			{
				auto currPtr = currStackPtr - 1;
				auto pData = &vStack[currPtr];
				auto elemCount = (*pData);
				currPtr -= elemCount;
				currStackPtr = currPtr;
				return currStackPtr;
			}

			uint32_t Pop(STACK_ELEMENT_T* pBuff)
			{
				auto currPtr = currStackPtr -1;
				auto pData = &vStack[currPtr];
				auto elemCount = (*pData);
				currPtr -= elemCount;

				for (uint32_t i = 0; i < elemCount; ++i)
				{
					pBuff[i] = vStack[currPtr + i];
				}
				currStackPtr = currPtr;
				return currStackPtr;
			}

			template<typename _T_>
			uint32_t Pop(_T_* pOut)
			{
				const STACK_ELEMENT_T elemCount = sizeof(_T_) / sizeof(STACK_ELEMENT_T);
				STACK_ELEMENT_T buff[elemCount];
				auto ptr = Pop();
				auto pData = GetStackMemory();
				*pOut = *reinterpret_cast<_T_*>(pData);
				return ptr;
			}

			STACK_ELEMENT_T* GetStackMemory()
			{
				return &vStack[currStackPtr];
			}

            RegisterTable aaRegisters;
			StackVec vStack;
            uint32_t currStackPtr = 0;
        };

		struct SHeap
		{
			UintVec vConstants;
			UintVec vMemory;
		};

    } // ASM
} // SPP