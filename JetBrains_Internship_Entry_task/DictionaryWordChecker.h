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

	WordDictionary(wxString filename) {
		FILE* in;
		fopen_s(&in, filename, "rb");

		fread(&chars, sizeof(chars[0]), BUFFER_SIZE, in);

		fclose(in);
	}

	const buffer_type_t* GetNewLineDelimitedString(OffsetArray::offset_t offset) {
		return chars + offset;
	}

	wxString GetString(OffsetArray::offset_t offset) {
		return wxString(GetPtrOf(chars + offset, '\n', false) + 1, GetPtrOf(chars + offset, '\n'));
	}

	int StrCmpAInB(const buffer_type_t* a ,const buffer_type_t* b) {
		int res = 0;
		size_t i = 0;
		while (!IsDelimiter(a[i]) &&
			!IsDelimiter(b[i])) {
			if (a[i] != b[i])
				return a[i] - b[i];
			++i;
		}
		if (IsDelimiter(a[i]) && !IsDelimiter(b[i]))
			return 0;
		else if (IsDelimiter(b[i]) && !IsDelimiter(a[i]))
			return 1;
		return 0;
	}

	bool IsDelimiter(wchar_t c) {
		return c == '\0' || c == '\n';
	}

protected:
	char chars[BUFFER_SIZE];

	buffer_type_t* GetPtrOf(buffer_type_t* from, buffer_type_t c, bool forward = true) {
		
		while (from >= chars && from < (chars + BUFFER_SIZE) && *from != c)
			if (forward)
				from++;
			else
				from--;
		return from;
	}
};