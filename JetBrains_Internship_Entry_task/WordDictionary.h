#pragma once

#include <cstdio>

#include "SuffixArray.h"

class wxString;
class PartiallyBoldString;

class WordDictionary
{
public:
	typedef char buffer_t;

	static WordDictionary* GetWordDictionary(const wxString& pathToDict, SuffixArray* suffixArray, bool inMemory = true);

	static bool IsDelimiter(buffer_t c);

	virtual void AppendPBString(PartiallyBoldString& out, size_t suffixIndex, SuffixArray::offset_t boldLen) = 0;
	// returns 0 even if only the first part of str is equal to the suffix
	virtual int CmpStrWithSuffix(const wxString& str, size_t suffixIndex) = 0;
	size_t GetNumSuffixes() const;

	virtual ~WordDictionary() = 0;
protected:
	WordDictionary(SuffixArray* suffixArray);
	SuffixArray* suffixArray;

	int CmpStrWithSuffix(const buffer_t* str, const buffer_t* suffix) const;

	// bufferEnd is exclusive
	static const buffer_t* GetPtrOf(const buffer_t* from, const buffer_t c, buffer_t* bufferBegin, buffer_t* bufferEnd, bool forward = true);
	static void AppendPBString(PartiallyBoldString& out, SuffixArray::offset_t boldLen, buffer_t* bufferBegin, buffer_t* cachedSuffixPtr, buffer_t* bufferEnd);
};


class InMemoryWordDictionary : public WordDictionary
{
public:
	static const size_t BUFFER_SIZE = 38567249;

	void AppendPBString(PartiallyBoldString& out, size_t suffixIndex, SuffixArray::offset_t boldLen) override;

	int CmpStrWithSuffix(const wxString& a, size_t suffixIndex) override;

protected:
	char chars[BUFFER_SIZE];

	InMemoryWordDictionary(const wxString& filename, SuffixArray* suffixArray);

	friend class WordDictionary;
};


class StreamingWordDictionary : public WordDictionary
{
	static const size_t MAX_WORD_LENGTH = 45;
	static const size_t BUFFER_LEN = MAX_WORD_LENGTH * 2 + 2;
public:
	StreamingWordDictionary(const wxString& filename, SuffixArray* suffixArray);

	void AppendPBString(PartiallyBoldString& out, size_t suffixIndex, SuffixArray::offset_t boldLen) override;

	int CmpStrWithSuffix(const wxString& a, size_t suffixIndex) override;

	~StreamingWordDictionary();

protected:
	FILE* file;

	// variable for caching 
	buffer_t buffer[BUFFER_LEN];
	size_t cachedSuffixIndex;
	// pointer to where the cached suffix is in buffer
	buffer_t* cachedSuffixPtr;

	void LoadStrAroundSuffix(size_t suffixIndex);
};