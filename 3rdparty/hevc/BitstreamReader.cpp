#include "stdafx.h"
#include "BitstreamReader.h"

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

BitstreamReader::BitstreamReader(const uint8 *ptr, size_t size)
	: m_ptr(ptr)
	, m_size(size)
	, m_posBase(0)
	, m_posInBase(CHAR_BIT - 1)
{
}

size_t BitstreamReader::available()
{
	return (m_size - m_posBase -1) * CHAR_BIT + m_posInBase + 1;
}

size_t BitstreamReader::availableInNalU()
{
	size_t pos = m_posBase;
	if(m_posInBase)
		pos++;
	for(; pos<(m_size - 3); pos++)
	{
		bool naluFinded = m_ptr[pos] == 0 && m_ptr[pos+1] == 0 && m_ptr[pos+2] == 1;
		
		if(!naluFinded)
		{
			if(m_size - pos >= 4 && m_ptr[pos] == 0 && m_ptr[pos+1] == 0 && m_ptr[pos+2] == 0 && m_ptr[pos+3] == 1)
				naluFinded = true;
		}
		
		if(naluFinded)
		{
			return (pos - m_posBase - 1) * CHAR_BIT + m_posInBase + 1;
		}
	}
	return m_size;
}

bool BitstreamReader::getBit()
{
	if(m_posBase < m_size)
	{
		bool res = (m_ptr[m_posBase] & 1 << m_posInBase) ? true : false;
	
		m_posInBase--;
	
		if(m_posInBase > CHAR_BIT)
		{
			m_posInBase = CHAR_BIT-1;
			m_posBase++;
		
			if(m_posBase >= 2)
			{
				if(m_ptr[m_posBase - 2] == 0 && m_ptr[m_posBase - 1] == 0 && m_ptr[m_posBase] == 3)
					m_posBase++;
			}
		}
	
		return res;
	}
	return false;
}

uint32 BitstreamReader::getBits(size_t num)
{
	if(num <= 32)
	{
		uint32 result = 0;
		for(size_t i=0; i<num; i++)
			if(getBit())
				result |= 1 << (num - i - 1);

		return result;
	}
	return 0;
}

void BitstreamReader::skipBits(size_t num)
{
	if(m_posBase >= 2)
	{
		if(m_ptr[m_posBase - 2] == 0 && m_ptr[m_posBase - 1] == 0 && m_ptr[m_posBase] == 3)
			m_posBase++;
	}
	
	uint32 scipBytes = (uint32)num / 8;
	
	while(scipBytes)
	{
		scipBytes--;
		m_posBase++;
		if(m_posBase >= 2)
		{
			if(m_ptr[m_posBase - 2] == 0 && m_ptr[m_posBase - 1] == 0 && m_ptr[m_posBase] == 3)
				m_posBase++;
		}
	}
	
	if(m_posInBase > num % 8)
		m_posInBase -= num % 8;
	else
	{
		m_posBase++;
		m_posInBase = m_posInBase - num % 8 + 8;
	}
}

uint32 BitstreamReader::showBits(size_t num)
{
	if(num <= 32)
	{
		size_t posBasePrev = m_posBase;
		size_t posInBasePrev = m_posInBase;
		
		uint32 result = 0;
		for(size_t i=0; i<num; i++)
			if(getBit())
				result |= 1 << (num - i - 1);
		
		m_posBase = posBasePrev;
		m_posInBase = posInBasePrev;
		
		return result;
	}
	return 0;
}

uint32 BitstreamReader::getGolombU()
{
	long numZeroBits = -1;
	for(long bit = 0; !bit; numZeroBits++)
		bit = getBit();
	
	if(numZeroBits >= 32)
		return 0;
	
	return (1 << numZeroBits) - 1 + getBits(numZeroBits);
}

int32 BitstreamReader::getGolombS()
{
	int32 buf = getGolombU();
	
	if (buf & 1)
		buf = (buf + 1) >> 1;
	else
		buf = -(buf >> 1);
	
	return buf;
}
