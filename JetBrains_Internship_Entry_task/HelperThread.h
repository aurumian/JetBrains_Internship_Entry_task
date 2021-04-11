#pragma once

#include <wx/thread.h>
#include <wx/string.h>

#include "PartiallyBoldString.h"

// declare new types of events to send to main thread
wxDECLARE_EVENT(wxEVT_HELPER_THREAD_FOUND_MATCH, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_HELPER_THREAD_DONE, wxThreadEvent);
// TODO: use this event to display erro messages
//wxDECLARE_EVENT(wxEVT_HELPER_THREAD_ERROR, wxThreadEvent);

class MainFrame;
class DictionaryWordChecker;

class HelperThread : public wxThread
{
public:
	HelperThread(MainFrame* mainFrame);
	~HelperThread();
protected:
	virtual wxThread::ExitCode Entry() override;

private:
	MainFrame* mainFrame;

	DictionaryWordChecker* dictChecker;
	bool working;
	size_t matchedCount = 0;
	size_t totalMatchedCount = 0;
	wxString searchStr;
	PartiallyBoldString results;

public:
	struct MsgToHelperThread {
		enum class MessageType {
			SEARCH,
			EXIT
		};

		MessageType type;

		wxString searchStr;
	};
};

