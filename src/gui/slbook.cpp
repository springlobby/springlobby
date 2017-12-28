/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "slbook.h"
#include "settings.h"
#include "gui/chatpanel.h"
#include "gui/chatpanelmenu.h"
#include "gui/mainwindow.h"

#include <wx/menu.h>
#include <wx/scrolwin.h>
#include "utils/conversion.h"

static const long ID_CLOSE_TAB = wxNewId();
static const long ID_CLOSE_TAB_OTHER = wxNewId();
static const long ID_CLOSE_TAB_ALL = wxNewId();
static const long ID_NEW_TAB = wxNewId();

SLNotebook::SLNotebook(wxWindow* parent, const wxString& name, wxWindowID id,
		       const wxPoint& pos, const wxSize& size, long style)
    : wxAuiNotebook(parent, id, pos, size, style)
{
	SetName(name);
	m_mgr.SetFlags(wxAUI_MGR_ALLOW_FLOATING |

		       wxAUI_MGR_HINT_FADE |
		       wxAUI_MGR_VENETIAN_BLINDS_HINT);
}

// Advances the selection, generation page selection events
void SLNotebook::AdvanceSelection(bool forward)
{
	if (GetPageCount() <= 1)
		return;
	int currentSelection = GetSelection();
	wxAuiTabCtrl* tabCtrl = 0;
	int idx = -1;

	if (!FindTab(GetPage(currentSelection), &tabCtrl, &idx))
		return;

	if (!tabCtrl || idx < 0)
		return;

	wxWindow* page = 0;
	size_t maxPages = tabCtrl->GetPageCount();

	forward ? idx++ : idx--;

	if (idx < 0)
		idx = maxPages - 1;

	if ((size_t)idx < maxPages)
		page = tabCtrl->GetPage(idx).window;

	if (!page && maxPages > 0)
		page = tabCtrl->GetPage(0).window;

	if (page) {
		currentSelection = GetPageIndex(page);
		SetSelection(currentSelection);
	}
}


BEGIN_EVENT_TABLE(SLChatNotebook, SLNotebook)

EVT_AUINOTEBOOK_TAB_RIGHT_UP(wxID_ANY, SLChatNotebook::OnHeaderRightClick)
//     EVT_AUINOTEBOOK_ALLOW_DND(wxID_ANY, SLChatNotebook::OnAllowNotebookDnD)
//     EVT_AUINOTEBOOK_BEGIN_DRAG(wxID_ANY, SLChatNotebook::OnAllowNotebookDnD)
//     EVT_AUINOTEBOOK_DRAG_DONE(wxID_ANY, SLChatNotebook::OnAllowNotebookDnD)
//     EVT_AUINOTEBOOK_DRAG_MOTION(wxID_ANY, SLChatNotebook::OnAllowNotebookDnD)

END_EVENT_TABLE()

/** @brief SLChatNotebook
  *
  * @todo: document this function
  */
SLChatNotebook::SLChatNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : SLNotebook(parent, _T("chatnotebook"), id, pos, size, sett().GetShowXallTabs() ? style | wxAUI_NB_CLOSE_ON_ALL_TABS : style)
    , m_ch_menu(0)
    , m_cur_page(0)
{
}

//should work accroding to docs/sample, but doesn't (not even in sample)
void SLChatNotebook::OnAllowNotebookDnD(wxAuiNotebookEvent& evt)
{
	evt.Veto();
}

/** @brief OnHeaderRightClick
  *
  * @todo: document this function
  */
