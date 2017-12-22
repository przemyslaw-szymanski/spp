#include "asm/Asm.h"
#include <cctype>

//extern "C"
//{
//    SPP::ASM::CCompiler* SppCreateASMCompiler()
//    {
//        return new SPP::ASM::CCompiler();
//    }
//
//    void SppDestroyASMCompiler(SPP::ASM::CCompiler** ppC)
//    {
//        delete( *ppC );
//        *ppC = nullptr;
//    }
//}

namespace SPP
{
    namespace ASM
    {
        CCompiler::CCompiler()
        {
            m_mOpcodes[ "mov" ] = &CCompiler::_ParseMov;
            /*m_mOpcodes[ "jmp" ] = &CCompiler::_ParseJmp;
            m_mOpcodes[ "call" ] = &CCompiler::_ParseCall;
            m_mOpcodes[ "prnt" ] = &CCompiler::_ParsePrnt;
            m_mOpcodes[ "push" ] = &CCompiler::_ParsePush;
            m_mOpcodes[ "pop" ] = &CCompiler::_ParsePop;
            m_mOpcodes[ "ret" ] = &CCompiler::_ParseRet;
            m_mOpcodes[ "add" ] = &CCompiler::_ParseAdd;
            m_mOpcodes[ "sub" ] = &CCompiler::_ParseSub;
            m_mOpcodes[ "Mul" ] = &CCompiler::_ParseMul;
            m_mOpcodes[ "Div" ] = &CCompiler::_ParseDiv;*/
        }

        CCompiler::ErrorVec CCompiler::Compile(const SCompileDesc& Desc)
        {
            ErrorVec vErrors;
            if( m_pAsmCode )
            {
                delete m_pAsmCode;
            }
            m_pAsmCode = new char[ Desc.codeSize ] + 2;
            m_asmCodeSize = Desc.codeSize;
            
            for( uint32_t i = 0; i < Desc.codeSize; ++i )
            {
                m_pAsmCode[ i ] = std::tolower( Desc.pCode[ i ] );
            }
            m_pAsmCode[ Desc.codeSize ] = '\n';
            m_pAsmCode[ Desc.codeSize + 1 ] = '\0';

            _Parse( &vErrors );
            return vErrors;
        }

        using StrVec = std::vector< const char* >;
        void Tokenize(char* pCode, const char* delim, StrVec* pvOut)
        {
            const char* ptr = strtok( pCode, delim );
            while( ptr )
            {
                pvOut->push_back( ptr );
                ptr = strtok( nullptr, delim );
            }
        }

        bool CCompiler::_Parse(CCompiler::ErrorVec* pvOut)
        {
            StrVec vLines, vInstr;
            Tokenize( m_pAsmCode, "\n", &vLines );
            if( vLines.empty() )
            {
                SError Err;
                Err.line = 0;
                Err.error = "No lines of code.";
                pvOut->push_back(Err);
                return false;
            }
            uint32_t errorCount = 0;

            for( uint32_t i = 0; i < vLines.size(); ++i )
            {
                vInstr.clear();
                Tokenize( (char*)vLines[ i ], " ", &vInstr );
                errorCount += _ParseInstr( vInstr, i, pvOut );
            }
        }

        bool CCompiler::_ParseInstr(const StrVec& vTokens, uint32_t lineIdx, CCompiler::ErrorVec* pvOut)
        {
            //auto pFunc = m_mOpcodes[ vTokens[ 0 ] ];
            auto Itr = m_mOpcodes.find( vTokens[ 0 ] );
            if( Itr != m_mOpcodes.end() )
            {
                auto pFunc = Itr->second;
                SParseDesc Desc;
                Desc.pvErrors = pvOut;
                Desc.pvTokens = &vTokens;
                Desc.lineIdx = lineIdx;
                return ( this->*pFunc )( Desc );
            }
            else
            {
                SError Err;
                Err.line = lineIdx;
                std::stringstream ss;
                ss << "Unknown instruction: '" << vTokens[ 0 ] << "'.";
                pvOut->push_back( Err );
                return false;
            }
        }

        //std::stringstream g_strStream;

        enum
        {
            REGISTER = 0,
            CONSTANT = 1
        };

        enum
        {
            INT,
            UINT,
            FLOAT,
            PTR
        };    

        const char* CCompiler::_ParseArg(const char* pArg, SArgType* pOut)
        {
            uint32_t len = strlen( pArg );
            if( len > 2 )
            {
                switch( pArg[ len - 1 ] )
                {
                    case 'f': pOut->dataType = FLOAT; break;
                    case 'u': pOut->dataType = UINT; break;
                    case 'i': pOut->dataType = INT; break;
                    case 'p': pOut->dataType = PTR; break;
                    default: return "Invalid argument type.";
                }

                char value[ 4 ] = { 0 };
                memcpy_s( value, 4, pArg+1, len-2 );

                if( pArg[ 0 ] == 'r' )
                {
                    pOut->type = REGISTER;
                }
                else if( pArg[ 0 ] == 'c' )
                {
                    pOut->type = CONSTANT;
                }
                else
                {
                    return "Undefined argument type.";
                }

                pOut->idx = atoi( value );
                return "";
            }
            else
            {
                return "Invalid argument length.";
            }
        }

        bool CCompiler::_ParseArgs(const CCompiler::SParseDesc& Desc, uint32_t count, SArgType* paOut)
        {
            SError Err;
            Err.line = Desc.lineIdx;
            bool ret = true;
            for( uint32_t i = 0; i < count; ++i )
            {
                if( ( Err.error = _ParseArg( Desc.pvTokens->at( i+1 ), &paOut[i] ) ) != "" )
                {
                    Desc.pvErrors->push_back( Err );
                    ret = false;
                }
            }
            return ret;
        }

        bool CCompiler::_ParseMov(const SParseDesc& Desc)
        {
            if( Desc.pvTokens->size() != 3 )
            {
                SError Err;
                Err.error = "Invalid number of operands.";
                Err.line = Desc.lineIdx;
                Desc.pvErrors->push_back(Err);
                return false;
            }

            SArgType aArgs[ 4 ];
            if( !_ParseArgs( Desc, 2, aArgs ) )
            {
                return false;
            }


            SPP::Bin::Instructions::SInstr2 Instr;
            Instr.Opcode = SPP::Bin::Opcodes::MOV;
            Instr.dstRegIdx = aArgs[ 0 ].idx;
            Instr.srcRegIdx1 = aArgs[ 1 ].idx;
            SBinInstr BinInstr;
            BinInstr.size = sizeof( Instr );
            assert( sizeof( BinInstr.aBuffer ) > sizeof( Instr ) );
            memcpy_s( BinInstr.aBuffer, sizeof( BinInstr.aBuffer ), &Instr, sizeof( Instr ) );
            m_vInstrs.push_back( BinInstr );
            return true;
        }

    } // ASM
} // SPP