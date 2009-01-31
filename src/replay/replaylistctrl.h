#ifndef SPRINGLOBBY_REPLAYLISTCTRL_H_INCLUDED
#define SPRINGLOBBY_REPLAYLISTCTRL_H_INCLUDED

#include "../customlistctrl.h"

class wxMenu;
struct Replay;
class wxListEvent;
class wxCommandEvent;
class Ui;
class ReplayList_Iter;
class ReplayList;


class ReplayListCtrl : public CustomListCtrl
{
  public:
    ReplayListCtrl( wxWindow* parent, ReplayList& replaylist );
    ~ReplayListCtrl();

    void Sort();
    void SetUnsorted();

    void OnListRightClick( wxListEvent& event );
    void OnDLMap( wxCommandEvent& event );
    void OnDLMod( wxCommandEvent& event );
    void OnMouseMotion(wxMouseEvent& event);
    void OnColClick( wxListEvent& event );
    virtual void HighlightItem( long item ){};

  protected:
    static int wxCALLBACK CompareUniversal(long item1, long item2, long sortData);


    wxMenu* m_popup;
   // ReplayList_Iter& m_replaylist_iter;
    ReplayList& m_replaylist;
    static ReplayList* m_replaylist_sort;

    DECLARE_EVENT_TABLE()
};

enum
{
    RLIST_LIST,
    RLIST_DLMOD,
    RLIST_DLMAP
};

#endif // SPRINGLOBBY_REPLAYLISTCTRL_H_INCLUDED
