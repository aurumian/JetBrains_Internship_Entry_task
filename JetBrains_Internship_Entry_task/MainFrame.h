#pragma once

#include <wx/wx.h>
#include <wx/msgqueue.h>

#include "HelperThread.h"

class wxRichTextCtrl;
const wxString RESULTS_DELIMITER = wxT(", ");

class MainFrame : public wxFrame
{
public:
	MainFrame();

	void OnClose(wxCloseEvent& evt);

private:
	
	void OnInputFieldUpdated(wxCommandEvent& event);

	void OnHelperThreadDone(wxThreadEvent& event);

	void RemoveLastNCharsFromOut(size_t n);

	void ShowSearchIsInProgress();
	void HideSearchIsInProgress();

	void OnTimer(wxTimerEvent& event);

	wxTextCtrl* inField;
	wxRichTextCtrl* outField;

	wxMessageQueue<HelperThread::MsgToHelperThread> msgq;
	HelperThread* helperThread;
	wxCriticalSection helperThreadCS;
	wxMessageQueue<PartiallyBoldString> resultsQ;
	bool helperThreadDone = true;
	bool searchInProgressDisplayed = false;
	PartiallyBoldString currentBs;

	wxTimer timer;

	friend class HelperThread;
};