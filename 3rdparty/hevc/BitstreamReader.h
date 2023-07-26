#ifndef BITSTREAM_READER_H_
#define BITSTREAM_READER_H_

class BitstreamReader
{
public:
	BitstreamReader(const uint8 *ptr, size_t size);
	bool getBit();
	uint32 getBits(size_t num);
	void skipBits(size_t num);
	uint32 showBits(size_t num);
	uint32 getGolombU();
	int32 getGolombS();

	size_t available();
	size_t availableInNalU();

private:
	const uint8	*m_ptr;
	size_t		m_size;
	size_t		m_posBase;
	size_t		m_posInBase;
};

#endif
