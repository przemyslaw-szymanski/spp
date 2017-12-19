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
    uint8_t*    pCodeStart;
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
        pCodeStart = pCode + pHeader->codeStartOffset;
        pCurr = pCodeStart;
    }

    uint32_t AddConstant(void* pPtr, uint32_t size)
    {
        SPP::Bin::SConstant* c = reinterpret_cast< SPP::Bin::SConstant* >( pConstants );
        uint32_t offset = pConstants - pCode;
        c->offset = offset;
        c->Value.ptr = pPtr;
        pConstants += sizeof( SPP::Bin::SConstant );

        memcpy_s(pConstants, size, pPtr, size);
        pConstants += size;

        return offset;
    }

    template<typename T>
    uint32_t AddConstant(const T& v)
    {
        SPP::Bin::SConstant* c = reinterpret_cast< SPP::Bin::SConstant* >(pConstants);
        uint32_t offset = pConstants - pCode;
        c->offset = offset;
        memcpy_s(&c->Value, sizeof(c->Value), &v, sizeof(T));
        pConstants += sizeof(SPP::Bin::SConstant);

        const uint32_t size = sizeof(T);
        memcpy_s(pConstants, size, &v, size);
        pConstants += size;

        return offset;
    }

    void Add(Opcode op)
    {

    }

    template<typename T>
    uint32_t Add(const T& Instr)
    {
        T* pInstr = reinterpret_cast< T* >( pCurr );
        *pInstr = Instr;
        uint32_t offset = pCurr - pCodeStart;
        pCurr += sizeof( T );
        return offset;
    }

    void Add(Opcode op, uint32_t arg1, uint32_t arg2)
    {

    }
};

int MainDLL()
{
    
    const char* pFmt = "Hello %d";
    uint8_t aCode[1024];
    SCodeBuilder Builder;
    Builder.pCode = aCode;
    Builder.codeSize = sizeof(aCode);
    Builder.Init(128);
    uint32_t offset = Builder.AddConstant( (void*)pFmt, strlen(pFmt)+1 );
    uint32_t offset2 = Builder.AddConstant<uint32_t>(123);
    SPP::Bin::Instructions::SLoad Load;
    SPP::Bin::Instructions::SInstr1 Instr1;
    Load.Opcode = Opcodes::LOAD;
    Load.Mask.isConstant = 1;
    Load.Mask.type = SPP::Bin::RegDataTypes::PTR;
    Load.dstRegIdx = 1;
    Load.offset = offset;
    offset = Builder.Add(Load);

    Load.Mask.type = SPP::Bin::RegDataTypes::UINT_32;
    Load.dstRegIdx = 2;
    Load.offset = offset2;
    offset = Builder.Add(Load);

    Instr1.Opcode = Opcodes::PUSH;
    Instr1.dstRegIdx = 1;
    offset = Builder.Add(Instr1);

    Instr1.dstRegIdx = 2;
    Builder.Add(Instr1);

    Instr1.Opcode = Opcodes::PRNT;
    Instr1.arg1 = 2;
    Builder.Add(Instr1);

    SPP::Bin::Instructions::SJmp Jmp;
    Jmp.Opcode = Opcodes::JMP;
    Jmp.offset = offset;
    Builder.Add(Jmp);

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