#ifndef CHANNELCHOOSER_H
#define CHANNELCHOOSER_H

#include <wx/dialog.h>
#include <vector>

class ChannelListctrl;
class wxBoxSizer;
class wxButton;

class ChannelChooser : public wxDialog
{
    public:
        ChannelChooser(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = _T("dialogBox") );
        virtual ~ChannelChooser();

        void AddChannel( const wxString& name, int usercount, const wxString& topic = wxEmptyString );
        void ClearChannels();

    protected:
        wxButton* m_join_channels;
        wxButton* m_mark_autojoin;
        wxBoxSizer* m_main_sizer;
        ChannelListctrl* m_channellist;
    private:
};

#endif // CHANNELCHOOSER_H
