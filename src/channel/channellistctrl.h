#ifndef SPRINGLOBBY_HEADERGUARD_CHANNELLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_CHANNELLISTCTRL_H

#include "customlistctrl.h"


class ChannelListctrl : public CustomListCtrl
{
    public:
        ChannelListctrl(wxWindow* parent, wxWindowID id, const wxString& name = _T("ChannelListCtrl"),
            long style = wxSUNKEN_BORDER | wxLC_REPORT | wxLC_ALIGN_LEFT, const wxPoint& pt = wxDefaultPosition,
                    const wxSize& sz = wxDefaultSize);
        virtual ~ChannelListctrl();

        void AddChannel( const wxString& channel, unsigned int num_users, const wxString& topic);
        void ClearChannels();
        wxString GetInfo();
        void FilterChannel( const wxString& partial );

    protected:
        void Sort();
        void SetTipWindowText( const long item_hit, const wxPoint position);

        static int wxCALLBACK CompareChannelnameUP(long item1, long item2, long sortData);
        static int wxCALLBACK CompareChannelnameDOWN(long item1, long item2, long sortData);
        static int wxCALLBACK CompareNumUsersUP(long item1, long item2, long sortData);
        static int wxCALLBACK CompareNumUsersDOWN(long item1, long item2, long sortData);

        void OnColClick( wxListEvent& event );
        void OnActivateItem( wxListEvent& event );

        void HighlightItem( long item );

        enum {
          CHANNELLIST = wxID_HIGHEST

        };

        struct ChannelInfo
        {
            ChannelInfo()
                : name(_T("")), usercount(0),topic(_T("")) {}
            ChannelInfo( const wxString& name_, int usercount_, const wxString& topic_ = wxEmptyString )
                : name(name_), usercount(usercount_),topic(topic_) {}

            wxString name;
            int usercount;
            wxString topic;
        };

        typedef std::map<long, ChannelInfo > ChannelInfoMap;
        typedef ChannelInfoMap::const_iterator ChannelInfoIter;
        ChannelInfoMap m_data;

        struct {
          int col;
          bool direction;
        } m_sortorder[3];


        DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_CHANNELLISTCTRL_H
