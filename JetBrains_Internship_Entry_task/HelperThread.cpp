#include <wx/event.h>

#include "HelperThread.h"
#include "MainFrame.h"
#include "DictionaryWordChecker.h"

wxDEFINE_EVENT(wxEVT_HELPER_THREAD_FOUND_MATCH, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_HELPER_THREAD_DONE, wxThreadEvent);

namespace {
	const wxString DICTIONARY_FILEPATH = wxT("Resource\\words.txt");
	const wxString DICTIONARY_SUFFIXARRAY_FILEPATH = wxT("Resource\\words.suffixarray");
	const int MAX_NUM_RESULTS = 10000;
	const int MAX_NUM_RESULTS_PER_MESSAGE = 10;
}


HelperThread::HelperThread(MainFrame* mainFrame) {
	this->mainFrame = mainFrame;
	dictChecker = new DictionaryWordChecker(DICTIONARY_FILEPATH, DICTIONARY_SUFFIXARRAY_FILEPATH);
	working = false;
}

wxThread::ExitCode HelperThread::Entry() {

	while (!TestDestroy()) {
		// go through all the messages in the queue and choose the most recent one
		auto& msgq = mainFrame->msgq;
		MsgToHelperThread msg;
		bool exit = false;
		while (!exit && msgq.ReceiveTimeout(0, msg) != wxMSGQUEUE_TIMEOUT) {
			switch (msg.type) {
			case MsgToHelperThread::MessageType::SEARCH:
				dictChecker->Reset();
				dictChecker->SetWordToCheck(msg.searchStr);
				searchStr = msg.searchStr;
				working = true;
				matchedCount = 0;
				totalMatchedCount = 0;
				results.Clear();
				break;
			case MsgToHelperThread::MessageType::EXIT:
				exit = true;
				break;
			}	
		}
		if (exit)
			break;

		if (!working)
			continue;	

		// if the dictionary word fits add it to results
		bool wordAdded = dictChecker->CheckNext(results);
		if (totalMatchedCount < MAX_NUM_RESULTS && wordAdded)
		{
			matchedCount++;
			totalMatchedCount++;
			if (matchedCount < MAX_NUM_RESULTS_PER_MESSAGE)
				results.AppendNormal(RESULTS_DELIMITER);
		}

		bool done = dictChecker->CheckedAll() || totalMatchedCount == MAX_NUM_RESULTS;
		
		if (matchedCount >= MAX_NUM_RESULTS_PER_MESSAGE || done && matchedCount > 0) 
		{
			wxThreadEvent evt(wxEVT_HELPER_THREAD_FOUND_MATCH);
			evt.SetString(results.GetString());
			wxQueueEvent(mainFrame, evt.Clone());
			matchedCount = 0;
			results.Clear();
		}

		// if done, notify the main thread
		if (done)
		{
			wxThreadEvent evt(wxEVT_HELPER_THREAD_DONE);
			// if we stopped because of the limit we should propagte the info to MainFrame
			evt.SetInt(dictChecker->CheckedAll());
			wxQueueEvent(mainFrame, evt.Clone());
			working = false;
		}

		wxMilliSleep(100);

	}

	return (wxThread::ExitCode)0;
}

HelperThread::~HelperThread()
{
	wxCriticalSectionLocker lock(mainFrame->helperThreadCS);
	mainFrame->helperThread = nullptr;
}