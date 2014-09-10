/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H

#include "battlelistfilter.h"
#include "battlelist.h"

#include "gui/customvirtlistctrl.h"

class wxMenu;
class IBattle;
class wxListEvent;
class wxCommandEvent;
class Ui;

/** \brief The ListCtrll contained in BattleListTab dispalying all currently active battles with their infos
 * \todo DOCMEMORE */
class BattleListCtrl : public CustomVirtListCtrl< IBattle *, BattleListCtrl>
{
  public:
    BattleListCtrl( wxWindow* parent );
    ~BattleListCtrl();

    void AddBattle( IBattle& battle );
    void RemoveBattle( IBattle& battle );
    void UpdateBattle( IBattle& battle );


    void OnListRightClick( wxListEvent& event );
    void OnDLMap( wxCommandEvent& event );
    void OnDLMod( wxCommandEvent& event );
    virtual void SetTipWindowText( const long item_hit, const wxPoint& position);

    //these are overloaded to use list in virtual style
    wxString GetItemText(long item, long column) const;
    int GetItemImage(long item) const;
    int GetItemColumnImage(long item, long column) const;
    wxListItemAttr * GetItemAttr(long item) const;

    enum {
        BLIST_LIST = wxID_HIGHEST,
        BLIST_DLMOD,
        BLIST_DLMAP
    };


private:
    static int CompareStatus( DataType u1, DataType u2 );
    static int ComparePlayer( DataType u1, DataType u2 );

	int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) const;
    int GetIndexFromData( const DataType& data ) const;

    wxMenu* m_popup;

    virtual void Sort();

    DECLARE_EVENT_TABLE()
};



#endif // SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H
