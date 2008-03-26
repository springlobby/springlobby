#ifndef SPRINGLOBBY_REPLAYLISTCTRL_H_INCLUDED
#define SPRINGLOBBY_REPLAYLISTCTRL_H_INCLUDED

//#include "replaylistfilter.h"
#include "replaylist.h"

#include "customlistctrl.h"

class wxMenu;
class Replay;
class wxListEvent;
class wxCommandEvent;
class Ui;
class ReplayList_Iter;


class ReplayListCtrl : public customListCtrl
{
  public:
    ReplayListCtrl( wxWindow* parent, Ui& ui, ReplayList& replaylist );
    ~ReplayListCtrl();


    int GetSelectedIndex();
    void Sort();

    void OnSelected( wxListEvent& event );
    void OnDeselected( wxListEvent& event );
    void OnListRightClick( wxListEvent& event );
    void OnDLMap( wxCommandEvent& event );
    void OnDLMod( wxCommandEvent& event );
    void OnMouseMotion(wxMouseEvent& event);
    void OnColClick( wxListEvent& event );

  protected:
    static int wxCALLBACK CompareDateUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareDateDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareSpringUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareSpringDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareMapUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareMapDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareModUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareModDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK ComparePlayerUP(long item1, long item2, long sortData);
    static int wxCALLBACK ComparePlayerDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareFileUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareFileDOWN(long item1, long item2, long sortData);

    struct {
      int col;
      bool direction;
    } m_sortorder[4];

    int m_selected;
    wxMenu* m_popup;
    Ui& m_ui;
   // ReplayList_Iter& m_replaylist_iter;
    ReplayList& m_replaylist;
    static ReplayList* m_replaylist_sort;

    DECLARE_EVENT_TABLE()
};

enum
{
    RLIST_LIST = wxID_HIGHEST,
    RLIST_DLMOD,
    RLIST_DLMAP
};

#endif // SPRINGLOBBY_REPLAYLISTCTRL_H_INCLUDED
