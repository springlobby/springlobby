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
#include "keybinderdef.h"
#include "wx/panel.h"
#include "wx/dialog.h"
#include "wx/listbox.h"
#include "wx/treectrl.h"
#include "keybinder.h"


//! Returns the ID of the first menu item with the given name which
//! is found inside the given menu bar.
int wxFindMenuItem(wxMenuBar *, const wxString &strMenuItemName);

//! The command type identifier for a wxMenuCmd.
#define wxMENUCMD_TYPE						0x1234


//! Represents a wxCmd which just generates a wxEVT_COMMAND_MENU_SELECTED
//! when is executed.
class wxMenuCmd : public wxCmd
{
	//! The menuitem which is connected to this command
	wxMenuItem *m_pItem;

public:		// static

	//! The menubar used by #CreateNew.
	static wxMenuBar *m_pMenuBar;

	//! Sets the menubar which will be used to search the menu items
	//! with the IDs given to the #CreateNew function.
	static void SetMenuBar(wxMenuBar *p)
		{ m_pMenuBar = p; }

	//! Register this type of command into wxCmd static array.
	//! Also calls the #SetMenuBar function with the given pointer.
	static void Register(wxMenuBar *p)
		{ wxCmd::AddCmdType(wxMENUCMD_TYPE, wxMenuCmd::CreateNew);
			wxMenuCmd::SetMenuBar(p); }

	//! Creates a new wxMenuCmd with the given ID.
	//! Automatically searches into the #m_pMenuBar variable the.
	static wxCmd *CreateNew(int id);

public:

	wxMenuCmd(wxMenuItem *p,
		const wxKeyBind &first,
		const wxString &name = wxEmptyString,
		const wxString &desc = wxEmptyString) :
		 wxCmd(first, p->GetId(), name, desc), m_pItem(p) {}

	wxMenuCmd(wxMenuItem *p = NULL,
		const wxString &name = wxEmptyString,
		const wxString &desc = wxEmptyString) {
		m_pItem = p;
		m_strDescription = desc;
		m_strName = name;

		if (m_pItem)
			m_nId = m_pItem->GetId();
	}

	virtual ~wxMenuCmd() {}

public:

	virtual void DeepCopy(const wxCmd *p) {
		wxMenuCmd *m = (wxMenuCmd *)p;
		m_pItem = m->m_pItem;
		wxCmd::DeepCopy(p);
	}

	virtual wxCmd *Clone() const {
		wxCmd *ret = new wxMenuCmd();
		ret->DeepCopy(this);
		return ret;
	}

	virtual int GetType() const {
		return wxMENUCMD_TYPE;
	}



protected:

	//! Generates the wxEVT_COMMAND_MENU_SELECTED and sends it to the
	//! client event handler which is given as client.
	void Exec(wxObject *origin, wxEvtHandler *client);

	//! Updates the associated menu item; in particularly, adds,
	//! removes or changes the first shortcut which is shown
	//! next to the menuitem label.
	//! The 2nd, 3rd, 4th... shortcuts cannot be shown in the
	//! menuitem label and they are thus not considered...
	//! (but they do still work !!!)
	void Update();
};



//! The base class for a generic tree-walker algorithm.
//! To perform your task, provide the implementation of the #OnMenuWalk
//! and #OnMenuItemWalk functions.
class wxMenuWalker
{
protected:

	//! The nest level.
	int m_nLevel;

public:
	wxMenuWalker() { m_nLevel=0; }
	virtual ~wxMenuWalker() {}

protected:		// the core functions

	//! Starts to climb the tree  :-)
	//! This is the main function to be called when everything is
	//! ready to begin.
	//! For an explanation of the 'data' argument, see below.
	void Walk(wxMenuBar *p, void *data);

	void WalkMenu(wxMenuBar *p, wxMenu *, void *);
	void WalkMenuItem(wxMenuBar *p, wxMenuItem *, void *);


	//! Called when a wxMenu is found in the menu bar passed to #Walk(),
	//! or when a submenu is found by #WalkMenuItem().
	//! - The "p" argument is the pointer given to #Walk()
	//! - The "m" argument is the menu which has been found
	//! - The "data" argument is a pointer to something which was
	//!   given to #Walk() as 'data' argument (if the given menu is
	//!   a top level menu) or the pointer to something which was
	//!   returned by #OnMenuItemWalk, if this function is being
	//!   called by #WalkMenuItem.
	//! The returned value is passed to the #OnMenuItemWalk function
	//! for all the items in the contained menu and then it's deleted
	//! through #DeleteData().
	virtual void *OnMenuWalk(wxMenuBar *p, wxMenu *m, void *data) = 0;

	//! Called when a wxMenuItem is found inside a wxMenu.
	//! This works like #OnMenuWalk() but "data" is something which was
	//! given to the #WalkMenuItem (which is always the caller of this
	//! function).
	//! If the current menu item contains a submenu, the returned value
	//! is passed to #OnMenuWalk() or it's directly deleted...
	virtual void *OnMenuItemWalk(wxMenuBar *p, wxMenuItem *m, void *data) = 0;