void SLChatNotebook::OnHeaderRightClick(wxAuiNotebookEvent& event)
{
	if (event.GetSelection() == -1)
		return;

	m_cur_page = static_cast<ChatPanel*>(GetPage(event.GetSelection()));
	wxMenu* pop = new wxMenu;
	pop->Append(ID_NEW_TAB, _("New Tab"));
	pop->Append(ID_CLOSE_TAB, _("Close"));
	if (GetPageCount() > 1) {
		pop->Append(ID_CLOSE_TAB_ALL, _("Close all chats"));
		pop->Append(ID_CLOSE_TAB_OTHER, _("Close all other chats"));
	}
	m_ch_menu = new ChatPanelMenu(m_cur_page, true);
	if (m_cur_page->GetPanelType() == CPT_User) {
		wxMenu* panel_menu = m_ch_menu->GetMenu();
		panel_menu->Remove(panel_menu->FindItem(_("User")));
		pop->AppendSubMenu(panel_menu, _("Panel"));
		pop->AppendSubMenu(m_ch_menu->GetUserMenuNoCreate(), _("User"));
	} else
		pop->AppendSubMenu(m_ch_menu->GetMenu(), _("Channel"));
	Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SLChatNotebook::OnMenuItem), 0, this);
	PopupMenu(pop);
}

bool SLChatNotebook::DeleteChatPage(size_t i)
{
	ChatPanel* cur_page = static_cast<ChatPanel*>(GetPage(i));

	//the checking for server panel is not supposed to be here, but it prevents a nasty crash bug in handling 'close all' for the time being
	if (cur_page && (cur_page->GetPanelType() != ChatPanelType::CPT_Server)) {
		cur_page->Part();
		SLNotebook::DeletePage(i);
		return true;
	}
	return false;
}

void SLChatNotebook::OnMenuItem(wxCommandEvent& event)
{
	long id = event.GetId();
	if (id == ID_NEW_TAB) {
		ui().mw().OnMenuJoin(event);
	} else if (id == ID_CLOSE_TAB) {
		DeleteChatPage(GetSelection());
	} else if (id == ID_CLOSE_TAB_ALL) {
		for (size_t i = 0; i < GetPageCount(); ++i) {
			if (DeleteChatPage(i))
				i--;
		}
	} else if (id == ID_CLOSE_TAB_OTHER) {
		size_t selected = GetSelection();
		for (size_t i = 0; i < GetPageCount(); ++i) {
			if (i == selected)
				continue;
			else {
				if (DeleteChatPage(i)) {
					i--;
					selected--;
				}
			}
		}
	} else {
		if (m_cur_page) {
			m_ch_menu->OnMenuItem(event);
		}
	}
	Disconnect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SLChatNotebook::OnMenuItem), 0, this);
}

// wxTabFrame is an interesting case.  It's important that all child pages
// of the multi-notebook control are all actually children of that control
// (and not grandchildren).  wxTabFrame facilitates this.  There is one
// instance of wxTabFrame for each tab control inside the multi-notebook.
// It's important to know that wxTabFrame is not a real window, but it merely
// used to capture the dimensions/positioning of the internal tab control and
// it's managed page windows

class wxTabFrame : public wxWindow
{
public:
	wxTabFrame()
	{
		m_tabs = NULL;
		m_rect = wxRect(0, 0, 200, 200);
		m_tab_ctrl_height = 20;
	}

	~wxTabFrame()
	{
		wxDELETE(m_tabs);
	}

	void SetTabCtrlHeight(int h)
	{
		m_tab_ctrl_height = h;
	}

	void DoSetSize(int x, int y,
		       int width, int height,
		       int WXUNUSED(sizeFlags = wxSIZE_AUTO))
	{
		m_rect = wxRect(x, y, std::max(1, width), std::max(1, height));
		DoSizing();
	}

	void DoGetClientSize(int* x, int* y) const
	{
		*x = m_rect.width;
		*y = m_rect.height;
	}

	bool Show(bool WXUNUSED(show = true))
	{
		return false;
	}

