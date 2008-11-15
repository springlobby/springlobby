#ifndef SPRINGLOBBY_CHANNELCHOOSERDIALOG_H_INCLUDED
#define SPRINGLOBBY_CHANNELCHOOSERDIALOG_H_INCLUDED

#include <wx/dialog.h>

class ChannelChooserPanel;
class wxBoxSizer;

class ChannelChooserDialog : public wxDialog
{
    public:
        ChannelChooserDialog(wxWindow* parent, wxWindowID id, const wxString& title,
            long style = wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxDEFAULT_DIALOG_STYLE );
        virtual ~ChannelChooserDialog();

        void AddChannel( const wxString& name, int usercount, const wxString& topic = wxEmptyString );
        void ClearChannels();

    protected:
        ChannelChooserPanel* m_chooser_panel;
        wxBoxSizer* m_main_sizer;

};

#endif // SPRINGLOBBY_CHANNELCHOOSERDIALOG_H_INCLUDED
