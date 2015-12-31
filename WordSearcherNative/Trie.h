#pragma once

#include <string>

enum TrieContains
{
    Contains,
    ValidPrefix,
    InvalidPrefix
};

class CharTrie
{
private:
    const int ChildSize = 26;
    char mChar;
    bool mTerminator;
    CharTrie **mChildren;

    void Add(std::string str, int curPos);
    void SetTerminator(bool terminator);
    static int CharToIndex(char ch);
public:
    CharTrie();
    CharTrie(char ch, bool terminator);
    CharTrie(const CharTrie& other);
    CharTrie(CharTrie&& other);
    CharTrie& operator=(const CharTrie& other);
    CharTrie& operator=(CharTrie&& other);
    ~CharTrie();

    int MaxLength();
    bool IsTerminator();
    CharTrie* GetChildForChar(char ch);
    void Add(std::string str);
    TrieContains Contains(char* buffer, int length);
};

