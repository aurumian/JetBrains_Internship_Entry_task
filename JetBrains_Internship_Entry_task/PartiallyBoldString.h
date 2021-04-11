#pragma once

#include <wx/string.h>

class PartiallyBoldString
{
public:
	PartiallyBoldString();
	PartiallyBoldString(const wxString& str);

	void AppendNormal(const wxString& str);

	void AppendBold(const wxString& str);

	void Clear();

	void ResetOffset();

	bool IsNextPartBold();

	bool CanGetNextPart();
	bool GetNextPart(wxString& outStr);

	wxString GetString();

private:

	bool IsNexPartBoldPrivate();

	wxString string;
	size_t offset = 0;

};

