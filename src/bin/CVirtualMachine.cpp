#include "Bin/CVirtualMachine.h"
#include <cstdarg>

namespace SPP
{
    namespace Bin
    {
        CVirtualMachine::CVirtualMachine()
        {
            m_aOpcodeFunctions[Opcodes::UNKNOWN] = &CVirtualMachine::_NoOP;
            m_aOpcodeFunctions[ Opcodes::PRNT ] = &CVirtualMachine::_Prnt;
            m_aOpcodeFunctions[ Opcodes::PUSH ] = &CVirtualMachine::_Push;
            m_aOpcodeFunctions[Opcodes::POP] = &CVirtualMachine::_Pop;
            m_aOpcodeFunctions[ Opcodes::LOAD ] = &CVirtualMachine::_Load;
            m_aOpcodeFunctions[Opcodes::JMP] = &CVirtualMachine::_Jmp;
        }

        CVirtualMachine::InstrRetType CVirtualMachine::_ExecuteInstruction(const SInstructionDesc& Desc)
        {
            auto pFunc = m_aOpcodeFunctions[Desc.op];
            return (this->*pFunc)(Desc.OpcodeDesc);
        }

        void CVirtualMachine::Execute(const uint8_t* pCode, const size_t size)
        {
            const uint8_t* pCurrPtr = pCode;
            const uint8_t* pEndPtr = pCode + size;
            // Read header
            const SHeader* pHeader = reinterpret_cast< const SHeader* >( pCurrPtr );
            
            m_pCode = pCode + pHeader->codeStartOffset;
            pCurrPtr = m_pCode;
            m_pConstants = pCode + pHeader->constantStartOffset;
            
            while (pCurrPtr < pEndPtr)
            {
                SInstructionDesc Desc;
                Desc.op = static_cast<OPCODE>( *pCurrPtr );
                Desc.OpcodeDesc.pCode = pCurrPtr;
                pCurrPtr = _ExecuteInstruction(Desc);
            }
        }

        CVirtualMachine::InstrRetType CVirtualMachine::_Prnt(const SInstrArguments& Args)
        {
            const auto pInstr = reinterpret_cast<const Instructions::SInstr1*>(Args.pCode);
            SStack Stack;
            for (uint32_t i = 0, r = pInstr->arg1-1; i < pInstr->arg1; ++i, --r)
            {
                Stack.aRegisters[r] = m_Stack.aRegStack[--m_Stack.currStackPtr];
            }
            const char* pFmt = static_cast< const char* >( Stack.aRegisters[0].ptr );
            std::stringstream ss;
            const char* pCurr = pFmt;
            char chr, lastChr;
            uint32_t currArg = 1;
            while ((chr = *pCurr++))
            {
                if (chr == '%' && lastChr != '\\')
                {
                    chr = *pCurr++;
                    switch (chr)
                    {
                        case 'd': ss << Stack.aRegisters[currArg++].i32; break;
                        case 'u': ss << Stack.aRegisters[currArg++].u32; break;
                        case 'f': ss << Stack.aRegisters[currArg++].f64; break;
                        case 'l': ss << Stack.aRegisters[currArg++].i64; break;
                        case 'q': ss << Stack.aRegisters[currArg++].u64; break;
                        case 's': ss << static_cast< const char* >( Stack.aRegisters[currArg++].ptr ); break;
                        case 'p': ss<< "0x" << Stack.aRegisters[currArg++].ptr; break;
                        default: ss << "<unknown>"; break;
                    }
                }
                else
                {
                    ss << chr;
                }
                lastChr = chr;
            }
            printf(ss.str().c_str());
            return Args.pCode + sizeof(Instructions::SInstr1);
        }

        CVirtualMachine::InstrRetType CVirtualMachine::_Push(const SInstrArguments& Args)
        {
            const auto pInstr = reinterpret_cast<const Instructions::SInstr1*>(Args.pCode);
            m_Stack.aRegStack[m_Stack.currStackPtr++] = m_Stack.aRegisters[pInstr->arg1];
            return Args.pCode + sizeof(Instructions::SInstr1);
        }

        CVirtualMachine::InstrRetType CVirtualMachine::_Pop(const SInstrArguments& Args)
        {
            const auto pInstr = reinterpret_cast<const Instructions::SInstr1*>(Args.pCode);
            m_Stack.aRegisters[pInstr->arg1] = m_Stack.aRegStack[--m_Stack.currStackPtr];
            return Args.pCode + sizeof(Instructions::SInstr1);
        }

        CVirtualMachine::InstrRetType CVirtualMachine::_Load(const SInstrArguments& Args)
        {
            const auto pLoad = reinterpret_cast< const Instructions::SLoad* >( Args.pCode );
            auto& Reg = m_Stack.aRegisters[ pLoad->dstRegIdx ];
            if( pLoad->Mask.isConstant )
            {
                const void* pPtr = m_pConstants + pLoad->offset;
                switch (pLoad->Mask.type)
                {
                    case RegDataTypes::PTR: Reg.ptr = pPtr; break;
                    /*case RegDataTypes::INT_16:
                    case RegDataTypes::INT_32:
                    case RegDataTypes::INT_64:
                    case RegDataTypes::INT_8:
                        Reg.i64 = *reinterpret_cast< const int64_t*>(pPtr);
                    break;
                    case RegDataTypes::UINT_16:
                    case RegDataTypes::UINT_32:
                    case RegDataTypes::UINT_64:
                    case RegDataTypes::UINT_8:
                        Reg.u64 = *reinterpret_cast< const uint64_t*>(pPtr);
                    break;*/
                    default:
                        Reg = *reinterpret_cast<const SValue*>(pPtr);
                    break;
                }
                
                //
            }
            return Args.pCode + sizeof( Instructions::SLoad );
        }

        CVirtualMachine::InstrRetType CVirtualMachine::_Jmp(const SInstrArguments& Args)
        {
            const auto pJmp = reinterpret_cast<const Instructions::SJmp*>(Args.pCode);
            return m_pCode + pJmp->offset;
        }

        CVirtualMachine::InstrRetType CVirtualMachine::_Mov(const SInstrArguments& Args)
        {
            const auto pInstr = reinterpret_cast<const Instructions::SInstr2*>(Args.pCode);
            m_Stack.aRegisters[pInstr->dstRegIdx] = m_Stack.aRegisters[pInstr->srcRegIdx1];
            return Args.pCode + sizeof(Instructions::SInstr2);
        }

        CVirtualMachine::InstrRetType CVirtualMachine::_Add(const SInstrArguments& Args)
        {
            const auto pInstr = reinterpret_cast<const Instructions::SInstr3*>(Args.pCode);
            auto& DstReg = m_Stack.aRegisters[pInstr->dstRegIdx];
            auto& SrcReg1 = m_Stack.aRegisters[pInstr->]
            return Args.pCode + sizeof(Instructions::SInstr2);
        }

    } // Bin
} // SPP