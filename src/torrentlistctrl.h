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

/** \brief list all currently active (queued,lecching,seeding) torrents with their infos
 * the list is newly populated every n-seconds from Ui::OnUpdate()
 */
class TorrentListCtrl : public customListCtrl
{
  public:
    TorrentListCtrl( wxWindow* parent, Ui& ui );
    ~TorrentListCtrl();

    void Sort();

    void OnListRightClick( wxListEvent& event );

    virtual void SetTipWindowText( const long item_hit, const wxPoint position);
    void OnColClick( wxListEvent& event );
    void SetInfoMap( map_infos* map);
    virtual void HighlightItem( long item );

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
    static int wxCALLBACK CompareFileSizeUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareFileSizeDOWN(long item1, long item2, long sortData);
    static map_infos* m_info_map;

    struct {
      int col;
      bool direction;
    } m_sortorder[4];

    //wxMenu* m_popup;

    DECLARE_EVENT_TABLE()
};

enum
{
    TLIST_CLICK

};

#endif

#endif // SPRINGLOBBY_HEADERGUARD_TORRENTLISTCTRL_H
