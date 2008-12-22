#ifndef SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H

#include "battlelistfilter.h"
#include "battlelist.h"

#include "customvirtlistctrl.h"

class wxMenu;
class Battle;
class wxListEvent;
class wxCommandEvent;
class Ui;

/** \brief The ListCtrll contained in BattleListTab dispalying all currently active battles with their infos
 * \todo DOCMEMORE */
class BattleListCtrl : public CustomVirtListCtrl<const Battle *>
{
  public:
    BattleListCtrl( wxWindow* parent, Ui& ui );
    ~BattleListCtrl();

    void AddBattle( const Battle& battle );
    void RemoveBattle( const Battle& battle );
    void UpdateBattle( const Battle& battle );


    void OnListRightClick( wxListEvent& event );
    void OnDLMap( wxCommandEvent& event );
    void OnDLMod( wxCommandEvent& event );
    void OnColClick( wxListEvent& event );
    virtual void SetTipWindowText( const long item_hit, const wxPoint position);
    void HighlightItem( long item );
    //void HighlightItem( long item, const wxString& name );

    //these are overloaded to use list in virtual style
    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemImage(long item) const;
    virtual int OnGetItemColumnImage(long item, long column) const;

    enum {
        BLIST_LIST = wxID_HIGHEST,
        BLIST_DLMOD,
        BLIST_DLMAP
    };


  protected:
    static int wxCALLBACK CompareStatusDOWN(long item1, long item2, long sortData);

    static int CompareStatus( DataType u1, DataType u2 );
    static int ComparePlayer( DataType u1, DataType u2 );
    static int wxCALLBACK CompareMaxPlayerUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareMaxPlayerDOWN(long item1, long item2, long sortData);

    static int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) ;
    int GetIndexFromData( const DataType& data ) const;

    wxMenu* m_popup;
    Ui& m_ui;
    static Ui* m_ui_for_sort;

    virtual void Sort();

    DECLARE_EVENT_TABLE()
};



#endif // SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H
