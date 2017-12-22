#pragma once

#include "bin/Bin.h"

namespace SPP
{
    namespace ASM
    {
        struct SError
        {
            uint32_t    line;
            std::string error;
        };

        struct SCompileDesc
        {
            const char* pCode;
            uint32_t    codeSize;
        };

        class SPP_API CCompiler
        {
            public:

                using ErrorVec = std::vector< SError >;

            protected:

                using StrVec = std::vector< const char* >;

                struct SParseDesc
                {
                    const StrVec*   pvTokens;
                    ErrorVec*       pvErrors;
                    uint32_t        lineIdx;
                };

                using ParseFunc = bool( CCompiler::* )(const SParseDesc&);
                using OpcodeMap = std::unordered_map< std::string, ParseFunc >;

                struct SBinInstr
                {
                    uint8_t aBuffer[ 32 ];
                    uint8_t size;
                };

                using InstrVec = std::vector< SBinInstr >;  

            public:

                CCompiler();
                ErrorVec    Compile(const SCompileDesc& Desc);

            protected:

                struct SArgType
                {
                    uint8_t type;
                    uint8_t dataType;
                    uint8_t idx;
                };

                bool        _Parse(ErrorVec* pvOut);
                bool        _ParseArgs(const SParseDesc&, uint32_t count, SArgType*);
                const char* _ParseArg(const char*, SArgType*);
                bool        _ParseInstr(const StrVec& vTokens, uint32_t lineIdx, ErrorVec* pvOut);

                bool        _ParseMov(const SParseDesc&);
                /*bool        _ParseJmp( const SParseDesc& );
                bool        _ParseCall( const SParseDesc& );
                bool        _ParsePrnt( const SParseDesc& );
                bool        _ParsePush( const SParseDesc& );
                bool        _ParsePop( const SParseDesc& );
                bool        _ParseRet( const SParseDesc& );
                bool        _ParseAdd( const SParseDesc& );
                bool        _ParseSub( const SParseDesc& );
                bool        _ParseMul( const SParseDesc& );
                bool        _ParseDiv( const SParseDesc& );*/
                

            protected:

                char*           m_pAsmCode = nullptr;
                uint32_t        m_asmCodeSize;
                uint8_t*        m_pBinCode = nullptr;
                uint32_t        m_binCodeSize;
                InstrVec        m_vInstrs;
                OpcodeMap       m_mOpcodes;
        };

    } // ASM
} // SPP

//extern "C"
//{
//    SPP_API SPP::ASM::CCompiler* SppCreateASMCompiler();
//    void SPP_API SppDestroyASMCompiler(SPP::ASM::CCompiler**);
//}
