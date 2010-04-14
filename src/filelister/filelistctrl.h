#ifndef SPRINGLOBBY_HEADERGUARD_FILELISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_FILELISTCTRL_H

#ifndef NO_TORRENT_SYSTEM

#include "../customvirtlistctrl.h"

class wxMenu;
class wxListEvent;
class wxCommandEvent;
class FileListDialog;
class PlasmaResourceInfo;

/** \brief list currently available torrents on tracker */
class FileListCtrl : public CustomVirtListCtrl< const PlasmaResourceInfo* ,FileListCtrl>
{
  public:
    FileListCtrl( FileListDialog* fld );
    ~FileListCtrl();

    typedef std::vector<wxString> InternalNameVector;

    void OnListRightClick( wxListEvent& event );
//    virtual void SetTipWindowText( const long item_hit, const wxPoint position);
//    void OnColClick( wxListEvent& event );
    void GetSelectedHashes(InternalNameVector&);
    bool AddFile( DataType info );

    //these are overloaded to use list in virtual style
    wxString GetItemText(long item, long column) const;
    int GetItemColumnImage(long item, long column) const;
    wxListItemAttr * GetItemAttr(long item) const;

  protected:

    //! passed as callback to generic ItemComparator, returns -1,0,1 as per defined ordering
	int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) const;
    //! utils func for comparing user status, so the CompareOneCrit doesn't get too crowded
    static int CompareUserStatus( DataType u1, DataType u2 );
    //! required per base clase
    virtual void Sort( );

    int GetIndexFromData( const DataType& data ) const;

  protected:
    wxMenu* m_popup;

    FileListDialog* m_parent_dialog;

    enum
    {
        FILELIST_COL_CLICK = wxID_HIGHEST,

    };

    DECLARE_EVENT_TABLE()
};



#endif

#endif // SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

