#ifndef SPRINGLOBBY_HEADERGUARD_FILELISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_FILELISTCTRL_H

#ifndef NO_TORRENT_SYSTEM

#include "filelistfilter.h"

#include "../customlistctrl.h"
#include <wx/intl.h>

class wxMenu;
class wxListEvent;
class wxCommandEvent;
class FileListDialog;

/** \brief list currently available torrents on tracker */
class FileListCtrl : public CustomListCtrl
{
  public:
    FileListCtrl( wxWindow* parent, FileListDialog* fld );
    ~FileListCtrl();

    typedef std::vector<wxString> HashVector;

    void Sort();

    void OnListRightClick( wxListEvent& event );
    virtual void SetTipWindowText( const long item_hit, const wxPoint position);
    void OnColClick( wxListEvent& event );
    void GetSelectedHashes(HashVector&);
    void SetColumnWidths();
    virtual void HighlightItem( long item );

  protected:
    static int wxCALLBACK CompareNameUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareNameDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareHashUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareHashDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareTypeUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareTypeDOWN(long item1, long item2, long sortData);

    wxMenu* m_popup;
//    Ui& m_ui;
//    static Ui* m_ui_for_sort;

    FileListDialog* m_parent_dialog;
    static FileListDialog* s_parent_dialog;

    DECLARE_EVENT_TABLE()
};

enum
{
    FILELIST_COL_CLICK = wxID_HIGHEST,

};

#endif

#endif // SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

