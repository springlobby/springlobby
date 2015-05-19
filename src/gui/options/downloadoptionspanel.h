/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADER_GUARD_TORRENTOPTIONSPANEL_H_INCLUDED
#define SPRINGLOBBY_HEADER_GUARD_TORRENTOPTIONSPANEL_H_INCLUDED

#include <wx/scrolwin.h>

class wxCheckBox;
class wxTextCtrl;
class Ui;
class wxStaticBox;
class wxRadioButton;
class wxButton;
class wxStaticText;
class wxSpinCtrl;
class wxStaticBoxSizer;
class TorrentOptionsPanel : public wxScrolledWindow
{
public:
	TorrentOptionsPanel(wxWindow* parent);
	~TorrentOptionsPanel();

	void OnApply(wxCommandEvent& event);
	void OnRestore(wxCommandEvent& event);

private:
	wxBoxSizer* m_main_sizer;
	wxSpinCtrl* m_parallel_http;
	wxStaticBoxSizer* m_parallel_http_sizer;
	void EnableSettings(bool enable);
	void SetStatusDisplay();

	enum {
		ID_ENABLEP2P = wxID_HIGHEST,
		ID_MAXUP,
		ID_MAXDOWN,
		ID_P2PPORT,
		ID_MAXCONNECTIONS,
		ID_APPLY,
		ID_RESTORE,
		ID_GAMESTART_RADIO,
		ID_INGAME_UP,
		ID_INGAME_DOWN
	};

	DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADER_GUARD_TORRENTOPTIONSPANEL_H_INCLUDED
