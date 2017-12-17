#include "Bin/CVirtualMachine.h"

namespace SPP
{
    namespace Bin
    {
        CVirtualMachine::CVirtualMachine()
        {
            m_aOpcodeFunctions[Opcodes::UNKNOWN] = &CVirtualMachine::_NoOP;
            m_aOpcodeFunctions[ Opcodes::PRNT ] = &CVirtualMachine::_Prnt;
            m_aOpcodeFunctions[ Opcodes::PUSH ] = &CVirtualMachine::_Push;
            m_aOpcodeFunctions[ Opcodes::LOAD ] = &CVirtualMachine::_Load;
        }

        uint32_t CVirtualMachine::_ExecuteInstruction(const SInstructionDesc& Desc)
        {
            auto pFunc = m_aOpcodeFunctions[Desc.op];
            return (this->*pFunc)(Desc.OpcodeDesc) + 1;
        }

        void CVirtualMachine::Execute(const uint8_t* pCode, const size_t size)
        {
            const uint8_t* pCurrPtr = pCode;
            const uint8_t* pEndPtr = pCode + size;
            // Read header
            const SHeader* pHeader = reinterpret_cast< const SHeader* >( pCurrPtr );
            
            pCurrPtr = pCode + pHeader->codeStartOffset;

            while (pCurrPtr < pEndPtr)
            {
                SInstructionDesc Desc;
                Desc.op = static_cast<OPCODE>( *pCurrPtr );
                Desc.OpcodeDesc.pCode = pCurrPtr;
                pCurrPtr += _ExecuteInstruction(Desc);
            }
        }

        uint32_t CVirtualMachine::_Prnt(const SInstrArguments& Desc)
        {
            printf("hello");
            return 0;
        }

        uint32_t CVirtualMachine::_Push(const SInstrArguments& Args)
        {
            return 0;
        }

        uint32_t CVirtualMachine::_Load(const SInstrArguments& Args)
        {
            const auto pLoad = reinterpret_cast< const TSInstruction< SLoadConstant >* >( Args.pCode );
            m_Stack.aRegisters[ pLoad->Data.reg1 ].u32 = pLoad->Data.offset;
            return sizeof( TSInstruction< SLoadConstant > );
        }

    } // Bin
} // SPP