/////////////////////////////////////////////////////////////////////////////
// Name:        menuutils.cpp
// Purpose:     wxMenuCmd, wxMenuWalker, wxMenuTreeWalker, 
//              wxMenuShortcutWalker...
// Author:      Francesco Montorsi
// Created:     2004/02/19
// RCS-ID:      $Id: menuutils.cpp 443 2007-03-01 19:15:57Z frm $
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// includes
#include "menuutils.h"


// static
wxMenuBar *wxMenuCmd::m_pMenuBar = NULL;




// ----------------------------------------------------------------------------
// Global utility functions
// ----------------------------------------------------------------------------

int wxFindMenuItem(wxMenuBar *p, const wxString &str)
{
	int id = wxNOT_FOUND;

	for (int i=0; i < (int)p->GetMenuCount(); i++) {

		id = p->GetMenu(i)->FindItem(str);
		if (id != wxNOT_FOUND)
			break;
	}

	return id;
}

/*
#ifdef __WXGTK__

	#include "wx/gtk/private.h"
	#include <gdk/gdkkeysyms.h>

#endif
*/


// ----------------------------------------------------------------------------
// wxMenuCmd
// ----------------------------------------------------------------------------

void wxMenuCmd::Update()
{
	wxString str = m_pItem->GetLabel();

#ifdef __WXGTK__
	// on GTK, an optimization in wxMenu::SetText checks
	// if the new label is identic to the old and in this
	// case, it returns without doing nothing... :-(
	// to solve the problem, a space is added or removed 
	// from the label to ovverride this optimization check
	str.Trim();
	if (str == m_pItem->GetLabel())
		str += wxT(" ");
#endif

	if (m_nShortcuts <= 0) {
	
		wxKBLogDebug(wxT("wxMenuCmd::Update - no shortcuts defined for [%s]"), str.c_str());

		// no more shortcuts for this menuitem: SetText()
		// will delete the hotkeys associated...
		m_pItem->SetText(str);
		return;
	}

	wxString newtext = str+wxT("\t")+GetShortcut(0)->GetStr();
	wxKBLogDebug(wxT("wxMenuCmd::Update - setting the new text to [%s]"), newtext.c_str());
	
#if defined( __WXMSW__ )

	// change the accelerator...
	m_pItem->SetText(newtext);
	m_pItem->GetMenu()->UpdateAccel(m_pItem);

	// we could also do that in this way:
	//    wxAcceleratorEntry acc = GetAccelerator(0);
	//    m_pItem->SetAccel(&acc);
	// but this is just slower because wxMenuItem::SetAccel
	// creates a string from the accelerator entry we give to it
	// (i.e. it internally builds a string analogue to our 'newtext')
	// and then calls wxMenuItem::SetText...

#elif defined( __WXGTK__ )

	// on GTK, the SetAccel() function doesn't have any effect...	   
	m_pItem->SetText(newtext);

#ifdef __WXGTK20__

	//   gtk_menu_item_set_accel_path(GTK_MENU_ITEM(m_pItem), wxGTK_CONV(newtext));

#endif
#endif
}

void wxMenuCmd::Exec(wxObject *origin, wxEvtHandler *client)
{
	wxCommandEvent menuEvent(wxEVT_COMMAND_MENU_SELECTED, GetId()); 
	wxASSERT_MSG(client, wxT("An empty client handler ?!?"));

	// set up the event and process it...
	menuEvent.SetEventObject(origin);
	client->AddPendingEvent(menuEvent);//ProcessEvent(menuEvent);
}

wxCmd *wxMenuCmd::CreateNew(int id)
{
	if (!m_pMenuBar) return NULL;

	// search the menuitem which is tied to the given ID
	wxMenuItem *p = m_pMenuBar->FindItem(id);

	if (!p) return NULL;
	wxASSERT(id == p->GetId());
	return new wxMenuCmd(p);
}




// ----------------------------------------------------------------------------
// wxMenuWalker
// ----------------------------------------------------------------------------

void wxMenuWalker::WalkMenuItem(wxMenuBar *p, wxMenuItem *m, void *data)
{
	wxKBLogDebug(wxT("wxMenuWalker::WalkMenuItem - walking on [%s] at level [%d]"), 
				m->GetLabel().c_str(), m_nLevel);
	void *tmp = OnMenuItemWalk(p, m, data);

	if (m->GetSubMenu()) {

		// if this item contains a sub menu, add recursively the menu items
		// of that sub menu... using the cookie from OnMenuItemWalk.
		wxKBLogDebug(wxT("wxMenuWalker::WalkMenuItem - recursing on [%s]"), m->GetLabel().c_str());
		m_nLevel++;
		WalkMenu(p, m->GetSubMenu(), tmp);
		OnMenuExit(p, m->GetSubMenu(), tmp);
		m_nLevel--;
	}

	// we can delete the cookie we got form OnMenuItemWalk
	DeleteData(tmp);
}

