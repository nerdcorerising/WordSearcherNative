#pragma once

#include <string>
#include <vector>

class Node
{
public:
    int hash;
    std::string value;
    Node* next;
};

class StringHash
{
private:
    const int InitialBucketsSize = 100;
    const int MaxCollisions = 10;

    const unsigned int fnv32Offset = 2166136261u;
    const unsigned int fnv32Prime = 16777619u;

    int mCurrentBucketsSize;
    Node** mBuckets;

    unsigned int StringToHash(std::string value);
    unsigned int ArrayToHash(char* buffer, int len);
    void Resize();
    bool Equal(std::string& value, std::string& item);
    bool Equal(std::string& value, char* buffer, int count);

public:
    StringHash();
    StringHash(const StringHash& other);
    StringHash(StringHash&& other);
    StringHash& operator=(const StringHash& other);
    StringHash& operator=(StringHash&& other);
    ~StringHash();

    bool Add(std::string item);
    void AddRange(StringHash& hash);
    bool Contains(std::string item);
    bool Contains(char* buffer, int count);
    int Count();
    std::vector<std::string> GetList();
};

