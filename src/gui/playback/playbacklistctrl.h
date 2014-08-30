/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_PlaybackListCtrl_H_INCLUDED
#define SPRINGLOBBY_PlaybackListCtrl_H_INCLUDED

#include "gui/customvirtlistctrl.h"

class wxMenu;
class wxListEvent;
class wxCommandEvent;
struct StoredGame;

class PlaybackListCtrl : public CustomVirtListCtrl< const StoredGame*, PlaybackListCtrl >
{
    protected:
  public:
    PlaybackListCtrl( wxWindow* parent );
    ~PlaybackListCtrl();

    void AddPlayback( const StoredGame& replay );
    void RemovePlayback( const StoredGame& replay );
    void RemovePlayback( const int index );
    void OnListRightClick( wxListEvent& event );
    void OnDLMap( wxCommandEvent& event );
    void OnDLMod( wxCommandEvent& event );
	void DeletePlayback(); //deletes the current selected playback from disk & list

    virtual void SetTipWindowText( const long item_hit, const wxPoint& position);

	virtual void HighlightItem( long /*unused*/ ){}

    //these are overloaded to use list in virtual style
    wxString GetItemText(long item, long column) const;
    int GetItemImage(long item) const;
    int GetItemColumnImage(long item, long column) const;
    wxListItemAttr * GetItemAttr(long item) const;
    int GetIndexFromData( const DataType& data ) const;

/*
    using ParentType::RefreshVisibleItems;
    using ParentType::GetDataFromIndex;
*/
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

#endif // SPRINGLOBBY_PlaybackListCtrl_H_INCLUDED

