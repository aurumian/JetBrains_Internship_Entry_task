#include <wx/string.h>

#include "WordDictionary.h"
#include "PartiallyBoldString.h"

bool WordDictionary::IsDelimiter(buffer_t c)
{
	return c == '\0' || c == '\n';
}

const WordDictionary::buffer_t* WordDictionary::GetPtrOf(const buffer_t* from, const buffer_t c,
	buffer_t* bufferBegin, buffer_t* bufferEnd, bool forward)
{
	while (from >= bufferBegin && from < bufferEnd && *from != c)
		if (forward)
			from++;
		else
			from--;
	return from;
}

WordDictionary::WordDictionary(SuffixArray* suffixArray)
{
	this->suffixArray = suffixArray;
}

WordDictionary::~WordDictionary()
{
	delete suffixArray;
}

size_t WordDictionary::GetNumSuffixes() const
{
	return suffixArray->GetNumSuffixes();
}

WordDictionary* WordDictionary::GetWordDictionary(const wxString& pathToDict, SuffixArray* suffixArray, bool inMemory)
{
	if (inMemory)
		return new InMemoryWordDictionary(pathToDict, suffixArray);
	else
		return new StreamingWordDictionary(pathToDict, suffixArray);
}

int WordDictionary::CmpStrWithSuffix(const buffer_t* str, const buffer_t* suffix) const
{
	int res = 0;
	size_t i = 0;
	while (!IsDelimiter(str[i]) &&
		!IsDelimiter(suffix[i])) {
		if (str[i] != suffix[i])
			return str[i] - suffix[i];
		++i;
	}
	if (IsDelimiter(str[i]) && !IsDelimiter(suffix[i]))
		return 0;
	else if (IsDelimiter(str[i]) && !IsDelimiter(suffix[i]))
		return 1;
	return 0;
}

void WordDictionary::AppendPBString(PartiallyBoldString& out, SuffixArray::offset_t boldLen, buffer_t* bufferBegin, buffer_t* cachedSuffixPtr, buffer_t* bufferEnd)
{
	out.AppendNormal(wxString(GetPtrOf(cachedSuffixPtr, '\n', bufferBegin, bufferEnd, false) + 1, cachedSuffixPtr));
	out.AppendBold(wxString(cachedSuffixPtr, cachedSuffixPtr + boldLen));
	out.AppendNormal(wxString(cachedSuffixPtr + boldLen, GetPtrOf(cachedSuffixPtr, '\n', bufferBegin, bufferEnd)));
}


InMemoryWordDictionary::InMemoryWordDictionary(const wxString& filename, SuffixArray* suffixArray) :
	WordDictionary(suffixArray)
{
	FILE* in;
	fopen_s(&in, filename, "rb");

	fread(&chars, sizeof(chars[0]), BUFFER_SIZE, in);

	fclose(in);
}

void InMemoryWordDictionary::AppendPBString(PartiallyBoldString& out, size_t suffixIndex, InMemorySuffixArray::offset_t boldLen)
{
	SuffixArray::offset_t offset = suffixArray->GetSuffixOffset(suffixIndex);
	WordDictionary::AppendPBString(out, boldLen, chars, chars + offset, chars + BUFFER_SIZE);
}

int InMemoryWordDictionary::CmpStrWithSuffix(const wxString& str, size_t suffixIndex)
{
	const buffer_t* b = chars + suffixArray->GetSuffixOffset(suffixIndex);
	auto* a = str.c_str().AsChar();
	return WordDictionary::CmpStrWithSuffix(a, b);
}


StreamingWordDictionary::StreamingWordDictionary(const wxString& filename, SuffixArray* suffixArray) :
	WordDictionary(suffixArray)
{
	fopen_s(&file, filename, "rb");
}

StreamingWordDictionary::~StreamingWordDictionary()
{
	fclose(file);
}

void StreamingWordDictionary::AppendPBString(PartiallyBoldString& out, size_t suffixIndex, SuffixArray::offset_t boldLen)
{
	LoadStrAroundSuffix(suffixIndex);
	WordDictionary::AppendPBString(out, boldLen, buffer, cachedSuffixPtr, buffer + BUFFER_LEN);
}

int StreamingWordDictionary::CmpStrWithSuffix(const wxString& str, size_t suffixIndex)
{
	LoadStrAroundSuffix(suffixIndex);
	auto* a = str.c_str().AsChar();
	return WordDictionary::CmpStrWithSuffix(a, cachedSuffixPtr);
}

void StreamingWordDictionary::LoadStrAroundSuffix(size_t suffixIndex) {
	if (suffixIndex == cachedSuffixIndex)
		return;
	cachedSuffixIndex = suffixIndex;
	SuffixArray::offset_t offset = suffixArray->GetSuffixOffset(suffixIndex);
	// load more to be able to use the cached data for AppendPBString operation
	// as it oftern comes after CmpStrWithSuffix operation (and  with the same suffixIndex arguement)
	if (offset <= MAX_WORD_LENGTH)
	{
		cachedSuffixPtr = buffer + offset;
		offset = 0;
	}
	else {
		cachedSuffixPtr = buffer + MAX_WORD_LENGTH;
		offset = offset - MAX_WORD_LENGTH;
	}
	fseek(file, offset, 0);
	fread(buffer, sizeof(buffer[0]), BUFFER_LEN - 1, file);
	buffer[BUFFER_LEN - 1] = 0;
}