#include "spp/S++.h"
#include "bin/CVirtualMachine.h"
#include "asm/Asm.h"

namespace SPP
{
    SPP::Bin::CVirtualMachine g_BinVM;
    SPP::ASM::CCompiler g_AsmC;

    CSpp::CSpp()
    {
        printf( "spp\n" );
        g_BinVM = SPP::Bin::CVirtualMachine();
    }
}