#pragma once

#include <wx/string.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

class PartiallyBoldString;
class OffsetArray;
class WordDictionary;

class OffsetArray
{
public:
	static const size_t MAX_OFFSETS_ARRAY_SIZE = 4396442;
	typedef uint32_t offset_t;


	OffsetArray(wxString filename);

	offset_t GetOffset(size_t index);

	offset_t GetNumOffsets();

protected:
	offset_t numOffsets;
	offset_t offsets[MAX_OFFSETS_ARRAY_SIZE];
};

class DictionaryWordChecker
{
public:
	DictionaryWordChecker(const wxString& filepath, const wxString& suffixArrayFilepath);
	~DictionaryWordChecker();

	void Reset();

	void SetWordToCheck(const wxString& word);

	bool CheckNext(PartiallyBoldString& outBStr);

	bool CheckedAll();

	ssize_t FindWord(const wxString& word);

private:

	bool CheckStrWithOffset(ssize_t offsetIndex);

	wxString word;
	OffsetArray* offsetArray;
	WordDictionary* dict;
	ssize_t foundWordOffsetIndex;
	bool checkedAllBack = false;
	bool checkedAllForward = false;
	ssize_t backOffsetIndex;
	ssize_t forwardOffsetIndex;
};



// TODO: move functions definitions to cpp file

class WordDictionary
{
public:
	static const size_t BUFFER_SIZE = 38567249;

	typedef char buffer_type_t;

	WordDictionary(const wxString& filename);

	const buffer_type_t* GetNewLineDelimitedString(OffsetArray::offset_t offset);

	wxString GetString(OffsetArray::offset_t offset);

	void AppendPBString(PartiallyBoldString& out, OffsetArray::offset_t offset, OffsetArray::offset_t boldLen);

	int StrCmpAInB(const buffer_type_t* a, const buffer_type_t* b);

	bool IsDelimiter(wchar_t c);

protected:
	char chars[BUFFER_SIZE];

	buffer_type_t* GetPtrOf(buffer_type_t* from, buffer_type_t c, bool forward = true);
};