#pragma once

#include <vector>

template<typename T>
class FastHash
{
protected:
	class Node
	{
	public:
		bool initialized;
		unsigned int hash;
		T value;
	};

	static const int InitialBucketsSize = 100;
	static const int MaxCollisions = 10;

	int mCurrentBucketsSize;
	int mCount;
	Node* mBuckets;

	void Resize()
	{
		int newBucketsSize = mCurrentBucketsSize * 2;
		Node* newData = new Node[newBucketsSize];
		for (int i = 0; i < newBucketsSize; ++i)
		{
			newData[i].initialized = false;
		}

		int newCount = 0;
		for (int i = 0; i < mCurrentBucketsSize; ++i)
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

	bool AddInternal(Node* data, int dataCount, T& item, unsigned int hash, int& count)
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
				data[pos].value = item;
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

	virtual bool Equal(T& lhs, T& rhs) = 0;
	virtual unsigned int Hash(T& item) = 0;

public:
	FastHash() :
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

	FastHash(const FastHash& other)
	{
		mCurrentBucketsSize = other.mCurrentBucketsSize;
		mBuckets = new Node[mCurrentBucketsSize];
		for (int i = 0; i < mCurrentBucketsSize; ++i)
		{
			mBuckets[i] = other.mBuckets[i];
		}
	}

	FastHash(FastHash&& other)
	{
		mBuckets = other.mBuckets;
		other.mBuckets = nullptr;
		mCurrentBucketsSize = other.mCurrentBucketsSize;
		other.mCurrentBucketsSize = 0;
		mCount = other.mCount;
		other.mCount = 0;
	}

	FastHash& operator=(const FastHash& other)
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

	FastHash& operator=(FastHash&& other)
	{
		mBuckets = other.mBuckets;
		other.mBuckets = nullptr;
		mCurrentBucketsSize = other.mCurrentBucketsSize;
		other.mCurrentBucketsSize = 0;
		mCount = other.mCount;
		other.mCount = 0;

		return *this;
	}

	virtual ~FastHash()
	{
		delete[] mBuckets;
	}

	bool Add(T& item)
	{
		if (((double)mCount / (double)mCurrentBucketsSize) >= 0.33)
		{
			Resize();
		}

		return AddInternal(mBuckets, mCurrentBucketsSize, item, Hash(item), mCount);
	}

	void AddRange(FastHash<T>& hash)
	{
		for (int i = 0; i < hash.mCurrentBucketsSize; ++i)
		{
			if (hash.mBuckets[i].initialized)
			{
				string temp = hash.mBuckets[i].value;
				Add(temp);
			}
		}
	}

	bool Contains(T& item)
	{
		unsigned int hash = Hash(item);
		int pos = abs((int)(hash % mCurrentBucketsSize));

		while (true)
		{
			if (pos >= mCurrentBucketsSize)
			{
				pos = 0;
			}

			if (mBuckets[pos].hash == hash && Equal(mBuckets[pos].value, item))
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

	int Count()
	{
		return mCount;
	}

	std::vector<T> GetList()
	{
		vector<T> items;

		for (int i = 0; i < mCurrentBucketsSize; ++i)
		{
			if (mBuckets[i].initialized)
			{
				items.push_back(mBuckets[i].value);
			}
		}

		return items;
	}
};
