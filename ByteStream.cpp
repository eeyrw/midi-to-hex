
#include "ByteStream.h"
#include <memory>
#include <algorithm>
#include <assert.h>

ByteStream::ByteStream(uint32_t reserveSize)
{
	usedSize = 0;
	bufferSize = reserveSize;
	if (bufferSize != 0)
	{
		buffer = new char[reserveSize];
		memset(buffer, 0, reserveSize);
	}
	else
	{
		buffer = NULL;
	}
}

ByteStream::~ByteStream()
{
	delete[] buffer;
	buffer = NULL;
}

void ByteStream::writeUInt32(uint32_t value)
{
	writeValue<uint32_t>(value);
}

void ByteStream::writeInt32(int32_t value)
{
	writeValue<int32_t>(value);
}

void ByteStream::writeUInt16(uint16_t value)
{
	writeValue<uint16_t>(value);
}

void ByteStream::writeInt16(int16_t value)
{
	writeValue<int16_t>(value);
}

void ByteStream::writeUInt8(uint8_t value)
{
	writeValue<uint8_t>(value);
}

void ByteStream::writeInt8(int8_t value)
{
	writeValue<int8_t>(value);
}

uint32_t ByteStream::readUInt32(uint32_t pos) const
{
	return readValue<uint32_t>(pos);
}

int32_t ByteStream::readInt32(int32_t pos) const
{
	return readValue<int32_t>(pos);
}

uint16_t ByteStream::readUInt16(uint32_t pos) const
{
	return readValue<uint16_t>(pos);
}

int16_t ByteStream::readInt16(int32_t pos) const
{
	return readValue<int16_t>(pos);
}

uint8_t ByteStream::readUInt8(uint32_t pos) const
{
	return readValue<uint8_t>(pos);
}

int8_t ByteStream::readInt8(int32_t pos) const
{
	return readValue<int8_t>(pos);
}

template <typename T>
void ByteStream::writeValue(T value)
{
	if (!bufferEnough(sizeof(T)))
	{
		extend(bufferSize + sizeof(T) + bufferSize / 2);
	}

	T *buff = (T *)(buffer + usedSize);
	*buff = value;
	usedSize += sizeof(T);
}

template <typename T>
T ByteStream::readValue(uint32_t pos) const
{
	ASSERT_T(usedSize > pos);
	return *((T *)(buffer + pos));
}

void ByteStream::writeBytes(const char *value, uint32_t size)
{
	if (!bufferEnough(size))
	{
		extend(bufferSize + size + bufferSize / 2);
	}

	memcpy(buffer + usedSize, value,size);
	usedSize += size;
}

char *ByteStream::getBuffer(uint32_t pos /* = 0*/)
{
	return buffer + pos;
}

void ByteStream::readBytes(char *dest, uint32_t destSize, uint32_t pos) const
{
	memcpy(dest, buffer + pos,destSize);
}

void ByteStream::extend(uint32_t newSize)
{
	if (newSize <= bufferSize)
		return;

	char *newBuffer = new char[newSize];
	memcpy(newBuffer, buffer,newSize );
	delete[] buffer;
	buffer = newBuffer;
	bufferSize = newSize;
}

uint32_t ByteStream::size() const
{
	return usedSize;
}

uint32_t ByteStream::reserveSize() const
{
	return bufferSize;
}

void ByteStream::remove(uint32_t size)
{
	if (usedSize < size)
		return;

	uint32_t remainSize = usedSize - size;
	for (uint32_t i = 0; i < remainSize; ++i)
	{
		buffer[i] = buffer[i + size];
	}
	usedSize = remainSize;
}

bool ByteStream::bufferEnough(uint32_t size) const
{
	return usedSize + size <= bufferSize;
}

void ByteStream::useSize(uint32_t size)
{
	ASSERT_T(bufferEnough(size));
	usedSize += size;
}

void ByteStream::clear()
{
	usedSize = 0;
}