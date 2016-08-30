#pragma once

#include <string>
#include "FastHash.h"

class StringHash : public FastHash<std::string>
{
private:
	static const unsigned int fnv32Offset = 2166136261u;
	static const unsigned int fnv32Prime = 16777619u;

	static unsigned int ArrayToHash(const char* buffer, int len);

	static bool Equal(std::string& value, const char *item, int count);

	virtual bool Equal(std::string& lhs, std::string& rhs);
	virtual unsigned int Hash(std::string& item);

public:

};


