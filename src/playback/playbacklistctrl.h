#ifndef SPRINGLOBBY_PlaybackListCtrl_H_INCLUDED
#define SPRINGLOBBY_PlaybackListCtrl_H_INCLUDED

#include "../customvirtlistctrl.h"

class wxMenu;
class wxListEvent;
class wxCommandEvent;

template <class PlaybackImp>
class PlaybackListCtrl : public CustomVirtListCtrl< const PlaybackImp*, PlaybackListCtrl<PlaybackImp> >
{
    protected:
        typedef PlaybackListCtrl<PlaybackImp>
            ThisType;
        typedef CustomVirtListCtrl< const PlaybackImp*,PlaybackListCtrl<PlaybackImp> >
            ParentType;
        typedef typename ParentType::DataType
            DataType;
        typedef PlaybackImp
            PlaybackType;

    using ParentType::AddColumn;
    using ParentType::m_sortorder;
    using ParentType::PopupMenu;
    using ParentType::m_data;
    using ParentType::m_selected_data;
    using ParentType::m_selected_index;
    using ParentType::SaveSelection;
    using ParentType::RestoreSelection;
    using ParentType::HitTest;
    using ParentType::m_comparator;
    using ParentType::m_tiptimer;
    using ParentType::m_tiptext;
    using ParentType::m_colinfovec;
    using ParentType::m_tooltip_delay;
    using ParentType::m_tooltip_duration;
	using ParentType::RemoveItem;
	using ParentType::AddItem;
	using ParentType::SetColumnWidth;
	using ParentType::compareSimple;
	using ParentType::SetItemCount;
//    using ParentType::DataVector;
    typedef typename ParentType::DataVector::const_iterator
        DataCIter; //! TODO (koshi) i'd be mighty thankful if some could explain to me why the import with using ParentType::DataCIter doesn't work here;
    using ParentType::getColumnFromPosition;

  public:
    PlaybackListCtrl( wxWindow* parent );
    ~PlaybackListCtrl();

    void AddPlayback( const PlaybackType& replay );
    void RemovePlayback( const PlaybackType& replay );
    void RemovePlayback( const int index );
    void OnListRightClick( wxListEvent& event );
    void OnDLMap( wxCommandEvent& event );
    void OnDLMod( wxCommandEvent& event );
    void OnColClick( wxListEvent& event );

    virtual void SetTipWindowText( const long item_hit, const wxPoint& position);

	virtual void HighlightItem( long /*unused*/ ){}

    //these are overloaded to use list in virtual style
    wxString GetItemText(long item, long column) const;
    int GetItemImage(long item) const;
    int GetItemColumnImage(long item, long column) const;
    wxListItemAttr * GetItemAttr(long item) const;
    int GetIndexFromData( const DataType& data ) const;

    using ParentType::RefreshVisibleItems;
    using ParentType::GetDataFromIndex;

  protected:
	int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) const;
	void OnChar(wxKeyEvent & event);

    virtual void Sort();

    wxMenu* m_popup;

    DECLARE_EVENT_TABLE()
};

enum
{
    RLIST_LIST,
    RLIST_DLMOD,
    RLIST_DLMAP
};

#include "playbacklistctrl.cpp"
#endif // SPRINGLOBBY_PlaybackListCtrl_H_INCLUDED

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

