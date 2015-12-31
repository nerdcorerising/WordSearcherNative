// WordSearcherNative.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <time.h>

#include "stdafx.h"
#include "Trie.h"
#include "StringHash.h"

using namespace std;

class Result
{
public:
    thread* t;
    StringHash* words;
};

string FileName = "english.txt";
string InputName = "input.txt";

void LaunchThread(CharTrie *englishWords, char *wordSearch, int rowCount, int colCount, StringHash *threadWords, int startRow, int endRow, int startCol, int endCol);
void LoadWordList(CharTrie& englishWords);
char* ReadFile(string fileName, int& rows, int& columns);
void FindWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, int startRow, int stopRow, int startCol, int stopCol);
void FindLowerRightWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol);
void FindLowerWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol);
void FindLowerLeftWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol);
void FindRightWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol);
void FindLeftWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol);
void FindUpperRightWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol);
void FindUpperWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol);
void FindUpperLeftWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol);
void GenerateRandomChars(int width, int height);
string BufferToCandidate(char* buffer, int len);

int main()
{
    time_t startTime = time(NULL);

    CharTrie *englishWords = new CharTrie();
    LoadWordList(*englishWords);
    
    int rowCount = 0;
    int colCount = 0;
    char* wordSearch = ReadFile(InputName, rowCount, colCount);
    if (wordSearch == nullptr)
    {
        return -1;
    }

    StringHash words;
    time_t algStartTime = time(NULL);

    vector<Result> results;

    int rowSection = rowCount / 2;
    int startRow = 0;
    int endRow = rowSection;
    int colSection = colCount / 2;

    while (true)
    {
        int startCol = 0;
        int endCol = colSection;

        while (true)
        {
            StringHash* threadWords = new StringHash();
            thread *t = new thread(LaunchThread, englishWords, wordSearch, rowCount, colCount, threadWords, startRow, endRow, startCol, endCol);
            Result r;
            r.t = t;
            r.words = threadWords;
            results.push_back(r);

            if (endCol == colCount)
            {
                break;
            }

            startCol = endCol;
            endCol += rowSection;
            if (endCol > colCount)
            {
                endCol = colCount;
            }
        }

        if (endRow == rowCount)
        {
            break;
        }

        startRow = endRow;
        endRow += rowSection;
        if (endRow > rowCount)
        {
            endRow = rowCount;
        }
    }

    for (size_t i = 0; i < results.size(); ++i)
    {
        Result result = results[i];
        result.t->join();
    }

    time_t algEndTime = time(NULL);
    for (size_t i = 0; i < results.size(); ++i)
    {
        Result result = results[i];
        words.AddRange(*(result.words));

        delete result.t;
        delete result.words;
    }

    time_t endTime = time(NULL);

    printf("%d words\r\n", words.Count());
    printf("Core search done in %f seconds\r\n", difftime(algEndTime, algStartTime));
    printf("Whole process took %f seconds\r\n", difftime(endTime, startTime));

    delete[] wordSearch;
    delete englishWords;
    return 0;
}

void LaunchThread(CharTrie *englishWords, char *wordSearch, int rowCount, int colCount, StringHash *threadWords, int startRow, int endRow, int startCol, int endCol)
{
    printf("Started thread startRow=%d endRow=%d startCol=%d endCol=%d\r\n", startRow, endRow, startCol, endCol);
    FindWords(englishWords, wordSearch, rowCount, colCount, threadWords, startRow, endRow, startCol, endCol);
}

void LoadWordList(CharTrie& englishWords)
{
    ifstream wordList(FileName);

    string line;
    while (getline(wordList, line))
    {
        for (size_t i = 0; i < line.size(); ++i)
        {
            line[i] = toupper(line[i]);
        }

        englishWords.Add(line);
    }
 }

