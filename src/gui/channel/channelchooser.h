/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef CHANNELCHOOSERPANEL_H
#define CHANNELCHOOSERPANEL_H

#include <wx/scrolwin.h>
#include "utils/mixins.h"
class ChannelListView;
class wxBoxSizer;
class wxButton;
class wxTextCtrl;
class wxStaticText;

class ChannelChooserPanel : public wxScrolledWindow, public SL::NonCopyable
{
public:
	ChannelChooserPanel(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition,
			    const wxSize& size = wxDefaultSize,
			    long style = wxHSCROLL | wxVSCROLL,
			    const wxString& name = _T("dialogBox"));
	virtual ~ChannelChooserPanel();

	void AddChannel(const wxString& name, int usercount, const wxString& topic = wxEmptyString);
	void ClearChannels();
	void OnSearch(wxCommandEvent& event);

private:
	//        wxButton* m_join_channels;
	//        wxButton* m_mark_autojoin;
	wxBoxSizer* m_main_sizer;
	ChannelListView* m_channellist;
	wxTextCtrl* m_search_text;
	wxStaticText* m_info_label;

	enum {
		ID_SEARCH_TEXT
	};

	DECLARE_EVENT_TABLE()
};

#endif // CHANNELCHOOSER_H
