#ifndef CHANNELCHOOSERPANEL_H
#define CHANNELCHOOSERPANEL_H

#include <wx/scrolwin.h>
#include <vector>

class ChannelListctrl;
class wxBoxSizer;
class wxButton;

class ChannelChooserPanel : public wxScrolledWindow
{
    public:
        ChannelChooserPanel(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxHSCROLL | wxVSCROLL ,
            const wxString& name = _T("dialogBox") );
        virtual ~ChannelChooserPanel();

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