char* ReadFile(string fileName, int& rowCount, int& colCount)
{
    char *wordSearch = nullptr;
    rowCount = 0;
    colCount = 0;
    ifstream inFile(fileName);
    string line;
    
    if (!getline(inFile, line))
    {
        return nullptr;
    }

    for (size_t i = 0; i < line.size(); ++i)
    {
        if (line[i] == ',')
        {
            line[i] = ' ';
        }
    }

    istringstream ss(line);
    if(!(ss >> rowCount >> colCount))
    {
        printf("First line of input file should specify the size of the input file in each dimension, separated by a comma.\r\n");
        return nullptr;
    }

    int numChars = rowCount * colCount;
    wordSearch = new char[numChars];
    
    const int bufferSize = 4096;
    char *buffer = new char[bufferSize];
    int nonWSChars = 0;
    while (nonWSChars < numChars)
    {
        inFile.read(buffer, bufferSize);
        for (int i = 0; i < inFile.gcount(); ++i)
        {
            char ch = buffer[i];
            if (isspace(ch))
            {
                continue;
            }

            wordSearch[nonWSChars] = buffer[i];
            ++nonWSChars;
        }

        if (inFile.eof() && nonWSChars < numChars)
        {
            printf("Invalid word search provided.\r\n");
            delete[] wordSearch;
            rowCount = 0;
            colCount = 0;
            return nullptr;
        }
    }

    return wordSearch;
}

void FindWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, int startRow, int stopRow, int startCol, int stopCol)
{
    char* buffer = new char[englishWords->MaxLength()];

    for (int currentRow = startRow; currentRow < stopRow; ++currentRow)
    {
        for (int currentCol = startCol; currentCol < stopCol; ++currentCol)
        {
            FindUpperLeftWords(englishWords, wordSearch, rowCount, colCount, words, buffer, currentRow, currentCol);

            FindUpperWords(englishWords, wordSearch, rowCount, colCount, words, buffer, currentRow, currentCol);

            FindUpperRightWords(englishWords, wordSearch, rowCount, colCount, words, buffer, currentRow, currentCol);

            FindLeftWords(englishWords, wordSearch, rowCount, colCount, words, buffer, currentRow, currentCol);

            FindRightWords(englishWords, wordSearch, rowCount, colCount, words, buffer, currentRow, currentCol);

            FindLowerLeftWords(englishWords, wordSearch, rowCount, colCount, words, buffer, currentRow, currentCol);

            FindLowerWords(englishWords, wordSearch, rowCount, colCount, words, buffer, currentRow, currentCol);

            FindLowerRightWords(englishWords, wordSearch, rowCount, colCount, words, buffer, currentRow, currentCol);
        }
    }
}

void FindLowerRightWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol)
{
    int curPos = 0;
    int row = currentRow;
    int col = currentCol;
    CharTrie* ct = englishWords;
    string candidate;
    while (row < rowCount && col < colCount)
    {
        buffer[curPos] = wordSearch[(row * colCount) + col];

        ct = ct->GetChildForChar(buffer[curPos]);
        if (ct == nullptr)
        {
            break;
        }

        ++curPos;
        if (ct->IsTerminator())
        {
            if (!words->Contains(buffer, curPos))
            {
                candidate = BufferToCandidate(buffer, curPos);
                words->Add(candidate);
            }
        }

        ++row;
        ++col;
    }
}

void FindLowerWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol)
{
    int curPos = 0;
    int row = currentRow;
    int col = currentCol;
    CharTrie* ct = englishWords;
    string candidate;
    while (row < rowCount)
    {
        buffer[curPos] = wordSearch[(row * colCount) + col];

        ct = ct->GetChildForChar(buffer[curPos]);
        if (ct == nullptr)
        {
            break;
        }

        ++curPos;
        if (ct->IsTerminator())
        {
            if (!words->Contains(buffer, curPos))
            {
                candidate = BufferToCandidate(buffer, curPos);
                words->Add(candidate);
            }
        }

        ++row;
    }
}

void FindLowerLeftWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol)
{
    int curPos = 0;
    int row = currentRow;
    int col = currentCol;
    CharTrie* ct = englishWords;
    string candidate;
    while (row < rowCount && col >= 0)
    {
        buffer[curPos] = wordSearch[(row * colCount) + col];

        ct = ct->GetChildForChar(buffer[curPos]);
        if (ct == nullptr)
        {
            break;
        }

        ++curPos;
        if (ct->IsTerminator())
        {
            if (!words->Contains(buffer, curPos))
            {
                candidate = BufferToCandidate(buffer, curPos);
                words->Add(candidate);
            }
        }

        ++row;
        --col;
    }
}

void FindRightWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol)
{
    int curPos = 0;
    int row = currentRow;
    int col = currentCol;
    CharTrie* ct = englishWords;
    string candidate;
    while (col < colCount)
    {
        buffer[curPos] = wordSearch[(row * colCount) + col];

        ct = ct->GetChildForChar(buffer[curPos]);
        if (ct == nullptr)
        {
            break;
        }

        ++curPos;
        if (ct->IsTerminator())
        {
            if (!words->Contains(buffer, curPos))
            {
                candidate = BufferToCandidate(buffer, curPos);
                words->Add(candidate);
            }
        }

        ++col;
    }
}

void FindLeftWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol)
{
    int curPos = 0;
    int row = currentRow;
    int col = currentCol;
    CharTrie* ct = englishWords;
    string candidate;
    while (col >= 0)
    {
        buffer[curPos] = wordSearch[(row * colCount) + col];

        ct = ct->GetChildForChar(buffer[curPos]);
        if (ct == nullptr)
        {
            break;
        }

        ++curPos;
        if (ct->IsTerminator())
        {
            if (!words->Contains(buffer, curPos))
            {
                candidate = BufferToCandidate(buffer, curPos);
                words->Add(candidate);
            }
        }

        --col;
    }
}

void FindUpperRightWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol)
{
    int curPos = 0;
    int row = currentRow;
    int col = currentCol;
    CharTrie* ct = englishWords;
    string candidate;
    while (row >= 0 && col < colCount)
    {
        buffer[curPos] = wordSearch[(row * colCount) + col];

        ct = ct->GetChildForChar(buffer[curPos]);
        if (ct == nullptr)
        {
            break;
        }

        ++curPos;
        if (ct->IsTerminator())
        {
            if (!words->Contains(buffer, curPos))
            {
                candidate = BufferToCandidate(buffer, curPos);
                words->Add(candidate);
            }
        }

        --row;
        ++col;
    }
}

void FindUpperWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol)
{
    int curPos = 0;
    int row = currentRow;
    int col = currentCol;
    CharTrie* ct = englishWords;
    string candidate;
    while (row >= 0)
    {
        buffer[curPos] = wordSearch[(row * colCount) + col];

        ct = ct->GetChildForChar(buffer[curPos]);
        if (ct == nullptr)
        {
            break;
        }

        ++curPos;
        if (ct->IsTerminator())
        {
            if (!words->Contains(buffer, curPos))
            {
                candidate = BufferToCandidate(buffer, curPos);
                words->Add(candidate);
            }
        }

        --row;
    }
}

void FindUpperLeftWords(CharTrie* englishWords, char* wordSearch, int rowCount, int colCount, StringHash* words, char* buffer, int currentRow, int currentCol)
{
    int curPos = 0;
    int row = currentRow;
    int col = currentCol;
    CharTrie* ct = englishWords;
    string candidate;
    while (row >= 0 && col >= 0)
    {
        buffer[curPos] = wordSearch[(row * colCount) + col];

        ct = ct->GetChildForChar(buffer[curPos]);
        if (ct == nullptr)
        {
            break;
        }

        ++curPos;
        if (ct->IsTerminator())
        {
            if (!words->Contains(buffer, curPos))
            {
                candidate = BufferToCandidate(buffer, curPos);
                words->Add(candidate);
            }
        }

        --row;
        --col;
    }
}

string BufferToCandidate(char* buffer, int len)
{
    string str(len, 0);
    for (int i = 0; i < len; ++i)
    {
        str[i] = buffer[i];
    }

    return str;
}