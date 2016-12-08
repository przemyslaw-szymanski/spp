#pragma once

#include "spp/SppCommon.h"

namespace SPP
{
	namespace ASM
	{
		using TextToken = std::string;
		using TextTokenVec = std::vector< TextToken >;
		using ByteCodeToken = uint8_t;
		using ByteCodeVec = std::vector< ByteCodeToken >;

		struct SByteCodeBuffer
		{
			ByteCodeToken*	m_pMem = nullptr;
			ByteCodeToken*	m_pCurrMem = nullptr;
			ByteCodeToken*	m_pEnd = nullptr;
			ByteCodeVec		m_vTmp;
			uint32_t		m_memSize = 0;

			SByteCodeBuffer()
			{
				//m_vTmp.resize(1024 * 1024); // 1MB
			}

			bool Create(uint32_t size)
			{
				m_pMem = new(std::nothrow) ByteCodeToken[size];
				if (!m_pMem)
				{
					return false;
				}
				m_memSize = size;
				m_pCurrMem = m_pMem;
				m_pEnd = m_pMem + m_memSize;
				return true;
			}

			bool Resize(uint32_t sizeNeeded)
			{
				if (m_pCurrMem + sizeNeeded >= m_pEnd)
				{
					auto lastSize = m_memSize;
					ByteCodeToken* pTmp = new(std::nothrow) ByteCodeToken[lastSize];
					if (!pTmp)
					{
						return false;
					}
					SPP_MEMCPY(pTmp, m_memSize, m_pMem, m_memSize);
					delete[] m_pMem;
					if (!Create(m_memSize + sizeNeeded))
					{
						return false;
					}
					SPP_MEMCPY(m_pCurrMem, m_memSize, pTmp, lastSize);
					m_pCurrMem += lastSize;
					delete[] pTmp;
				}
				return true;
			}

			template<typename _T_>
			bool AppendData(const _T_& data)
			{
				if (!Resize(sizeof(data)))
				{
					return false;
				}
				*m_pCurrMem = data;
				m_pCurrMem += sizeof(data);
				return true;
			}
		};
	} // ASM
} // SPP