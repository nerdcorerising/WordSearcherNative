#include "StringHash.h"
#include <cmath>

using namespace std;

StringHash::StringHash() :
	mCurrentBucketsSize(InitialBucketsSize),
	mCount(0),
    mBuckets(nullptr)
{
    mBuckets = new Node[mCurrentBucketsSize];
    for (int i = 0; i < mCurrentBucketsSize; ++i)
    {
		mBuckets[i].initialized = false;
    }
}

StringHash::StringHash(const StringHash& other) :
    mCurrentBucketsSize(0),
	mCount(other.mCount),
    mBuckets(nullptr)
{
    mCurrentBucketsSize = other.mCurrentBucketsSize;
    mBuckets = new Node[mCurrentBucketsSize];
    for (int i = 0; i < mCurrentBucketsSize; ++i)
    {
		mBuckets[i] = other.mBuckets[i];
    }
}

StringHash::StringHash(StringHash&& other)
{
    mBuckets = other.mBuckets;
    other.mBuckets = nullptr;
    mCurrentBucketsSize = other.mCurrentBucketsSize;
    other.mCurrentBucketsSize = 0;
	mCount = other.mCount;
	other.mCount = 0;
}

StringHash& StringHash::operator=(const StringHash& other)
{
    mCurrentBucketsSize = other.mCurrentBucketsSize;
    mBuckets = new Node[mCurrentBucketsSize];
	mCount = other.mCount;
    for (int i = 0; i < mCurrentBucketsSize; ++i)
    {
		mBuckets[i] = other.mBuckets[i];
    }

    return *this;
}

StringHash& StringHash::operator=(StringHash&& other)
{
    mBuckets = other.mBuckets;
    other.mBuckets = nullptr;
    mCurrentBucketsSize = other.mCurrentBucketsSize;
    other.mCurrentBucketsSize = 0;
	mCount = other.mCount;
	other.mCount = 0;

    return *this;
}

StringHash::~StringHash()
{
	delete[] mBuckets;
}

unsigned int StringHash::StringToHash(string& value)
{
    unsigned int hash = fnv32Offset;

    for (size_t i = 0; i < value.size(); i++)
    {
        hash = hash ^ value[i];
        hash *= fnv32Prime;
    }

    return hash;
}

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

void StringHash::Resize()
{
	int newBucketsSize = mCurrentBucketsSize * 2;
	Node* newData = new Node[newBucketsSize];
	for (int i = 0; i < newBucketsSize; ++i)
	{
		newData[i].initialized = false;
	}

	int newCount = 0;
	for(int i = 0; i < mCurrentBucketsSize; ++i)
	{
		if (mBuckets[i].initialized)
		{
			AddInternal(newData, newBucketsSize, mBuckets[i].value, mBuckets[i].hash, newCount);
		}
	}

	Node* currArray = mBuckets;
	mBuckets = newData;
	mCurrentBucketsSize = newBucketsSize;
	mCount = newCount;

    delete[] currArray;
}

bool StringHash::Equal(string& value, string& item)
{
    if (value.size() != item.size())
    {
        return false;
    }

    for (size_t i = 0; i < item.size(); ++i)
    {
        if (value[i] != item[i])
        {
            return false;
        }
    }

    return true;
}

bool StringHash::Equal(string& value, const char* buffer, int count)
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

void StringHash::AddRange(StringHash& hash)
{
    for (int i = 0; i < hash.mCurrentBucketsSize; ++i)
    {
		if(hash.mBuckets[i].initialized)
        {
			string temp = hash.mBuckets[i].value;
            Add(temp);
        }
    }
}


bool StringHash::AddInternal(Node* data, int dataCount, const char *item, int itemLen, unsigned int hash, int& count)
{
	int pos = abs((int)(hash % dataCount));
	while (true)
	{
		if (pos >= dataCount)
		{
			pos = 0;
		}

		if (!data[pos].initialized)
		{
			string str(itemLen, 0);
			for (int i = 0; i < itemLen; ++i)
			{
				str[i] = item[i];
			}

			data[pos].initialized = true;
			data[pos].hash = hash;
			data[pos].value = move(str);
			count++;
			return true;
		}

		if (data[pos].hash == hash && Equal(data[pos].value, item, itemLen))
		{
			return false;
		}

		++pos;
	}
}

bool StringHash::AddInternal(Node* data, int dataCount, string& item, unsigned int hash, int& count)
{
	int pos = abs((int)(hash % dataCount));
	while (true)
	{
		if (pos >= dataCount)
		{
			pos = 0;
		}

		if (!data[pos].initialized)
		{
			data[pos].initialized = true;
			data[pos].hash = hash;
			data[pos].value = move(item);
			count++;
			return true;
		}

		if (data[pos].hash == hash && Equal(data[pos].value, item))
		{
			return false;
		}

		++pos;
	}
}

bool StringHash::Add(const char *buffer, int count)
{
	if (((double)mCount / (double)mCurrentBucketsSize) >= 0.33)
	{
		Resize();
	}

	return AddInternal(mBuckets, mCurrentBucketsSize, buffer, count, ArrayToHash(buffer, count), mCount);
}

bool StringHash::Add(string& item)
{
	if (((double)mCount / (double)mCurrentBucketsSize) >= 0.33)
	{
		Resize();
	}

	return AddInternal(mBuckets, mCurrentBucketsSize, item, StringToHash(item), mCount);
}

bool StringHash::Contains(string& item)
{
	return Contains(item.c_str(), item.size());
}

bool StringHash::Contains(const char* buffer, int count)
{
	unsigned int hash = ArrayToHash(buffer, count);
	int pos = abs((int)(hash % mCurrentBucketsSize));

	while (true)
	{
		if (pos >= mCurrentBucketsSize)
		{
			pos = 0;
		}

		if (mBuckets[pos].hash == hash && Equal(mBuckets[pos].value, buffer, count))
		{
			return true;
		}

		if (!(mBuckets[pos].initialized))
		{
			return false;
		}

		++pos;
	}
}

int StringHash::Count()
{
    return mCount;
}

vector<string> StringHash::GetList()
{
    vector<string> items;

    for (int i = 0; i < mCurrentBucketsSize; ++i)
    {
		if (mBuckets[i].initialized)
		{
			items.push_back(mBuckets[i].value);
		}
    }

    return items;
}
