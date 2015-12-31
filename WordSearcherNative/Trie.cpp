#include "Trie.h"

using namespace std;

CharTrie::CharTrie() :
    mChar(0),
    mTerminator(false),
    mChildren(nullptr)
{
    mChildren = new CharTrie*[ChildSize];
    for (int i = 0; i < ChildSize; ++i)
    {
        mChildren[i] = nullptr;
    }
}

CharTrie::CharTrie(char ch, bool mTerminator) :
    mChar(ch),
    mTerminator(mTerminator),
    mChildren(new CharTrie*[ChildSize])
{
    mChildren = new CharTrie*[ChildSize];
    for (int i = 0; i < ChildSize; ++i)
    {
        mChildren[i] = nullptr;
    }
}

CharTrie::CharTrie(const CharTrie& other)
{
    mChar = other.mChar;
    mTerminator = other.mTerminator;

    for (int i = 0; i < ChildSize; ++i)
    {
        if (other.mChildren[i] != nullptr)
        {
            mChildren[i] = new CharTrie(*(other.mChildren[i]));
        }
        else
        {
            mChildren[i] = nullptr;
        }
    }
}

CharTrie::CharTrie(CharTrie&& other) :
    mChar(other.mChar),
    mTerminator(other.mTerminator),
    mChildren(other.mChildren)
{
    other.mChar = 0;
    other.mTerminator = false;
    other.mChildren = nullptr;
}

CharTrie& CharTrie::operator=(const CharTrie& other)
{
    mChar = other.mChar;
    mTerminator = other.mTerminator;

    for (int i = 0; i < ChildSize; ++i)
    {
        if (mChildren[i] != nullptr)
        {
            delete mChildren[i];
        }

        mChildren[i] = new CharTrie(other);
    }

    return *this;
}

CharTrie& CharTrie::operator=(CharTrie&& other)
{
    mChar = other.mChar;
    other.mChar = 0;
    mTerminator = other.mTerminator;
    other.mTerminator = false;
    mChildren = other.mChildren;
    other.mChildren = nullptr;

    return *this;
}

CharTrie::~CharTrie()
{
    for (int i = 0; i < ChildSize; ++i)
    {
        if (mChildren[i] != nullptr)
        {
            delete mChildren[i];
        }
    }

    delete[] mChildren;
}

void CharTrie::Add(string str, int curPos)
{
    char ch = str[curPos];
    int index = CharToIndex(ch);
    CharTrie *ct = mChildren[index];

    if (ct == nullptr)
    {
        ct = new CharTrie(ch, false);
        mChildren[index] = ct;
    }

    if (str.size() - 1 == curPos)
    {
        ct->SetTerminator(true);
        return;
    }

    ct->Add(str, curPos + 1);
}

int CharTrie::CharToIndex(char ch)
{
    return (int)((int)ch - (int)'A');
}

void CharTrie::SetTerminator(bool terminator)
{
    mTerminator = terminator;
}

int CharTrie::MaxLength()
{
    int maxLength = 0;
    for (int i = 0; i < ChildSize; ++i)
    {
        CharTrie* ct = mChildren[i];
        if (ct != nullptr)
        {
            int candidateLength = ct->MaxLength();
            if (candidateLength > maxLength)
            {
                maxLength = candidateLength;
            }
        }
    }

    // If all children are null, maxLength is 0, so 0 + 1 = 1 is correct.
    // Otherwise, the max length is 1 (the current position) + the max child length.
    return maxLength + 1;
}

bool CharTrie::IsTerminator()
{
    return mTerminator;
}

CharTrie* CharTrie::GetChildForChar(char ch)
{
    int index = CharToIndex(ch);
    CharTrie* ct = mChildren[index];

    return ct;
}

void CharTrie::Add(string str)
{
    Add(str, 0);
}

TrieContains CharTrie::Contains(char* buffer, int length)
{
    CharTrie* ct = this;
    int curPos = 0;

    TrieContains status;
    while (true)
    {
        if (curPos == length)
        {
            if (ct->mTerminator)
            {
                status = TrieContains::Contains;
                break;
            }
            status = TrieContains::ValidPrefix;
            break;
        }

        char ch = buffer[curPos];
        ct = ct->GetChildForChar(ch);
        ++curPos;
        if (ct == nullptr)
        {
            status = TrieContains::InvalidPrefix;
            break;
        }

    }

    return status;
}
