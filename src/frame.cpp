#include "frame.h"
#include "tabs.h"

BEGIN_EVENT_TABLE(settings_frame,wxFrame)
	EVT_CLOSE(settings_frame::OnClose)
END_EVENT_TABLE()

settings_frame::settings_frame(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style)
{
	CreateGUIControls();
}

settings_frame::~settings_frame()
{
}

void settings_frame::CreateGUIControls()
{

	Options = new wxNotebook(this, ID_OPTIONS, wxPoint(0,0),TAB_SIZE, wxNB_BOTTOM);
	Options->SetFont(wxFont(8, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Tahoma")));

//	Simple = new wxPanel(Options, ID_SIMPLE, wxPoint(4,24), wxSize(724,552));
//	Simple->SetFont(wxFont(8, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Tahoma")));
//	Simple->Add(new wxString(wxT("nothing here yet")));
//	Options->AddPage(Simple, wxT("Simple"));

	Options->AddPage(new rendering_panel(Options,ID_RENDERING), wxT("Rendering"));

	Options->AddPage(new video_panel(Options,ID_VIDEO), wxT("Video"));

	Options->AddPage(new general_panel(Options, ID_GENERAL), wxT("General"));

	Options->AddPage(new audio_panel(Options,ID_AUDIO), wxT("Audio"));

	Options->AddPage(new mouse_panel(Options, ID_MOUSE), wxT("Mouse"));

	Options->AddPage(new debug_panel(Options,ID_DEBUG), wxT("Debug"));

	SetTitle(wxT("Settings++"));
	SetIcon(wxNullIcon);
	SetSize(8,8,520,560);
	Center();

}

void settings_frame::OnClose(wxCloseEvent& event)
{
	Destroy();
}
