#pragma once

#include <cstdio>
#include <cstdint>

class wxString;

class SuffixArray
{
public:
	typedef uint32_t offset_t;

	virtual offset_t GetSuffixOffset(size_t index) const = 0;

	offset_t GetNumSuffixes();

	static SuffixArray* GetSuffixArray(const wxString& filepath, bool loadFileIntoMemory = true);

	virtual ~SuffixArray() = 0;

protected:
	offset_t numOffsets;
};

class StreamingSuffixArray : public SuffixArray
{
public:
	offset_t GetSuffixOffset(size_t index) const override;
	~StreamingSuffixArray();
protected:
	FILE* file;

	StreamingSuffixArray(const wxString& filename);

	friend class SuffixArray;
};

class InMemorySuffixArray : public SuffixArray
{
public:
	static const size_t MAX_OFFSETS_ARRAY_SIZE = 4396442;

	offset_t GetSuffixOffset(size_t index) const override;

protected:
	offset_t offsets[MAX_OFFSETS_ARRAY_SIZE];

	InMemorySuffixArray(const wxString& filename);

	friend class SuffixArray;
};