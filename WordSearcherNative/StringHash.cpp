#include "StringHash.h"
#include <cmath>

using namespace std;

StringHash::StringHash() :
    mCurrentBucketsSize(InitialBucketsSize),
    mBuckets(nullptr)
{
    mBuckets = new Node*[mCurrentBucketsSize];
    for (int i = 0; i < mCurrentBucketsSize; ++i)
    {
        mBuckets[i] = nullptr;
    }
}

StringHash::StringHash(const StringHash& other) :
    mCurrentBucketsSize(0),
    mBuckets(nullptr)
{
    mCurrentBucketsSize = other.mCurrentBucketsSize;
    mBuckets = new Node*[mCurrentBucketsSize];
    for (int i = 0; i < mCurrentBucketsSize; ++i)
    {
        mBuckets[i] = nullptr;

        Node* n = other.mBuckets[i];
        while (n != nullptr)
        {
            Node* temp = new Node();
            temp->hash = n->hash;
            temp->value = n->value;
            temp->next = mBuckets[i];
            mBuckets[i] = temp;
        }
    }
}

StringHash::StringHash(StringHash&& other)
{
    mBuckets = other.mBuckets;
    other.mBuckets = nullptr;
    mCurrentBucketsSize = other.mCurrentBucketsSize;
    other.mCurrentBucketsSize = 0;
}

StringHash& StringHash::operator=(const StringHash& other)
{
    mCurrentBucketsSize = other.mCurrentBucketsSize;
    mBuckets = new Node*[mCurrentBucketsSize];
    for (int i = 0; i < mCurrentBucketsSize; ++i)
    {
        if (mBuckets[i] != nullptr)
        {
            delete mBuckets[i];
            mBuckets[i] = nullptr;
        }

        Node* n = other.mBuckets[i];
        while (n != nullptr)
        {
            Node* temp = new Node();
            temp->hash = n->hash;
            temp->value = n->value;
            temp->next = mBuckets[i];
            mBuckets[i] = temp;
        }
    }

    return *this;
}

StringHash& StringHash::operator=(StringHash&& other)
{
    mBuckets = other.mBuckets;
    other.mBuckets = nullptr;
    mCurrentBucketsSize = other.mCurrentBucketsSize;
    other.mCurrentBucketsSize = 0;

    return *this;
}

StringHash::~StringHash()
{
    for (int i = 0; i < mCurrentBucketsSize; ++i)
    {
        Node* n = mBuckets[i];
        while (n != nullptr)
        {
            Node* next = n->next;
            delete n;
            n = next;
        }
    }
}

unsigned int StringHash::StringToHash(string value)
{
    unsigned int hash = fnv32Offset;

    for (size_t i = 0; i < value.size(); i++)
    {
        hash = hash ^ value[i];
        hash *= fnv32Prime;
    }

    return hash;
}

unsigned int StringHash::ArrayToHash(char* buffer, int len)
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
    Node** currArray = mBuckets;
    int newLength = mCurrentBucketsSize * 2;
    Node** newArray = new Node*[newLength];
    for (int i = 0; i < newLength; ++i)
    {
        newArray[i] = nullptr;
    }

    for (int i = 0; i < mCurrentBucketsSize; ++i)
    {
        Node* n = currArray[i];
        while (n != nullptr)
        {
            int pos = n->hash % newLength;
            Node* curr = newArray[pos];
            newArray[pos] = new Node();
            newArray[pos]->hash = n->hash;
            newArray[pos]->value = n->value;
            newArray[pos]->next = curr;

            n = n->next;
        }
    }

    mBuckets = newArray;
    mCurrentBucketsSize = newLength;
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

bool StringHash::Equal(string& value, char* buffer, int count)
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
        Node* n = hash.mBuckets[i];
        while (n != nullptr)
        {
            Add(n->value);

            n = n->next;
        }
    }
}

bool StringHash::Add(string item)
{
    int hash = abs((int)StringToHash(item));
    int pos = hash % mCurrentBucketsSize;
    Node* curr = mBuckets[pos];
    if (curr == nullptr)
    {
        curr = new Node();
        curr->hash = hash;
        curr->value = item;
        curr->next = nullptr;

        mBuckets[pos] = curr;
        return true;
    }
    else
    {
        int collisions = 0;
        Node* prev = curr;
        while (curr != nullptr)
        {
            ++collisions;

            if (curr->hash == hash && Equal(item, curr->value))
            {
                // Found our string
                return false;
            }

            prev = curr;
            curr = curr->next;
        }

        if (collisions >= MaxCollisions)
        {
            Resize();

            int newHash = abs((int)StringToHash(item));
            int newPos = hash % mCurrentBucketsSize;
            Node* temp = mBuckets[pos];
            mBuckets[pos] = new Node();
            mBuckets[pos]->hash = hash;
            mBuckets[pos]->value = item;
            mBuckets[pos]->next = temp;
        }
        else
        {
            prev->next = new Node();
            prev->next->hash = hash;
            prev->next->value = item;
            prev->next->next = nullptr;
        }

        return true;
    }
}

bool StringHash::Contains(string item)
{
    int hash = abs((int)StringToHash(item));
    int pos = hash % mCurrentBucketsSize;
    Node* curr = mBuckets[pos];
    while (curr != nullptr)
    {
        if (curr->hash == hash && Equal(curr->value, item))
        {
            return true;
        }

        curr = curr->next;
    }

    return false;
}

bool StringHash::Contains(char* buffer, int count)
{
    int hash = abs((int)ArrayToHash(buffer, count));
    int pos = hash % mCurrentBucketsSize;
    Node* curr = mBuckets[pos];
    while (curr != nullptr)
    {
        if (curr->hash == hash && Equal(curr->value, buffer, count))
        {
            return true;
        }

        curr = curr->next;
    }

    return false;
}

int StringHash::Count()
{
    int count = 0;
    for (int i = 0; i < mCurrentBucketsSize; ++i)
    {
        Node* n = mBuckets[i];
        while (n != nullptr)
        {
            ++count;
            n = n->next;
        }
    }

    return count;
}

vector<string> StringHash::GetList()
{
    vector<string> items;

    for (int i = 0; i < mCurrentBucketsSize; ++i)
    {
        Node* n = mBuckets[i];
        while (n != nullptr)
        {
            items.push_back(n->value);
            n = n->next;
        }
    }

    return items;
}
