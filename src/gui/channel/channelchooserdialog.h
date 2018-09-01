/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_CHANNELCHOOSERDIALOG_H_INCLUDED
#define SPRINGLOBBY_CHANNELCHOOSERDIALOG_H_INCLUDED

#include <wx/dialog.h>
#include "gui/windowattributespickle.h"
class ChannelChooserPanel;
class wxBoxSizer;

class ChannelChooserDialog : public wxDialog, public WindowAttributesPickle
{
public:
	ChannelChooserDialog(wxWindow* parent, wxWindowID id, const wxString& title,
			     long style = wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxDEFAULT_DIALOG_STYLE);
	virtual ~ChannelChooserDialog();

	void AddChannel(const wxString& name, int usercount, const wxString& topic = wxEmptyString);
	void ClearChannels();

private:
	ChannelChooserPanel* m_chooser_panel;
	wxBoxSizer* m_main_sizer;
};

#endif // SPRINGLOBBY_CHANNELCHOOSERDIALOG_H_INCLUDED
