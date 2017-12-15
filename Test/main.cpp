#include "Spp/S++.h"
#include <iostream>
#include <string>

using Opcode = SPP::Bin::SOpcode;
using Opcodes = SPP::Bin::Opcodes;
using OPCODE = SPP::Bin::OPCODE;

struct SCodeBuilder
{
    uint8_t*    pCode;
    uint8_t*    pCurr;
    uint8_t*    pConstants;
    uint32_t    codeSize;

    void Init(uint32_t startOffset, uint32_t constantSize)
    {
        pCurr = pCode + startOffset;
        pConstants = pCode;
    }

    void Add(Opcode op)
    {

    }

    void Add(Opcode op, uint32_t arg1)
    {
        Opcode* pOp = reinterpret_cast<Opcode*>(pCurr);
        *pOp = op;
        pOp++;
        pCurr = reinterpret_cast<uint8_t*>(pOp);
        *pCurr = arg1;
        pCurr++;
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
    Builder.Init();
    Opcode op;
    op.op = Opcodes::PUSH;
    op.mask.arg1IsConstant = 1;
    Builder.Add(op, 1);

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