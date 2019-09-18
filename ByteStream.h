
#pragma once
#define __STDC_WANT_LIB_EXT1__
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
 
#define FLAG 1010
#define VER 0
#define HEAD_SIZE 8
 
#define ASSERT_T(x) assert(x)
 
class ByteStream
{
public:
	ByteStream(uint32_t reserveSize);
	~ByteStream();
 
	void writeUInt32(uint32_t value);
	void writeInt32(int32_t value);
 
	void writeUInt16(uint16_t value);
	void writeInt16(int16_t value);
 
	void writeUInt8(uint8_t value);
	void writeInt8(int8_t value);
 
	void writeBytes(const char* value, uint32_t size);
 
	uint32_t readUInt32(uint32_t pos) const;
	int32_t readInt32(int32_t pos) const;
 
	uint16_t readUInt16(uint32_t pos) const;
	int16_t readInt16(int32_t pos) const;
 
	uint8_t readUInt8(uint32_t pos) const;
	int8_t readInt8(int32_t pos) const;
 
	char* getBuffer(uint32_t pos = 0);
	void readBytes(char* dest, uint32_t destSize, uint32_t pos) const;
	void useSize(uint32_t size);
 
	uint32_t size() const;
	uint32_t reserveSize() const;
 
	void remove(uint32_t size);
	void clear();
 
private:
	template<typename T>
	void writeValue(T value);
	template<typename T>
	T readValue(uint32_t pos) const;
	void extend(uint32_t newSize);
	bool bufferEnough(uint32_t size) const;
 
private:
	char* buffer;
	uint32_t bufferSize;
	uint32_t usedSize;
};