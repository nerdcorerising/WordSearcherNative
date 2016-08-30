#include "StringHash.h"
#include <cmath>

using namespace std;

unsigned int StringHash::ArrayToHash(const char* buffer, int len)
{
    unsigned int hash = fnv32Offset;

    for (int i = 0; i < len; i++)
    {
        hash = hash ^ buffer[i];
        hash *= fnv32Prime;
    }

    return hash;
}

bool StringHash::Equal(string& value, const char *buffer, int count)
{
	if (value.size() != count)
	{
		return false;
	}

	for (int i = 0; i < count; ++i)
	{
		if (value[i] != buffer[i])
		{
			return false;
		}
	}

	return true;
}

bool StringHash::Equal(string& lhs, string& rhs)
{
	return Equal(lhs, rhs.c_str(), rhs.size());
}

unsigned int StringHash::Hash(string& item)
{
	return ArrayToHash(item.c_str(), item.size());
}