void wxMenuWalker::WalkMenu(wxMenuBar *p, wxMenu *m, void *data)
{
	wxKBLogDebug(wxT("wxMenuWalker::WalkMenu - walking on [%s] at level [%d]"), 
				m->GetTitle().c_str(), m_nLevel);
	for (int i=0; i < (int)m->GetMenuItemCount(); i++) {

		wxMenuItem *pitem = m->GetMenuItems().Item(i)->GetData();
		
		// inform the derived class that we have reached a menu
		// and get the cookie to give on WalkMenuItem...
		void *tmp = OnMenuWalk(p, m, data);

		// skip separators (on wxMSW they are marked as wxITEM_NORMAL
		// but they do have empty labels)...
		if (pitem->GetKind() != wxITEM_SEPARATOR && 
			pitem->GetLabel() != wxEmptyString)
			WalkMenuItem(p, pitem, tmp);

		// the cookie we gave to WalkMenuItem is not useful anymore		
		DeleteData(tmp);
	}

	OnMenuExit(p, m, data);
}

void wxMenuWalker::Walk(wxMenuBar *p, void *data)
{
	wxASSERT(p);

	for (int i=0; i < (int)p->GetMenuCount(); i++) {

		// create a new tree branch for the i-th menu of this menubar
		wxMenu *m = p->GetMenu(i);

		m_nLevel++;
		wxKBLogDebug(wxT("wxMenuWalker::Walk - walking on [%s] at level [%d]"), 
					p->GetLabelTop(i).c_str(), m_nLevel);
		void *tmp = OnMenuWalk(p, m, data);

		// and fill it...
		WalkMenu(p, m, tmp);
		m_nLevel--;

		DeleteData(tmp);
	}
}




// ----------------------------------------------------------------------------
// wxMenuTreeWalker
// ----------------------------------------------------------------------------

void wxMenuTreeWalker::FillTreeBranch(wxMenuBar *p, wxTreeCtrl *ctrl, wxTreeItemId branch)
{
	// these will be used in the recursive functions...
	m_root = branch;
	m_pTreeCtrl = ctrl;

	// be sure that the given tree item is empty...
	m_pTreeCtrl->DeleteChildren(branch);

	// ...start !!!
	Walk(p, &branch);
}

void *wxMenuTreeWalker::OnMenuWalk(wxMenuBar *p, wxMenu *m, void *data)
{
	wxTreeItemId *id = (wxTreeItemId *)data;
	int i;

	// if we receive an invalid tree item ID, we must stop everything...
	// (in fact a NULL value given as DATA in wxMenuTreeWalker function
	// implies the immediate processing stop)...
	if (!id->IsOk())
		return NULL;
	
	// if this is the first level of menus, we must create a new tree item
	if (*id == m_root) {

		// find the index of the given menu
		for (i=0; i < (int)p->GetMenuCount(); i++)
			if (p->GetMenu(i) == m)
				break;
		wxASSERT(i != (int)p->GetMenuCount());
	
		// and append a new tree branch with the appropriate label
		wxTreeItemId newId = m_pTreeCtrl->AppendItem(*id, 
			wxMenuItem::GetLabelFromText(p->GetLabelTop(i)));

		// menu items contained in the given menu must be added
		// to the just created branch
		return new wxTreeItemId(newId);
	}

	// menu items contained in the given menu must be added
	// to this same branch...
	return new wxTreeItemId(*id);
}

void *wxMenuTreeWalker::OnMenuItemWalk(wxMenuBar *, wxMenuItem *m, void *data)
{
	wxTreeItemId *id = (wxTreeItemId *)data;
	if (id->IsOk()) {

		// to each tree branch attach a wxTreeItemData containing 
		// the ID of the menuitem which it represents...
		wxExTreeItemData *treedata = new wxExTreeItemData(m->GetId());

		// create the new item in the tree ctrl
		wxTreeItemId newId = m_pTreeCtrl->AppendItem(*id, 
			m->GetLabel(), -1, -1, treedata);
		
		return new wxTreeItemId(newId);
	}
	
	return NULL;
}

