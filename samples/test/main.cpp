#include "asm/Asm.h"
#include "bin/CVirtualMachine.h"
#include <iostream>
#include <string>
#include <xhash>

using Opcode = SPP::Bin::SOpcode;
using Opcodes = SPP::Bin::Opcodes;
using OPCODE = SPP::Bin::OPCODE;
using SInstruction1 = SPP::Bin::SInstruction1;
template<typename T>
using TInstruction = SPP::Bin::TSInstruction< T >;

struct SEntryPointDesc
{
    SPP::Bin::SEntryPointDesc   Desc;
    const char*                 pName;
};

struct SCodeBuilder
{
    uint8_t*    pCode;
    uint8_t*    pCurr;
    uint8_t*    pConstantStart;
    uint8_t*    pCurrConstant;
    uint8_t*    pCodeStart;
    uint8_t*    pEntryPoints;
    uint32_t    codeSize;
    std::vector<SEntryPointDesc> vEntryPointDescs;
    std::vector<uint8_t*> vpEntryPoints;

    void Init(uint32_t constantSize)
    {
        pCurr = pCode;
        SPP::Bin::SHeader* pHeader = reinterpret_cast< SPP::Bin::SHeader* >( pCurr );
        pHeader->constantSize = constantSize;
        pHeader->entryPointCount = vEntryPointDescs.size();
        pHeader->entryPointStartOffset = sizeof( SPP::Bin::SHeader );
        pEntryPoints = ( pCode + pHeader->entryPointStartOffset );
        auto* pEntryPtr = reinterpret_cast<SPP::Bin::SEntryPointDesc*>( pEntryPoints );
        for( uint32_t i = 0; i < vEntryPointDescs.size(); ++i )
        {
            auto& Desc = vEntryPointDescs[ i ];
            Desc.Desc.hash = std::hash<const char*>{}( Desc.pName );
            *pEntryPtr = Desc.Desc;
            pEntryPtr++;
        }
        const uint32_t entryPointsSize = reinterpret_cast<uint8_t*>( pEntryPtr) - pEntryPoints;
        pHeader->constantStartOffset = pHeader->entryPointStartOffset + entryPointsSize;
        pHeader->codeStartOffset = pHeader->constantStartOffset + pHeader->constantSize;
        pHeader->codeSize = codeSize - pHeader->codeStartOffset;
        
        
        pConstantStart = pCode + pHeader->constantStartOffset;
        pCodeStart = pCode + pHeader->codeStartOffset;
        pCurr = pCodeStart;
        pCurrConstant = pConstantStart;

        for( uint32_t i = 0; i < vEntryPointDescs.size(); ++i )
        {
            uint8_t* pPtr = pCodeStart + vEntryPointDescs[ i ].Desc.offset;
            vpEntryPoints.push_back( pPtr );
        }

    }

    uint8_t* GetEntryPoint(uint32_t idx)
    {
        return vpEntryPoints[ idx ];
    }

    void SetCodePointer( uint8_t* ptr )
    {
        pCurr = ptr;
    }

    uint32_t AddConstant(void* pPtr, uint32_t size)
    {
        SPP::Bin::SConstant* c = reinterpret_cast< SPP::Bin::SConstant* >( pCurrConstant );
        uint32_t offset = pCurrConstant - pConstantStart;
        c->offset = offset;
        c->Value.ptr = pPtr;
        const uint32_t constantSize = sizeof( SPP::Bin::SConstant );
        pCurrConstant += constantSize;

        memcpy_s(pCurrConstant, size, pPtr, size);
        pCurrConstant += size;

        return offset;
    }

