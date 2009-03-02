#ifndef SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H

#include "battlelistfilter.h"
#include "battlelist.h"

#include "customvirtlistctrl.h"

class wxMenu;
class IBattle;
class wxListEvent;
class wxCommandEvent;
class Ui;

/** \brief The ListCtrll contained in BattleListTab dispalying all currently active battles with their infos
 * \todo DOCMEMORE */
class BattleListCtrl : public CustomVirtListCtrl< IBattle *>
{
  public:
    BattleListCtrl( wxWindow* parent, Ui& ui );
    ~BattleListCtrl();

    void AddBattle( IBattle& battle );
    void RemoveBattle( IBattle& battle );
    void UpdateBattle( IBattle& battle );


    void OnListRightClick( wxListEvent& event );
    void OnDLMap( wxCommandEvent& event );
    void OnDLMod( wxCommandEvent& event );
    void OnColClick( wxListEvent& event );
    virtual void SetTipWindowText( const long item_hit, const wxPoint position);

    //these are overloaded to use list in virtual style
    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemImage(long item) const;
    virtual int OnGetItemColumnImage(long item, long column) const;
    wxListItemAttr * OnGetItemAttr(long item) const;

    enum {
        BLIST_LIST = wxID_HIGHEST,
        BLIST_DLMOD,
        BLIST_DLMAP
    };


  protected:
    static int wxCALLBACK CompareStatusDOWN(long item1, long item2, long sortData);

    static int CompareStatus( DataType u1, DataType u2 );
    static int ComparePlayer( DataType u1, DataType u2 );

    static int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) ;
    int GetIndexFromData( const DataType& data ) const;

    wxMenu* m_popup;
    Ui& m_ui;

    virtual void Sort();

    DECLARE_EVENT_TABLE()
};



#endif // SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H
