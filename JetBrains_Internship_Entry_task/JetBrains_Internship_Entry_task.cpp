#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/stattext.h>

#include "JetBrains_Internship_Entry_Task.h"
#include "MainFrame.h"

bool MyApp::OnInit() 
{
	MainFrame* mf = new MainFrame();
	mf->Show(true);

	return true;
}
