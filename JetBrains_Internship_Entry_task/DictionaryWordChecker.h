#pragma once

#include <wx/string.h>

#include "SuffixArray.h"

class PartiallyBoldString;
class WordDictionary;

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

	bool CheckStrWithOffset(ssize_t suffixIndex);

	wxString word;
	WordDictionary* dict;
	ssize_t foundWordOffsetIndex;
	bool checkedAllBack = false;
	bool checkedAllForward = false;
	ssize_t backOffsetIndex;
	ssize_t forwardOffsetIndex;
};