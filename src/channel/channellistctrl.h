#ifndef SPRINGLOBBY_HEADERGUARD_CHANNELLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_CHANNELLISTCTRL_H

#include "../customvirtlistctrl.h"


class ChannelListctrl : public CustomVirtListCtrl
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

            //these are overloaded to use list in virtual style
        virtual wxString OnGetItemText(long item, long column) const;
        virtual int OnGetItemImage(long item) const;
        virtual int OnGetItemColumnImage(long item, long column) const;

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

    protected:
        void Sort();
        void SetTipWindowText( const long item_hit, const wxPoint position);

        void OnColClick( wxListEvent& event );
        void OnActivateItem( wxListEvent& event );

        void HighlightItem( long item );

        enum {
          CHANNELLIST = wxID_HIGHEST

        };

        typedef ChannelInfo DataType;
        typedef std::vector< DataType > ChannelInfoMap;
        typedef ChannelInfoMap::iterator ChannelInfoIter;
        typedef ChannelInfoMap::const_iterator ChannelInfoCIter;
        ChannelInfoMap m_data;

        int GetIndexFromData( const DataType& data );

        //! the Comparator object passed to the SLInsertionSort function
        ItemComparator<DataType> m_comparator;

        //! passed as callback to generic ItemComparator, returns -1,0,1 as per defined ordering
        static int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) ;

        wxString m_last_filter_value;

        DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_CHANNELLISTCTRL_H