void wxMenuTreeWalker::DeleteData(void *data)
{
	wxTreeItemId *p = (wxTreeItemId *)data;
	if (p) delete p;
}





// ----------------------------------------------------------------------------
// wxMenuComboListWalker
// ----------------------------------------------------------------------------

void wxMenuComboListWalker::FillComboListCtrl(wxMenuBar *p, wxComboBox *combo)
{
	// these will be used in the recursive functions...
	m_pCategories = combo;
	
	// be sure that the given tree item is empty...
	m_pCategories->Clear();

	// ...start !!!
	Walk(p, NULL);
}

void *wxMenuComboListWalker::OnMenuWalk(wxMenuBar *p, wxMenu *m, void *)
{
	wxKBLogDebug(wxT("wxMenuWalker::OnMenuWalk - walking on [%s]"), m->GetTitle().c_str());
	wxString toadd;

	// find the index of the given menu
	if (m_strAcc.IsEmpty()) {

		int i;
		for (i=0; i < (int)p->GetMenuCount(); i++)
			if (p->GetMenu(i) == m)
				break;
		wxASSERT(i != (int)p->GetMenuCount());
		toadd = wxMenuItem::GetLabelFromText(p->GetLabelTop(i));

		m_strAcc = toadd;

	} else {

		//toadd = m->GetTitle();
		toadd = m_strAcc;
		//wxString str((wxString)()acc);
		//m_strAcc += str;
	}

	//int last = m_pCategories->GetCount()-1;
	int found;
	if ((found=m_pCategories->FindString(toadd)) != wxNOT_FOUND)
		return m_pCategories->GetClientObject(found);
	
	// create the clientdata that our new combobox item will contain
	wxClientData *cd = new wxExComboItemData();	

	// and create a new element in our combbox
	wxKBLogDebug(wxT("wxMenuWalker::OnMenuWalk - appending [%s]"), toadd.c_str());
	m_pCategories->Append(toadd, cd);
	return cd;
}

void *wxMenuComboListWalker::OnMenuItemWalk(wxMenuBar *, wxMenuItem *m, void *data)
{
	wxKBLogDebug(wxT("wxMenuWalker::OnMenuItemWalk - walking on [%s]"), m->GetLabel().c_str());
	//int last = m_pCategories->GetCount()-1;
	wxExComboItemData *p = (wxExComboItemData *)data;//m_pCategories->GetClientObject(last);

	// append a new item
	if (m->GetSubMenu() == NULL)
		p->Append(m->GetLabel(), m->GetId());
	else
		m_strAcc += wxT(" | ") + m->GetLabel();

	// no info to give to wxMenuComboListWalker::OnMenuWalk
	return NULL;//(void *)str;
}

void wxMenuComboListWalker::OnMenuExit(wxMenuBar *, wxMenu *m, void *)
{
	wxKBLogDebug(wxT("wxMenuWalker::OnMenuExit - walking on [%s]"), m->GetTitle().c_str());

	if (!m_strAcc.IsEmpty()){// && m_strAcc.Right() == str) {

		int diff = m_strAcc.Find(wxT('|'), TRUE);

		if (diff == wxNOT_FOUND)
			m_strAcc = wxEmptyString;
		else 
			m_strAcc = m_strAcc.Left(diff);
		m_strAcc.Trim();
	}
}

void wxMenuComboListWalker::DeleteData(void *)
{ /* we need NOT TO DELETE the given pointer !! */ }





// ----------------------------------------------------------------------------
// wxMenuShortcutWalker
// ----------------------------------------------------------------------------

void *wxMenuShortcutWalker::OnMenuItemWalk(wxMenuBar *, wxMenuItem *m, void *)
{
	wxASSERT(m);

	// add an entry to the command array
	wxCmd *cmd = new wxMenuCmd(m, m->GetLabel(), m->GetHelp());
	m_pArr->Add(cmd);

	// check for shortcuts
	wxAcceleratorEntry *a = m->GetAccel();		// returns a pointer which we have to delete
	if (a) {

		// this menuitem has an associated accelerator... add an entry
		// to the array of bindings for the relative command...
		cmd->AddShortcut(a->GetFlags(), a->GetKeyCode());
	}

	// cleanup
	if (a) delete a;
	return NULL;
}

void wxMenuShortcutWalker::DeleteData(void *
#ifdef __WXDEBUG__
									  data
#endif	// to avoid warnings about unused arg
									  )
{
	wxASSERT_MSG(data == NULL, 
		wxT("wxMenuShortcutWalker does not use the 'data' parameter"));	
}

