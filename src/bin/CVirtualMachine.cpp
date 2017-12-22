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
            m_aOpcodeFunctions[ Opcodes::CALL ] = &CVirtualMachine::_Call;
            m_aOpcodeFunctions[ Opcodes::RET ] = &CVirtualMachine::_Ret;
            m_aOpcodeFunctions[ Opcodes::ADDI ] = &CVirtualMachine::_AddI;
            m_aOpcodeFunctions[ Opcodes::ADDU ] = &CVirtualMachine::_AddU;
            m_aOpcodeFunctions[ Opcodes::ADDF ] = &CVirtualMachine::_AddF;
            //m_aOpcodeFunctions[ Opcodes::ADDP ] = &CVirtualMachine::_AddP;
            m_aOpcodeFunctions[ Opcodes::SUBI ] = &CVirtualMachine::_SubI;
            m_aOpcodeFunctions[ Opcodes::SUBU ] = &CVirtualMachine::_SubU;
            m_aOpcodeFunctions[ Opcodes::SUBF ] = &CVirtualMachine::_SubF;
            m_aOpcodeFunctions[ Opcodes::MULI ] = &CVirtualMachine::_MulI;
            m_aOpcodeFunctions[ Opcodes::MULU ] = &CVirtualMachine::_MulU;
            m_aOpcodeFunctions[ Opcodes::MULF ] = &CVirtualMachine::_MulF;
            m_aOpcodeFunctions[ Opcodes::DIVI ] = &CVirtualMachine::_DivI;
            m_aOpcodeFunctions[ Opcodes::DIVU ] = &CVirtualMachine::_DivU;
            m_aOpcodeFunctions[ Opcodes::DIVF ] = &CVirtualMachine::_DivF;
            m_aOpcodeFunctions[ Opcodes::ADDAI ] = &CVirtualMachine::_AddAI;
            m_aOpcodeFunctions[ Opcodes::ADDAU ] = &CVirtualMachine::_AddAU;
            m_aOpcodeFunctions[ Opcodes::ADDAF ] = &CVirtualMachine::_AddAF;
            //m_aOpcodeFunctions[ Opcodes::ADDAP ] = &CVirtualMachine::_AddAP;
            m_aOpcodeFunctions[ Opcodes::SUBAI ] = &CVirtualMachine::_SubAI;
            m_aOpcodeFunctions[ Opcodes::SUBAU ] = &CVirtualMachine::_SubAU;
            m_aOpcodeFunctions[ Opcodes::SUBAF ] = &CVirtualMachine::_SubAF;
            m_aOpcodeFunctions[ Opcodes::MULAI ] = &CVirtualMachine::_MulAI;
            m_aOpcodeFunctions[ Opcodes::MULAU ] = &CVirtualMachine::_MulAU;
            m_aOpcodeFunctions[ Opcodes::MULAF ] = &CVirtualMachine::_MulAF;
            m_aOpcodeFunctions[ Opcodes::DIVAI ] = &CVirtualMachine::_DivAI;
            m_aOpcodeFunctions[ Opcodes::DIVAU ] = &CVirtualMachine::_DivAU;
            m_aOpcodeFunctions[ Opcodes::DIVAF ] = &CVirtualMachine::_DivAF;
            m_aOpcodeFunctions[ Opcodes::INCI ] = &CVirtualMachine::_IncI;
            m_aOpcodeFunctions[ Opcodes::INCU ] = &CVirtualMachine::_IncU;
            m_aOpcodeFunctions[ Opcodes::INCF ] = &CVirtualMachine::_IncF;
            //m_aOpcodeFunctions[ Opcodes::INCP ] = &CVirtualMachine::_IncP;
            m_aOpcodeFunctions[ Opcodes::DECI ] = &CVirtualMachine::_DecI;
            m_aOpcodeFunctions[ Opcodes::DECU ] = &CVirtualMachine::_DecU;
            m_aOpcodeFunctions[ Opcodes::DECF ] = &CVirtualMachine::_DecF;
            
        }

        CVirtualMachine::InstrRetType CVirtualMachine::_ExecuteInstruction(const SInstructionDesc& Desc)
        {
            auto pFunc = m_aOpcodeFunctions[Desc.op];
            return (this->*pFunc)(Desc.OpcodeDesc);
        }

        CVirtualMachine::InstrRetType CVirtualMachine::_Execute(const uint8_t* pCode)
        {
            const uint8_t* pCurrPtr = pCode;
            const uint8_t* pRet = pCurrPtr;
            SStack Stack;
            SStack* pPrevStack = m_pCurrStack;
            m_pCurrStack = &Stack;

            SInstructionDesc Desc;
            Desc.OpcodeDesc.pStack = m_pCurrStack;

            while( pCurrPtr )
            {    
                Desc.op = static_cast<OPCODE>( *pCurrPtr );
                Desc.OpcodeDesc.pCode = pCurrPtr;
                
                pCurrPtr = _ExecuteInstruction( Desc );
                if( pCurrPtr )
                {
                    pRet = pCurrPtr;
                }
            }
            m_pCurrStack = pPrevStack;
            return pRet;
        }

        void CVirtualMachine::Execute(const char* pEntryPointName)
        {
            uint32_t hash = std::hash< const char* >{}( pEntryPointName );
            _Execute( hash );
        }

        void CVirtualMachine::_Execute(uint32_t hash)
        {
            
            const SEntryPointDesc& Desc = m_mEntryPoints[ hash ];
            const uint8_t* pCode = m_pCode + Desc.offset;
            _Execute( pCode );
        }

        void CVirtualMachine::Init(const uint8_t* pCode, const size_t size)
        {
            const uint8_t* pCurrPtr = pCode;
            const uint8_t* pEndPtr = pCode + size;
            // Read header
            const SHeader* pHeader = reinterpret_cast< const SHeader* >( pCurrPtr );
            
            m_pCode = pCode + pHeader->codeStartOffset;
            pCurrPtr = m_pCode;
            m_pConstants = pCode + pHeader->constantStartOffset;
            m_pCurrStack = &m_Stack;

            const SEntryPointDesc* pEntryPoints = reinterpret_cast< const SEntryPointDesc* >( pCode + pHeader->entryPointStartOffset);
            for( uint32_t i = 0; i < pHeader->entryPointCount; ++i )
            {
                const SEntryPointDesc* pDesc = ( pEntryPoints );
                pEntryPoints++;
                m_mEntryPoints[ pDesc->hash ] = *pDesc;
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
                        case 'f': ss << Stack.aRegisters[currArg++].f64; break;
                        case 'd': ss << Stack.aRegisters[currArg++].i64; break;
                        case 'u': ss << Stack.aRegisters[currArg++].u64; break;
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
            m_Stack.aRegStack[ m_Stack.currStackPtr++ ] = Args.pStack->aRegisters[ pInstr->arg1 ];
            return Args.pCode + sizeof(Instructions::SInstr1);
        }

        CVirtualMachine::InstrRetType CVirtualMachine::_Pop(const SInstrArguments& Args)
        {
            const auto pInstr = reinterpret_cast<const Instructions::SInstr1*>(Args.pCode);
            //m_Stack.aRegisters[pInstr->arg1] = m_Stack.aRegStack[--m_Stack.currStackPtr];
            Args.pStack->aRegisters[ pInstr->dstRegIdx ] = m_Stack.aRegStack[ --m_Stack.currStackPtr ];
            return Args.pCode + sizeof(Instructions::SInstr1);
        }

        CVirtualMachine::InstrRetType CVirtualMachine::_Load(const SInstrArguments& Args)
        {
            const auto pLoad = reinterpret_cast< const Instructions::SLoad* >( Args.pCode );
            //auto& Reg = m_Stack.aRegisters[ pLoad->dstRegIdx ];
            auto& Reg = Args.pStack->aRegisters[ pLoad->dstRegIdx ];
            if( pLoad->Mask.isConstant )
            {
                const void* pPtr = m_pConstants + pLoad->offset + sizeof(SConstant);
                switch (pLoad->Mask.type)
                {
                    case ValueTypes::PTR: Reg.ptr = pPtr; break;
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
            Args.pStack->aRegisters[pInstr->dstRegIdx] = Args.pStack->aRegisters[pInstr->srcRegIdx1];
            return Args.pCode + sizeof(Instructions::SInstr2);
        }

        CVirtualMachine::InstrRetType CVirtualMachine::_Ret(const SInstrArguments& Args)
        {
            using InstrType = Instructions::SInstr1;
            const auto pInstr = reinterpret_cast<const InstrType*>( Args.pCode );
            
            return 0;
        }

        CVirtualMachine::InstrRetType CVirtualMachine::_Call(const SInstrArguments& Args)
        {
            using InstrType = Instructions::SCall;
            const auto pInstr = reinterpret_cast<const InstrType*>( Args.pCode );
            const uint8_t* pPtr = m_pCode + pInstr->offset;
            _Execute( pPtr );
            return Args.pCode + sizeof(InstrType);
        }

    } // Bin
} // SPP