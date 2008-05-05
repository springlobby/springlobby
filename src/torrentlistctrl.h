#ifndef SPRINGLOBBY_HEADERGUARD_TORRENTLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_TORRENTLISTCTRL_H

#ifndef NO_TORRENT_SYSTEM

#include "customlistctrl.h"

#include <map>
#include "torrentwrapper.h"
typedef std::map<int,TorrentInfos> map_infos;


class wxMenu;
class Battle;
class wxListEvent;
class wxCommandEvent;
class Ui;


class TorrentListCtrl : public customListCtrl
{
  public:
    TorrentListCtrl( wxWindow* parent, Ui& ui );
    ~TorrentListCtrl();


    int GetSelectedIndex();
    void Sort();

    void OnSelected( wxListEvent& event );
    void OnDeselected( wxListEvent& event );
    void OnListRightClick( wxListEvent& event );

    void OnMouseMotion(wxMouseEvent& event);
    void OnColClick( wxListEvent& event );
    void SetInfoMap( map_infos* map);

  protected:
    static int wxCALLBACK CompareNameUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareNameDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareCompletedUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareCompletedDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareDownSizeUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareDownSizeDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareUpSizeUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareUpSizeDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareDownSpeedUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareDownSpeedDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareUpSpeedUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareUpSpeedDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareLeechUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareLeechDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareEtaUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareEtaDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareCopiesUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareCopiesDOWN(long item1, long item2, long sortData);
    static map_infos* m_info_map;

    struct {
      int col;
      bool direction;
    } m_sortorder[4];

    int m_selected;
    //wxMenu* m_popup;

    DECLARE_EVENT_TABLE()
};

enum
{
    BLIST_LIST = wxID_HIGHEST,
    BLIST_DLMOD,
    BLIST_DLMAP
};

#endif

#endif // SPRINGLOBBY_HEADERGUARD_TORRENTLISTCTRL_H
