
#include "PartiallyBoldString.h"

namespace {
	const wxChar boldMarker = '*';
}

PartiallyBoldString::PartiallyBoldString()
{
	string = wxEmptyString;
}

PartiallyBoldString::PartiallyBoldString(const wxString& str)
{
	string = str;
}

void PartiallyBoldString::AppendNormal(const wxString& str)
{
	string.Append(str);
}

void PartiallyBoldString::AppendBold(const wxString& str)
{
	string.Append(boldMarker);
	string.Append(str);
	string.Append(boldMarker);
}

void PartiallyBoldString::Clear()
{
	string.Clear();
}

void PartiallyBoldString::ResetOffset()
{
	offset = 0;
}

bool PartiallyBoldString::CanGetNextPart() {
	return offset < string.Length();
}

bool PartiallyBoldString::IsNextPartBold()
{
	return CanGetNextPart() && IsNexPartBoldPrivate();
}

bool PartiallyBoldString::GetNextPart(wxString& outStr)
{
	if (!CanGetNextPart())
		return false;

	if (IsNextPartBold()) {
		size_t end = string.find(boldMarker, offset + 1);
		outStr = string.SubString(offset + 1, end - 1);
		offset = end + 1;
	}
	else {
		size_t end = string.find(boldMarker, offset);
		outStr = string.SubString(offset, end - 1);
		offset = end;
	}

	return true;
}

bool PartiallyBoldString::IsNexPartBoldPrivate()
{
	return string.GetChar(offset) == boldMarker;
}


wxString PartiallyBoldString::GetString()
{
	return string;
}

bool PartiallyBoldString::IsEmpty() {
	return string.empty();
}