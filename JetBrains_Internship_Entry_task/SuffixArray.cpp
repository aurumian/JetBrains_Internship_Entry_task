#include "SuffixArray.h"
#include "wx/string.h"

SuffixArray::offset_t SuffixArray::GetNumSuffixes()
{
	return numOffsets;
}

SuffixArray* SuffixArray::GetSuffixArray(const wxString& filepath, bool loadFileIntoMemory)
{
	if (loadFileIntoMemory)
		return new InMemorySuffixArray(filepath);
	else
		return new StreamingSuffixArray(filepath);
}

SuffixArray::~SuffixArray() 
{

}


InMemorySuffixArray::InMemorySuffixArray(const wxString& filename)
{
	FILE* in;
	fopen_s(&in, filename, "rb");
	fread(&numOffsets, sizeof(numOffsets), 1, in);

	fread(&offsets, sizeof(offsets[0]), numOffsets, in);

	fclose(in);
}

InMemorySuffixArray::offset_t InMemorySuffixArray::GetSuffixOffset(size_t index) const
{
	if (index > MAX_OFFSETS_ARRAY_SIZE)
		index = MAX_OFFSETS_ARRAY_SIZE - 1;
	return offsets[index];
}


StreamingSuffixArray::StreamingSuffixArray(const wxString& filename)
{
	fopen_s(&file, filename, "rb");
	fread(&numOffsets, sizeof(numOffsets), 1, file);
}

StreamingSuffixArray::~StreamingSuffixArray()
{
	fclose(file);
}

StreamingSuffixArray::offset_t StreamingSuffixArray::GetSuffixOffset(size_t index) const
{
	fseek(file, (index + 1) * sizeof(offset_t), 0);
	offset_t res;
	fread(&res, sizeof(res), 1, file);
	return res;
}