	void DoSizing()
	{
		if (!m_tabs)
			return;

		if (m_tabs->GetFlags() & wxAUI_NB_BOTTOM) {
			m_tab_rect = wxRect(m_rect.x, m_rect.y + m_rect.height - m_tab_ctrl_height, m_rect.width, m_tab_ctrl_height);
			m_tabs->SetSize(m_rect.x, m_rect.y + m_rect.height - m_tab_ctrl_height, m_rect.width, m_tab_ctrl_height);
			m_tabs->SetRect(wxRect(0, 0, m_rect.width, m_tab_ctrl_height));
		} else //TODO: if (GetFlags() & wxAUI_NB_TOP)
		{
			m_tab_rect = wxRect(m_rect.x, m_rect.y, m_rect.width, m_tab_ctrl_height);
			m_tabs->SetSize(m_rect.x, m_rect.y, m_rect.width, m_tab_ctrl_height);
			m_tabs->SetRect(wxRect(0, 0, m_rect.width, m_tab_ctrl_height));
		}
		// TODO: else if (GetFlags() & wxAUI_NB_LEFT){}
		// TODO: else if (GetFlags() & wxAUI_NB_RIGHT){}

		m_tabs->Refresh();
		m_tabs->Update();

		wxAuiNotebookPageArray& pages = m_tabs->GetPages();
		size_t i, page_count = pages.GetCount();

		for (i = 0; i < page_count; ++i) {
			wxAuiNotebookPage& page = pages.Item(i);
			if (m_tabs->GetFlags() & wxAUI_NB_BOTTOM) {
				page.window->SetSize(m_rect.x, m_rect.y,
						     std::max(1, m_rect.width), std::max(1, m_rect.height - m_tab_ctrl_height));
			} else //TODO: if (GetFlags() & wxAUI_NB_TOP)
			{
				page.window->SetSize(m_rect.x, m_rect.y + m_tab_ctrl_height,
						     std::max(1, m_rect.width), std::max(1, m_rect.height - m_tab_ctrl_height));
			}
			// TODO: else if (GetFlags() & wxAUI_NB_LEFT){}
			// TODO: else if (GetFlags() & wxAUI_NB_RIGHT){}
		}
	}

	void DoGetSize(int* x, int* y) const
	{
		if (x)
			*x = m_rect.GetWidth();
		if (y)
			*y = m_rect.GetHeight();
	}

	void Update()
	{
		// does nothing
	}

public:
	wxRect m_rect;
	wxRect m_tab_rect;
	wxAuiTabCtrl* m_tabs;
	int m_tab_ctrl_height;
};


wxString SLNotebook::SavePerspective()
{
	// Build list of panes/tabs
	wxString tabs;
	wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
	const size_t pane_count = all_panes.GetCount();

	for (size_t i = 0; i < pane_count; ++i) {
		wxAuiPaneInfo& pane = all_panes.Item(i);
		if (pane.name == wxT("dummy"))
			continue;

		wxTabFrame* tabframe = (wxTabFrame*)pane.window;

		if (!tabs.empty())
			tabs += wxT("|");
		tabs += pane.name;
		tabs += wxT("=");

		// add tab id's
		size_t page_count = tabframe->m_tabs->GetPageCount();
		for (size_t p = 0; p < page_count; ++p) {
			wxAuiNotebookPage& page = tabframe->m_tabs->GetPage(p);
			const size_t page_idx = m_tabs.GetIdxFromWindow(page.window);

			if (p)
				tabs += wxT(",");

			if ((int)page_idx == GetSelection())
				tabs += wxT("*");
			else if ((int)p == tabframe->m_tabs->GetActivePage())
				tabs += wxT("+");
			tabs += wxString::Format(wxT("%d"), (int)page_idx);
		}
	}
	tabs += wxT("@");

	// Add frame perspective
	tabs += m_mgr.SavePerspective();

	return tabs;
}

