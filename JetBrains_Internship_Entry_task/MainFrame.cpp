#include <wx/richtext/richtextctrl.h>

#include "MainFrame.h"
#include "PartiallyBoldString.h"

// TODO: add pausing/resuming of helper thread
// TODO: send exit message to helper thread onClose

namespace {
	const unsigned long MAX_INPUT_LEN = 30;
	const int IN_ID = 1;
	const wxString IN_SEARCH_PROGRESS_STR = wxT(" ...");
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
		wxDefaultSize, wxVSCROLL);
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

		Connect(wxEVT_HELPER_THREAD_FOUND_MATCH,
			wxThreadEventHandler(MainFrame::OnHelperThreadFoundMatch));
		Connect(wxEVT_HELPER_THREAD_DONE,
			wxThreadEventHandler(MainFrame::OnHelperThreadDone));
	}

	// create and start the helper thread
	helperThread = new HelperThread(this);
	helperThread->Run();

}

void MainFrame::OnInputFieldUpdated(wxCommandEvent& event) 
{
	// send a message to the helper thread to start searching
	HelperThread::MsgToHelperThread msg;
	msg.searchStr = inField->GetValue();
	msg.type = HelperThread::MsgToHelperThread::MessageType::SEARCH;
	msgq.Post(msg);

	// clear the outField and show that the search is in progress
	outField->Clear();
	ShowSearchIsInProgress();
}

void MainFrame::OnHelperThreadFoundMatch(wxThreadEvent& event) 
{
	HideSearchIsInProgress();

	wxString str = event.GetString();
	/*PartiallyBoldString bs(str);
	while (bs.CanGetNextPart())
	{
		bool isBold = bs.IsNextPartBold();
		if (isBold) 
			outField->BeginBold();

		wxString s;
		bs.GetNextPart(s);
		outField->WriteText(s);

		if (isBold)
			outField->EndBold();

	}*/
	outField->WriteText(str);
	outField->WriteText(RESULTS_DELIMITER);

	ShowSearchIsInProgress();
}

void MainFrame::OnHelperThreadDone(wxThreadEvent& event)
{
	if (event.GetInt()) {
		HideSearchIsInProgress();
		RemoveLastNCharsFromOut(RESULTS_DELIMITER.size());
	}
}

void MainFrame::RemoveLastNCharsFromOut(size_t n) 
{
	size_t len = outField->GetLineLength(0);
	outField->Remove(len < n ? 0 : len - n, len);
}

void MainFrame::ShowSearchIsInProgress()
{
	outField->WriteText(IN_SEARCH_PROGRESS_STR);
}

void MainFrame::HideSearchIsInProgress()
{
	RemoveLastNCharsFromOut(IN_SEARCH_PROGRESS_STR.length());
}