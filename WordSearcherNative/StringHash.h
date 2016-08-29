#pragma once

#include <string>
#include <vector>

class Node
{
public:
	bool initialized;
    unsigned int hash;
    std::string value;
};

class StringHash
{
private:
    static const int InitialBucketsSize = 100;
    static const int MaxCollisions = 10;

    static const unsigned int fnv32Offset = 2166136261u;
    static const unsigned int fnv32Prime = 16777619u;

    int mCurrentBucketsSize;
	int mCount;
    Node* mBuckets;

    static unsigned int StringToHash(std::string& value);
    static unsigned int ArrayToHash(const char* buffer, int len);
    void Resize();
    static bool Equal(std::string& value, std::string& item);
    static bool Equal(std::string& value, const char* buffer, int count);

	static bool AddInternal(Node* data, int dataCount, std::string& item, unsigned int hash, int& count);
	static bool AddInternal(Node* data, int dataCount, const char *item, int len, unsigned int hash, int& count);

public:
    StringHash();
    StringHash(const StringHash& other);
    StringHash(StringHash&& other);
    StringHash& operator=(const StringHash& other);
    StringHash& operator=(StringHash&& other);
    ~StringHash();

    bool Add(std::string& item);
	bool Add(const char *buffer, int count);
    void AddRange(StringHash& hash);
    bool Contains(std::string& item);
    bool Contains(const char* buffer, int count);
    int Count();
    std::vector<std::string> GetList();
};