bool SLNotebook::LoadPerspective(const wxString& layout)
{
	// Remove all tab ctrls (but still keep them in main index)
	const size_t tab_count = m_tabs.GetPageCount();
	for (size_t i = 0; i < tab_count; ++i) {
		wxWindow* wnd = m_tabs.GetWindowFromIdx(i);

		// find out which onscreen tab ctrl owns this tab
		wxAuiTabCtrl* ctrl;
		int ctrl_idx;
		if (!FindTab(wnd, &ctrl, &ctrl_idx))
			return false;

		// remove the tab from ctrl
		if (!ctrl->RemovePage(wnd))
			return false;
	}
	RemoveEmptyTabFrames();

	size_t sel_page = 0;

	wxString tabs = layout.BeforeFirst(wxT('@'));
	while (1) {
		const wxString tab_part = tabs.BeforeFirst(wxT('|'));

		// if the string is empty, we're done parsing
		if (tab_part.empty())
			break;

		// Get pane name
		const wxString pane_name = tab_part.BeforeFirst(wxT('='));

		// create a new tab frame
		wxTabFrame* new_tabs = new wxTabFrame;
		const int nextid = m_tabIdCounter++;
		new_tabs->m_tabs = new wxAuiTabCtrl(this,
						    nextid,
						    wxDefaultPosition,
						    wxDefaultSize,
						    wxNO_BORDER | wxWANTS_CHARS);
		new_tabs->m_tabs->SetArtProvider(m_tabs.GetArtProvider()->Clone());
		new_tabs->SetTabCtrlHeight(GetTabCtrlHeight());
		new_tabs->m_tabs->SetFlags(m_flags);
		wxAuiTabCtrl* dest_tabs = new_tabs->m_tabs;

		// create a pane info structure with the information
		// about where the pane should be added
		wxAuiPaneInfo pane_info = wxAuiPaneInfo().Name(pane_name).Bottom().CaptionVisible(false);
		m_mgr.AddPane(new_tabs, pane_info);

		// Get list of tab id's and move them to pane
		wxString tab_list = tab_part.AfterFirst(wxT('='));
		while (1) {
			wxString tab = tab_list.BeforeFirst(wxT(','));
			if (tab.empty())
				break;
			tab_list = tab_list.AfterFirst(wxT(','));

			// Check if this page has an 'active' marker
			const wxChar c = tab[0];
			if (c == wxT('+') || c == wxT('*')) {
				tab = tab.Mid(1);
			}

			const size_t tab_idx = wxAtoi(tab.c_str());
			if (tab_idx >= GetPageCount())
				continue;

			// Move tab to pane
			wxAuiNotebookPage& page = m_tabs.GetPage(tab_idx);
			const size_t newpage_idx = dest_tabs->GetPageCount();
			dest_tabs->InsertPage(page.window, page, newpage_idx);
			if (c == wxT('+'))
				dest_tabs->SetActivePage(newpage_idx);
			else if (c == wxT('*'))
				sel_page = tab_idx;
		}
		dest_tabs->DoShowHide();

		tabs = tabs.AfterFirst(wxT('|'));
	}

	// Load the frame perspective
	const wxString frames = layout.AfterFirst(wxT('@'));
	m_mgr.LoadPerspective(frames);

	// Force refresh of selection
	SetSelection(sel_page);

	return true;
}

void SLNotebook::FitChildPages()
{
	for (size_t i = 0; i < GetPageCount(); ++i) {
		wxWindow* page = GetPage(i);
		if (page)
			page->FitInside();
	}
}

void LoadNotebookPerspective(SLNotebook* notebook, const wxString& perspective_name)
{
	wxString pers_name = (perspective_name.IsEmpty() ? sett().GetLastPerspectiveName() : perspective_name);
	wxString name = notebook->GetName();
	wxString pers = sett().LoadPerspective(name, pers_name);
	if (!pers.IsEmpty()) {
		notebook->LoadPerspective(pers);
		notebook->Layout();
		wxWindow* parent = notebook->GetParent();

		if (parent) {
			parent->Layout();
			parent->Refresh();
		}
	}
}

void SaveNotebookPerspective(SLNotebook* notebook, const wxString& perspective_name)
{
	wxString pers = (perspective_name.IsEmpty() ? sett().GetLastPerspectiveName() : perspective_name);
	wxString name = notebook->GetName();
	sett().SavePerspective(name, pers, notebook->SavePerspective());
}
