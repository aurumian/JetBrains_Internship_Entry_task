#include "DictionaryWordChecker.h"
#include <wx/stream.h>

#include "PartiallyBoldString.h"


DictionaryWordChecker::DictionaryWordChecker(const wxString& filepath, const wxString& suffixArrayFilepath)
{
	offsetArray = new OffsetArray(suffixArrayFilepath);
	dict = new WordDictionary(filepath);
	foundWordOffsetIndex = -1;
	// TODO: if file cannot be opened send a message to the main thread

}

DictionaryWordChecker::~DictionaryWordChecker()
{
	delete offsetArray;
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
	size_t end = offsetArray->GetNumOffsets();

	auto* w = word.c_str().AsChar();
	if (dict->IsDelimiter(*w))
		return -1;
	while (start != end) {
		size_t mid = (start + end) / 2;
		auto offset = offsetArray->GetOffset(mid);
		const WordDictionary::buffer_type_t* str = dict->GetNewLineDelimitedString(offset);
		int cmpRes = dict->StrCmpAInB(w, str);
;		if (cmpRes == 0) {
			return mid;
		}
		if (end == start + 1) {
			cmpRes = dict->StrCmpAInB(w, str);
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

bool DictionaryWordChecker::CheckStrWithOffset(ssize_t offsetIndex) {
	auto* w = word.c_str().AsChar();
	auto offset = offsetArray->GetOffset(offsetIndex);
	const WordDictionary::buffer_type_t* str = dict->GetNewLineDelimitedString(offset);
	return dict->StrCmpAInB(w, str) == 0;
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
			dict->AppendPBString(outBStr, offsetArray->GetOffset(foundWordOffsetIndex), word.size());
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
			dict->AppendPBString(outBStr, offsetArray->GetOffset(backOffsetIndex), word.size());
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
			dict->AppendPBString(outBStr, offsetArray->GetOffset(forwardOffsetIndex), word.size());
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


OffsetArray::OffsetArray(wxString filename)
{
	FILE* in;
	fopen_s(&in, filename, "rb");
	fread(&numOffsets, sizeof(numOffsets), 1, in);

	fread(&offsets, sizeof(offsets[0]), numOffsets, in);

	fclose(in);
}

OffsetArray::offset_t OffsetArray::GetOffset(size_t index)
{
	if (index > MAX_OFFSETS_ARRAY_SIZE)
		index = MAX_OFFSETS_ARRAY_SIZE - 1;
	return offsets[index];
}

OffsetArray::offset_t OffsetArray::GetNumOffsets()
{
	return numOffsets;
}

WordDictionary::WordDictionary(const wxString& filename)
{
	FILE* in;
	fopen_s(&in, filename, "rb");

	fread(&chars, sizeof(chars[0]), BUFFER_SIZE, in);

	fclose(in);
}

const WordDictionary::buffer_type_t* WordDictionary::GetNewLineDelimitedString(OffsetArray::offset_t offset) {
	return chars + offset;
}

wxString WordDictionary::GetString(OffsetArray::offset_t offset) {
	return wxString(GetPtrOf(chars + offset, '\n', false) + 1, GetPtrOf(chars + offset, '\n'));
}

void WordDictionary::AppendPBString(PartiallyBoldString& out, OffsetArray::offset_t offset, OffsetArray::offset_t boldLen) {
	out.AppendNormal(wxString(GetPtrOf(chars + offset, '\n', false) + 1, chars + offset));
	out.AppendBold(wxString(chars + offset, chars + offset + boldLen));
	out.AppendNormal(wxString(chars + offset + boldLen, GetPtrOf(chars + offset, '\n')));
}

int WordDictionary::StrCmpAInB(const buffer_type_t* a, const buffer_type_t* b) {
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

bool WordDictionary::IsDelimiter(wchar_t c) {
	return c == '\0' || c == '\n';
}

WordDictionary::buffer_type_t* WordDictionary::GetPtrOf(buffer_type_t* from, buffer_type_t c, bool forward) {

	while (from >= chars && from < (chars + BUFFER_SIZE) && *from != c)
		if (forward)
			from++;
		else
			from--;
	return from;
}