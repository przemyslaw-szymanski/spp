#include "Spp/S++.h"
#include <iostream>
#include <string>

using Opcode = SPP::Bin::SOpcode;
using Opcodes = SPP::Bin::Opcodes;
using OPCODE = SPP::Bin::OPCODE;
using SInstruction1 = SPP::Bin::SInstruction1;
template<typename T>
using TInstruction = SPP::Bin::TSInstruction< T >;

struct SCodeBuilder
{
    uint8_t*    pCode;
    uint8_t*    pCurr;
    uint8_t*    pConstants;
    uint32_t    codeSize;

    void Init(uint32_t constantSize)
    {
        pCurr = pCode;
        SPP::Bin::SHeader* pHeader = reinterpret_cast< SPP::Bin::SHeader* >( pCurr );
        pHeader->constantSize = constantSize;
        pHeader->constantStartOffset = sizeof( SPP::Bin::SHeader );
        pHeader->codeStartOffset = pHeader->constantStartOffset + pHeader->constantSize;
        pHeader->codeSize = codeSize - pHeader->codeStartOffset;
        
        pConstants = pCode + pHeader->constantStartOffset;
        pCurr = pCode + pHeader->codeStartOffset;
    }

    uint32_t AddConstant(void* pPtr)
    {
        SPP::Bin::SConstant* c = reinterpret_cast< SPP::Bin::SConstant* >( pConstants );
        uint32_t offset = pConstants - pCode;
        c->offset = offset;
        c->Value.ptr = pPtr;
        pConstants += sizeof( SPP::Bin::SConstant );
        return offset;
    }

    void Add(Opcode op)
    {

    }

    template<typename T>
    void Add(const TInstruction< T >& Instr)
    {
        TInstruction< T >* pInstr = reinterpret_cast< TInstruction< T >* >( pCurr );
        *pInstr = Instr;
        pCurr += sizeof( TInstruction< T > );
    }

    void Add(Opcode op, uint32_t arg1, uint32_t arg2)
    {

    }
};

int MainDLL()
{
    
    const char* pFmt = "Hello";
    uint8_t aCode[1024];
    SCodeBuilder Builder;
    Builder.pCode = aCode;
    Builder.codeSize = sizeof(aCode);
    Builder.Init(128);
    uint32_t offset = Builder.AddConstant( (void*)pFmt );
    TInstruction< SPP::Bin::SLoadConstant > Load;
    Load.Op.op = Opcodes::LOAD;
    Load.Op.mask.arg1Constant = 1;
    Load.Data.reg1 = 1;
    Load.Data.offset = offset;
    SInstruction1 Push;
    Push.Op.op = Opcodes::PUSH;
    Push.reg1 = Load.Data.reg1;
    Builder.Add(Load);

    SPP::Bin::CVirtualMachine VM;
    VM.Execute(aCode, sizeof(aCode));
    return 0;
}

int MainCSharp()
{
    return 0;
}

int main()
{
    int ret = MainDLL();
    _CrtDumpMemoryLeaks();
    return ret;
}