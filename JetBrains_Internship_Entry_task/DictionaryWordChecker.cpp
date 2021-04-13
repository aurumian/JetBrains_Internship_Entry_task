#include "DictionaryWordChecker.h"
#include <wx/stream.h>

#include "PartiallyBoldString.h"
#include "WordDictionary.h"


DictionaryWordChecker::DictionaryWordChecker(const wxString& filepath, const wxString& suffixArrayFilepath)
{
	dict = WordDictionary::GetWordDictionary(filepath, SuffixArray::GetSuffixArray(suffixArrayFilepath, false), false);
	foundWordOffsetIndex = -1;
	forwardOffsetIndex = -1;
	backOffsetIndex = -1;
	// TODO: if file cannot be opened send a message to the main thread

}

DictionaryWordChecker::~DictionaryWordChecker()
{
	delete dict;
}

void DictionaryWordChecker::Reset()
{
	foundWordOffsetIndex = -1;
}

void DictionaryWordChecker::SetWordToCheck(const wxString& word)
{
	this->word = word;
	checkedAllForward = false;
	checkedAllBack = false;
}

bool DictionaryWordChecker::CheckedAll()
{
	return checkedAllForward && checkedAllBack || word.empty();
}

ssize_t DictionaryWordChecker::FindWord(const wxString& word) {
	// do binary search using suffix array
	size_t start = 0;
	size_t end = dict->GetNumSuffixes();

	auto* w = word.c_str().AsChar();
	if (dict->IsDelimiter(*w))
		return -1;

	while (start != end) {
		size_t mid = (start + end) / 2;
		int cmpRes = dict->CmpStrWithSuffix(w, mid);
;		if (cmpRes == 0) {
			return mid;
		}
		if (end == start + 1) {
			cmpRes = dict->CmpStrWithSuffix(w, mid + 1);
			if (cmpRes == 0) {
				return mid;
			}
			else
				break;
		}
		if (cmpRes < 0)
			end = mid;
		else
			start = mid;
	}

	return -1;
}

bool DictionaryWordChecker::CheckStrWithOffset(ssize_t suffixIndex) {
	return dict->CmpStrWithSuffix(word, suffixIndex) == 0;
}

bool DictionaryWordChecker::CheckNext(PartiallyBoldString& outBStr)
{
	if (CheckedAll())
		return false;
	if (word.empty())
		return false;

	if (foundWordOffsetIndex == -1) {
		foundWordOffsetIndex = FindWord(word);
		if (foundWordOffsetIndex != -1)
		{
			forwardOffsetIndex = foundWordOffsetIndex;
			backOffsetIndex = foundWordOffsetIndex;
			dict->AppendPBString(outBStr, foundWordOffsetIndex, word.size());
			return true;
		}
		else {
			checkedAllBack = true;
			checkedAllForward = true;
			return false;
		}
	}

	if (!checkedAllBack) {
		--backOffsetIndex;
		if (CheckStrWithOffset(backOffsetIndex))
		{
			dict->AppendPBString(outBStr, backOffsetIndex, word.size());
			return true;
		}
		else 
		{
			checkedAllBack = true;
			return false;
		}
	}

	if (!checkedAllForward) {
		++forwardOffsetIndex;
		if (CheckStrWithOffset(forwardOffsetIndex))
		{
			dict->AppendPBString(outBStr, forwardOffsetIndex, word.size());
			return true;
		}
		else
		{
			checkedAllForward = true;
			return false;
		}
	}

	return false;
}