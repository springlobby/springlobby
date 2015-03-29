/////////////////////////////////////////////////////////////////////////////
// Name:        keybinder.h
// Purpose:     Classes for binding keypresses to commands.
// Author:      Francesco Montorsi
// Created:     2004/02/19
// RCS-ID:      $Id: menuutils.h 443 2007-03-01 19:15:57Z frm $
// Copyright:   (c) Aleksandras Gluchovas and (c) Francesco Montorsi
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_MENUUTILS_H__
#define __WX_MENUUTILS_H__


// includes
#include "wx/panel.h"
#include "wx/dialog.h"
#include "wx/listbox.h"
#include "wx/treectrl.h"
#include "keybinder.h"


//! Returns the ID of the first menu item with the given name which
//! is found inside the given menu bar.
int wxFindMenuItem(wxMenuBar*, const wxString& strMenuItemName);

//! The command type identifier for a wxMenuCmd.
#define wxMENUCMD_TYPE 0x1234

//! The data associated to each node of a tree used by
//! - wxMenuTreeWalker
//! - wxKeyConfigPanel
//!
//! This is a simple class which just contains an additional field,
//! which can be used to store the ID of the menu item associated
//! with this tree item.
//! The ID should be unique among all the commands contained in the
//! same command collection so it should be enough to identify a wxCmd...

class wxExTreeItemData : public wxTreeItemData
{
protected:
	int m_nMenuId;

public:
	wxExTreeItemData(int id = wxID_INVALID)
	    : m_nMenuId(id)
	{
	}
	virtual ~wxExTreeItemData()
	{
	}

	void SetMenuItemId(int n)
	{
		m_nMenuId = n;
	}

	int GetMenuItemId()
	{
		return m_nMenuId;
	}
};


//! The data associated with each entry of the combobox given
//! to the wxMenuComboListWalker class.
//! Contains the array of labels of the menu items contained
//! into a wxMenu and their IDs.
class wxExComboItemData : public wxClientData
{
protected:
	wxArrayString m_arrStr;
	wxArrayLong m_arrID;

public:
	wxExComboItemData()
	{
	}
	virtual ~wxExComboItemData()
	{
	}

	int GetID(size_t n) const
	{
		return m_arrID[n];
	}
	wxArrayString& GetCmdNameArr()
	{
		return m_arrStr;
	}

	void Append(const wxString& name, int id)
	{
		m_arrStr.Add(name);
		m_arrID.Add(id);
	}
	void SetID(size_t n, int id)
	{
		m_arrID[n] = id;
	}
};


#endif // __WX_MENUUTILS_H__