	//! Called when a wxMenu has been exausted and the nest level is going
	//! to be decreased. Default implementation does nothing.
	virtual void OnMenuExit(wxMenuBar *, wxMenu *, void *) {}

	//! Deletes the given 'data'.
	//! The derived class should pass to the OnMenuWalk/OnMenuWalkItem
	//! functions always the same type of structure/class in 'data'
	//! and in this function it should just cast the given pointer to
	//! that type and delete it.
	virtual void DeleteData(void *data) = 0;
};



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
	wxExTreeItemData(int id = wxID_INVALID) : m_nMenuId(id) {}
	virtual ~wxExTreeItemData() {}

	void SetMenuItemId(int n) {
		m_nMenuId = n;
	}

	int GetMenuItemId() {
		return m_nMenuId;
	}
};


//! A wxMenuWalker-derived class which uses the recursive capabilities
//! of the tree-walker algorithm to populate a wxTreeCtrl with a
//! structure identic to the given menubar.
//!
//! Each node of the wxTreeCtrl is associated with a wxExTreeItemData
//! object which contains the ID of the menuitem it represents
//! (for items representing the top level menus, wxID_INVALID is
//! contained).
class wxMenuTreeWalker : public wxMenuWalker
{
	wxTreeCtrl *m_pTreeCtrl;
	wxTreeItemId m_root;

public:
	wxMenuTreeWalker() {}
	virtual ~wxMenuTreeWalker() {}


	//! Attaches the tree structure of the menubar in the given tree control
	//! using the given tree item as root.
	void FillTreeBranch(wxMenuBar *p, wxTreeCtrl *ctrl, wxTreeItemId branch);

	//! Works like #FillTreeBranch but this function also removes all the tree
	//! items (before starting) and then builds the root with the given label.
	void FillTreeCtrl(wxMenuBar *p, wxTreeCtrl *ctrl,
		const wxString &rootname = wxT("root")) {
		ctrl->DeleteAllItems();
		FillTreeBranch(p, ctrl, ctrl->AddRoot(rootname));
	}

protected:

	void *OnMenuWalk(wxMenuBar *p, wxMenu *, void *);
	void *OnMenuItemWalk(wxMenuBar *p, wxMenuItem *, void *);
	void DeleteData(void *data);
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
	wxExComboItemData() {}
	virtual ~wxExComboItemData() {}

	int GetID(size_t n) const
 		{ return m_arrID[n]; }
	wxArrayString &GetCmdNameArr()
		{ return m_arrStr; }

	void Append(const wxString &name, int id)
		{ m_arrStr.Add(name); m_arrID.Add(id); }
	void SetID(size_t n, int id)
		{ m_arrID[n] = id; }
};


//! A wxMenuWalker-derived class which fills the given wxComboBox
//! with items associated with wxExComboItemData classes.
//! The labels of the items of the wxComboBox are the titles of
//! the wxMenus (nested & non-nested) contained in the wxMenuBar
//! which is "walked".
//! The wxExComboItemData associated with these items contains the
//! list of the wxMenuItem contained into the relative wxMenu.
class wxMenuComboListWalker : public wxMenuWalker
{
	wxComboBox *m_pCategories;
	wxString m_strAcc;

public:
	wxMenuComboListWalker() {}
	virtual ~wxMenuComboListWalker() {}

	//! Inserts into the given combobox all the menus
	void FillComboListCtrl(wxMenuBar *p, wxComboBox *combo);

protected:

	void *OnMenuWalk(wxMenuBar *p, wxMenu *, void *);
	void *OnMenuItemWalk(wxMenuBar *p, wxMenuItem *, void *);
	void OnMenuExit(wxMenuBar *p, wxMenu *m, void *data);
 	void DeleteData(void *data);
};



//! An helper class which is used by wxKeyBinder to import the
//! wxMenuCmd associated to a menu bar.
//! This function uses the wxMenuWalker algorithm to create a
//! plain list of wxMenuCmd (without any hierarchical structure)
//! in the given array of wxCmd*.
class wxMenuShortcutWalker : public wxMenuWalker
{
	wxCmdArray *m_pArr;

public:
	wxMenuShortcutWalker() {}
	virtual ~wxMenuShortcutWalker() {}


	// Starts the process.
	void ImportMenuBarCmd(wxMenuBar *p, wxCmdArray *arr) {
		m_pArr = arr;
		Walk(p, NULL);
	}


protected:

	void *OnMenuWalk(wxMenuBar *, wxMenu *, void *) { return NULL; }
	void *OnMenuItemWalk(wxMenuBar *p, wxMenuItem *, void *);
	void DeleteData(void *data);
};


#endif		// __WX_MENUUTILS_H__
