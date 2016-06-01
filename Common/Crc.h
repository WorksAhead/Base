#ifndef CRC_HEADER_
#define CRC_HEADER_

#include <stdint.h>
#include <istream>

class Crc32 {
private:
	typedef unsigned char uchar;

public:
	Crc32() : crc_(0)
	{
	}

	void update(const void* data, size_t len);

	uint32_t value() const
	{
		return crc_;
	}

private:
	uint32_t crc_;
};

uint32_t calculateCRC(std::istream&, std::streamsize);

#endif // CRC_HEADER_