    template<typename T>
    uint32_t AddConstant(const T& v)
    {
        SPP::Bin::SConstant* c = reinterpret_cast< SPP::Bin::SConstant* >(pCurrConstant);
        uint32_t offset = pCurrConstant - pConstantStart;
        c->offset = offset;
        memcpy_s(&c->Value, sizeof(c->Value), &v, sizeof(T));
        pCurrConstant += sizeof(SPP::Bin::SConstant);

        const uint32_t size = sizeof(T);
        memcpy_s(pCurrConstant, size, &v, size);
        pCurrConstant += size;

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
    
    const char* pFmt = "Hello %d\n";
    uint8_t aCode[ 1024 ] = { 0 };
    
    SCodeBuilder Builder;
    Builder.pCode = aCode;
    Builder.codeSize = sizeof(aCode);
    
    {
        SEntryPointDesc Desc;
        Desc.pName = "main";
        Desc.Desc.offset = 0;
        Desc.Desc.size = 128;
        Desc.Desc.argCount = 0;
        Desc.Desc.retType = 0;
        Builder.vEntryPointDescs.push_back( Desc );
    }
    {
        SEntryPointDesc Desc;
        Desc.pName = "add";
        Desc.Desc.offset = 128;
        Desc.Desc.size = 128;
        Desc.Desc.argCount = 0;
        Desc.Desc.retType = 0;
        Builder.vEntryPointDescs.push_back( Desc );

    }
    Builder.Init( 128 );

    uint32_t fmtOffset = Builder.AddConstant( (void*)pFmt, strlen(pFmt)+1 );
    uint32_t offset2 = Builder.AddConstant<uint32_t>(123);
    SPP::Bin::Instructions::SLoad Load;
    SPP::Bin::Instructions::SInstr1 Instr1;
    SPP::Bin::Instructions::SInstr2 Instr2;
    SPP::Bin::Instructions::SInstr3 Instr3;

    Builder.SetCodePointer( Builder.GetEntryPoint( 0 ) );
    {
        Load.Opcode = Opcodes::LOAD;
        Load.Mask.isConstant = 1;
        Load.Mask.type = SPP::Bin::ValueTypes::PTR;
        Load.dstRegIdx = 1;
        Load.offset = fmtOffset;
        Builder.Add( Load );

        Load.Mask.type = SPP::Bin::ValueTypes::UINT;
        Load.dstRegIdx = 2;
        Load.offset = offset2;
        Builder.Add( Load );

        Instr1.Opcode = Opcodes::PUSH;
        Instr1.dstRegIdx = 1;
        Builder.Add( Instr1 );

        Instr1.dstRegIdx = 2;
        Builder.Add( Instr1 );

        Instr1.Opcode = Opcodes::PRNT;
        Instr1.arg1 = 2;
        Builder.Add( Instr1 );

        //Instr1.Opcode = Opcodes::RET;
        //Builder.Add( Instr1 );

        Instr1.Opcode = Opcodes::PUSH;
        Instr1.dstRegIdx = 2;
        Builder.Add( Instr1 );
        Builder.Add( Instr1 );

        /*SPP::Bin::Instructions::SJmp Jmp;
        Jmp.Opcode = Opcodes::JMP;
        Jmp.offset = 128;
        Builder.Add( Jmp );*/
        SPP::Bin::Instructions::SCall Call;
        Call.Opcode = Opcodes::CALL;
        Call.offset = Builder.vEntryPointDescs[1].Desc.offset;
        Builder.Add( Call );

        Instr1.Opcode = Opcodes::POP;
        Instr1.dstRegIdx = 3;
        Builder.Add( Instr1 );

        Instr1.Opcode = Opcodes::PUSH;
        Instr1.dstRegIdx = 1;
        Builder.Add( Instr1 );

        Instr1.Opcode = Opcodes::PUSH;
        Instr1.dstRegIdx = 3;
        Builder.Add( Instr1 );

        Instr1.Opcode = Opcodes::PRNT;
        Instr1.arg1 = 2;
        Builder.Add( Instr1 );
    }

    Builder.SetCodePointer( Builder.GetEntryPoint( 1 ) );
    {
        Instr1.Opcode = Opcodes::POP;
        Instr1.dstRegIdx = 1;
        Builder.Add( Instr1 );
        Instr1.dstRegIdx = 2;
        Builder.Add( Instr1 );

        Instr3.Opcode = Opcodes::ADDI;
        Instr3.dstRegIdx = 3;
        Instr3.srcRegIdx1 = 1;
        Instr3.srcRegIdx2 = 2;
        Builder.Add( Instr3 );

        Load.Opcode = Opcodes::LOAD;
        Load.Mask.isConstant = 1;
        Load.Mask.type = SPP::Bin::ValueTypes::PTR;
        Load.dstRegIdx = 4;
        Load.offset = fmtOffset;
        Builder.Add( Load );

        Instr1.Opcode = Opcodes::PUSH;
        Instr1.dstRegIdx = 4;
        Builder.Add( Instr1 );
        Instr1.dstRegIdx = 3;
        Builder.Add( Instr1 );

        Instr1.Opcode = Opcodes::PRNT;
        Instr1.arg1 = 2;
        Builder.Add( Instr1 );

        Instr1.Opcode = Opcodes::PUSH;
        Instr1.dstRegIdx = 3;
        Builder.Add( Instr1 );

        Instr1.Opcode = Opcodes::RET;
        Builder.Add( Instr1 );
    }

    SPP::Bin::CVirtualMachine VM;
    VM.Init(aCode, sizeof(aCode));

    VM.Execute( "main" );

    {
        SPP::ASM::CCompiler c;
        const char* pCode =
            "cnst c1p \"Hello %d\\n\""
            "mov r1u c1p\n"
            ;
        SPP::ASM::SCompileDesc Desc;
        Desc.pCode = pCode;
        Desc.codeSize = strlen( pCode ) + 1;
        c.Compile( Desc );
    }

    return 0;
}

int MainCSharp()
{
    return 0;
}

int main()
{
    int ret = MainDLL();
    //_CrtDumpMemoryLeaks();
    return 0;
}