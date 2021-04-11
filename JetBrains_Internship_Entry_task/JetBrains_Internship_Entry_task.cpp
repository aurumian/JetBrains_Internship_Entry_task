#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/stattext.h>

#include "JetBrains_Internship_Entry_Task.h"
#include "MainFrame.h"

class Simple : public wxFrame
{
public:
	Simple(const wxString& title);
};

class Button : public wxFrame
{
public:
	Button(const wxString& title);

	void onQuit(wxCommandEvent& event);
};


class LeftPanel : public wxPanel
{
public:
	LeftPanel(wxPanel* parent);

	void OnPlus(wxCommandEvent& event);
	void OnMinus(wxCommandEvent& event);

	wxButton* m_plus;
	wxButton* m_minus;
	wxPanel* m_parent;
	int count;
};

class RightPanel : public wxPanel
{
public:
	RightPanel(wxPanel* parent);
	void OnSetText(wxCommandEvent& event);

	wxStaticText* m_text;
};

class Communicate : public wxFrame
{
public:
	Communicate(const wxString& title);

	LeftPanel* m_lp;
	RightPanel* m_rp;
	wxPanel* m_parent;
};

const int ID_PLUS = 101;
const int ID_MINUS = 102;

LeftPanel::LeftPanel(wxPanel* parent) :
	wxPanel(parent, -1, wxPoint(-1, -1), wxSize(-1, -1), wxBORDER_SUNKEN)
{
	count = 0;
	m_parent = parent;
	m_plus = new wxButton(this, ID_PLUS, wxT("+"), wxPoint(10, 10));
	m_minus = new wxButton(this, ID_MINUS, wxT("-"), wxPoint(10, 60));
	Connect(ID_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(LeftPanel::OnPlus));
	Connect(ID_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(LeftPanel::OnMinus));
}

void LeftPanel::OnPlus(wxCommandEvent& WXUNUSED(event))
{
	count++;

	Communicate* comm = (Communicate*)m_parent->GetParent();
	comm->m_rp->m_text->SetLabel(wxString::Format(wxT("%d"), count));
}

void LeftPanel::OnMinus(wxCommandEvent& WXUNUSED(event))
{
	count--;

	Communicate* comm = (Communicate*)m_parent->GetParent();
	comm->m_rp->m_text->SetLabel(wxString::Format(wxT("%d"), count));
}

RightPanel::RightPanel(wxPanel* parent) :
	wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(270, 150), wxBORDER_SUNKEN)
{
	m_text = new wxStaticText(this, -1, wxT("0"), wxPoint(40, 60));
}

Communicate::Communicate(const wxString& title) :
	wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(290, 150))
{
	m_parent = new wxPanel(this, wxID_ANY);
	
	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

	m_lp = new LeftPanel(m_parent);
	m_rp = new RightPanel(m_parent);

	hbox->Add(m_lp, 1, wxEXPAND | wxALL, 5);
	hbox->Add(m_rp, 1, wxEXPAND | wxALL, 5);

	m_parent->SetSizer(hbox);

	SetMinSize(wxSize(250, 150));

	this->Center();
}

bool MyApp::OnInit() 
{
	/*Simple* simple = new Simple(wxT("Simple"));
	simple->Show(true);*/

	/*Button* btnapp = new Button(wxT("Button"));
	btnapp->Show(true);*/

	/*Communicate* comm = new Communicate(wxT("Widgets can communicate"));
	comm->Show(true);*/

	MainFrame* mf = new MainFrame();
	mf->Show(true);


	return true;
}

Simple::Simple(const wxString& title) :
	wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(250, 150))
{
	Centre();
}

Button::Button(const wxString& title) :
	wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(270, 150))
{
	wxPanel* panel = new wxPanel(this, wxID_ANY);
	wxButton* button = new wxButton(panel, wxID_EXIT, wxT("Quit"), wxPoint(20, 20));
	Connect(wxID_EXIT, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(Button::onQuit));
	button->SetFocus();
	Centre();
}

void Button::onQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}