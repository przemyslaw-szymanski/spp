#include "CParserASM.h"
#include <sstream>
#include <iterator>

namespace SPP
{
	namespace ASM
	{
		bool CParserASM::Parse(const uint8_t* pData, uint32_t dataSize, TextTokenVec* pOut)
		{
			std::string strData = reinterpret_cast<const char*>(pData);
			std::istringstream iss(strData);
			std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(*pOut));
			return true;
		}
	} // ASM
} // SPP