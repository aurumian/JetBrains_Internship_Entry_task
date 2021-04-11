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

	

private:
	
	void OnInputFieldUpdated(wxCommandEvent& event);

	void OnHelperThreadFoundMatch(wxThreadEvent& event);
	void OnHelperThreadDone(wxThreadEvent& event);

	void ShowSearchIsInProgress();
	void HideSearchIsInProgress();

	wxTextCtrl* inField;
	wxRichTextCtrl* outField;

	wxMessageQueue<HelperThread::MsgToHelperThread> msgq;
	wxCriticalSection helperThreadCS;
	HelperThread* helperThread;

	friend class HelperThread;
};

