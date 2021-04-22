#include <wx/richtext/richtextctrl.h>

#include "MainFrame.h"
#include "PartiallyBoldString.h"
#include "wx/time.h"

// TODO: add pausing/resuming of helper thread

namespace {
	const unsigned long MAX_INPUT_LEN = 30;
	const int IN_ID = 1;
	const wxString IN_SEARCH_PROGRESS_STR = wxT(" ...");
	const int UPDATE_INTERVAL_MILLIS = 50;
}

MainFrame::MainFrame() :
	wxFrame(NULL, wxID_ANY, wxT("JetBrains Internship Entry Task"))
{
	SetMinSize(wxSize(300, 300));
	Center();

	wxPanel* panel = new wxPanel(this, wxID_ANY);

	inField = new wxTextCtrl(panel, IN_ID);
	inField->SetMaxLength(MAX_INPUT_LEN);
	inField->SetHint(wxT("¬ведите поисковой запрос сюда"));
	
	outField = new wxRichTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxVSCROLL | wxTE_READONLY);
	outField->SetEditable(false);
	outField->GetCaret()->Hide();

	// define layout
	{
		wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
		panel->SetSizer(vbox);
		vbox->Add(inField, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
		//vbox->Add(cb, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
		vbox->Add(outField, 2, wxEXPAND | wxALL, 10);
	}

	// connect events
	{
		Connect(IN_ID, wxEVT_COMMAND_TEXT_UPDATED,
			wxCommandEventHandler(MainFrame::OnInputFieldUpdated));

		Connect(wxEVT_HELPER_THREAD_DONE,
			wxThreadEventHandler(MainFrame::OnHelperThreadDone));

		Connect(wxEVT_CLOSE_WINDOW,
			wxCloseEventHandler(MainFrame::OnClose));
	}

	// create and start the helper thread
	helperThread = new HelperThread(this);
	helperThread->Run();

	// start the timer
	timer.SetOwner(this);
	timer.Start(UPDATE_INTERVAL_MILLIS);
	Connect(wxEVT_TIMER,
		wxTimerEventHandler(MainFrame::OnTimer));
}

void MainFrame::OnInputFieldUpdated(wxCommandEvent& event) 
{
	helperThreadDone = false;
	// send a message to the helper thread to start searching
	HelperThread::MsgToHelperThread msg;
	msg.searchStr = inField->GetValue();
	msg.type = HelperThread::MsgToHelperThread::MessageType::SEARCH;
	msgq.Post(msg);

	// clear the outField and show that the search is in progress
	outField->Clear();
	currentBs.Clear();
	ShowSearchIsInProgress();
}

void MainFrame::OnHelperThreadDone(wxThreadEvent& event)
{
	helperThreadDone = true;
}

void MainFrame::RemoveLastNCharsFromOut(size_t n) 
{
	size_t len = outField->GetLineLength(0);
	outField->Remove(len < n ? 0 : len - n, len);
}

void MainFrame::ShowSearchIsInProgress()
{
	if (!searchInProgressDisplayed)
	{
		outField->WriteText(IN_SEARCH_PROGRESS_STR);
		searchInProgressDisplayed = true;
	}
}

void MainFrame::HideSearchIsInProgress()
{
	if (searchInProgressDisplayed)
	{
		RemoveLastNCharsFromOut(IN_SEARCH_PROGRESS_STR.length());
		searchInProgressDisplayed = false;
	}
}

void MainFrame::OnTimer(wxTimerEvent& event)
{
	auto beginTime = wxGetLocalTimeMillis();
	HideSearchIsInProgress();
	bool receivedMsg = false;
	while ((wxGetLocalTimeMillis() - beginTime).GetLo() < (UPDATE_INTERVAL_MILLIS * 0.5))
	{
		bool empty = currentBs.IsEmpty();
		if (!empty && currentBs.CanGetNextPart()) 
		{
			bool isBold = currentBs.IsNextPartBold();
			if (isBold)
				outField->BeginBold();

			wxString s;
			currentBs.GetNextPart(s);
			outField->WriteText(s);

			if (isBold)
				outField->EndBold();
		}
		else if ((resultsQ.ReceiveTimeout(0, currentBs)) != wxMSGQUEUE_TIMEOUT) 
		{
			if (!empty)
				outField->WriteText(RESULTS_DELIMITER);
		}
		else if (!helperThreadDone)
		{
			ShowSearchIsInProgress();
			break;
		}
	}
	
}


void MainFrame::OnClose(wxCloseEvent& evt)
{
	timer.Stop();
	HelperThread::MsgToHelperThread msg;
	msg.type = HelperThread::MsgToHelperThread::MessageType::EXIT;
	msgq.Post(msg);

	while (true)
	{
		wxCriticalSectionLocker lock(helperThreadCS);
		if (helperThread == nullptr)
			break;
	}

	Destroy();
}