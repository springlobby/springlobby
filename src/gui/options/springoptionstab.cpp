/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: SpringOptionsTab
//

#include "springoptionstab.h"

#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/intl.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/choicdlg.h>
#include <wx/stdpaths.h>
#include <wx/scrolbar.h> // added for scroll bar powers
#include <wx/log.h>
#include <wx/listbox.h>

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif

#include "gui/ui.h"
#include "gui/controls.h"
#include "utils/platform.h"
#include "utils/uievents.h"
#include "utils/slpaths.h"
#include "utils/conversion.h"
#include "utils/version.h"
#include "gui/uiutils.h"
#include "settings.h"
#include "gui/mainwindow.h"
#include "gui/customdialogs.h"
#include <lslunitsync/springbundle.h>
#include <lslunitsync/unitsync.h>


BEGIN_EVENT_TABLE(SpringOptionsTab, wxPanel)

EVT_BUTTON(SPRING_EXECBROWSE, SpringOptionsTab::OnBrowseExec)
EVT_BUTTON(SPRING_SYNCBROWSE, SpringOptionsTab::OnBrowseSync)
EVT_BUTTON(SPRING_AUTOCONF, SpringOptionsTab::OnAutoConf)
EVT_BUTTON(SPRING_RESTORE, SpringOptionsTab::OnRestore)
EVT_BUTTON(SPRING_EXECFIND, SpringOptionsTab::OnFindExec)
EVT_BUTTON(SPRING_SYNCFIND, SpringOptionsTab::OnFindSync)
EVT_BUTTON(SPRING_DATADIR, SpringOptionsTab::OnDataDir)
EVT_BUTTON(SPRING_ADD, SpringOptionsTab::OnAddBundle)
EVT_BUTTON(SPRING_REMOVE, SpringOptionsTab::OnRemoveBundle)
EVT_CHECKBOX(SPRING_DONTSEARCH, SpringOptionsTab::OnDontSearch)
EVT_LISTBOX(SPRING_LIST, SpringOptionsTab::OnGroupListSelectionChange)

END_EVENT_TABLE()


SpringOptionsTab::SpringOptionsTab(wxWindow* parent)
    : wxScrolledWindow(parent, -1)
{
	wxBoxSizer* windowSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* groupListSizer = new wxBoxSizer(wxVERTICAL);

	m_spring_list = new wxListBox(this, SPRING_LIST, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_ALWAYS_SB | wxLB_SINGLE | wxLB_SORT);
	groupListSizer->Add(m_spring_list, 1, wxEXPAND | wxTOP | wxRIGHT | wxLEFT, 5);

	wxBoxSizer* groupListButtonsSizer = new wxBoxSizer(wxHORIZONTAL);

	m_remove_spring_button = new wxButton(this, SPRING_REMOVE, _("Remove"), wxDefaultPosition, wxSize(-1, -1), wxBU_EXACTFIT);
	m_remove_spring_button->SetToolTip(_("Remove an existing spring version"));

	groupListButtonsSizer->Add(m_remove_spring_button, 0, wxTOP | wxRIGHT | wxLEFT, 5);

	groupListButtonsSizer->Add(0, 0, 1, wxEXPAND, 5);

	m_add_spring_button = new wxButton(this, SPRING_ADD, _("Add New.."), wxDefaultPosition, wxSize(-1, -1), wxBU_EXACTFIT);
	m_add_spring_button->SetToolTip(_("Add new spring version"));

	groupListButtonsSizer->Add(m_add_spring_button, 0, wxTOP | wxRIGHT | wxLEFT, 5);

	m_auto_btn = new wxButton(this, SPRING_AUTOCONF, _("Auto Configure"), wxDefaultPosition, wxSize(-1, -1), wxBU_EXACTFIT);
	groupListButtonsSizer->Add(m_auto_btn, 0, wxTOP | wxRIGHT | wxLEFT, 5);

	groupListSizer->Add(groupListButtonsSizer, 0, wxEXPAND | wxBOTTOM, 5);

	/* ================================
	 * Spring executable
	 */
	m_exec_box = new wxStaticBox(this, -1, _("Spring executable"));

	m_exec_loc_text = new wxStaticText(this, -1, _("Location"));
	m_exec_edit = new wxTextCtrl(this, -1, TowxString(SlPaths::GetSpringBinary()));
	m_exec_browse_btn = new wxButton(this, SPRING_EXECBROWSE, _("Browse"));
	m_exec_find_btn = new wxButton(this, SPRING_EXECFIND, _("Find"));

	/* ================================
	 * UnitSync
	 */
	m_sync_box = new wxStaticBox(this, -1, _("UnitSync library"));

	m_sync_edit = new wxTextCtrl(this, -1, TowxString(SlPaths::GetUnitSync()));
	m_sync_loc_text = new wxStaticText(this, -1, _("Location"));
	m_sync_browse_btn = new wxButton(this, SPRING_SYNCBROWSE, _("Browse"));
	m_sync_find_btn = new wxButton(this, SPRING_SYNCFIND, _("Find"));

	m_restore_btn = new wxButton(this, SPRING_RESTORE, _("Restore Paths"));
	//	m_datadir_btn = new wxButton( this, SPRING_DATADIR, _( "Change Datadir path" ) );

	wxBoxSizer* m_main_sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* m_aconf_sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* m_exec_loc_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* m_sync_loc_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_exec_loc_sizer->Add(m_exec_loc_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	m_exec_loc_sizer->Add(m_exec_edit, 1, wxEXPAND);
	m_exec_loc_sizer->Add(m_exec_browse_btn);
	m_exec_loc_sizer->Add(m_exec_find_btn);

	m_sync_loc_sizer->Add(m_sync_loc_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	m_sync_loc_sizer->Add(m_sync_edit, 1, wxEXPAND);
	m_sync_loc_sizer->Add(m_sync_browse_btn);
	m_sync_loc_sizer->Add(m_sync_find_btn);

	m_exec_box_sizer = new wxStaticBoxSizer(m_exec_box, wxHORIZONTAL);
	m_sync_box_sizer = new wxStaticBoxSizer(m_sync_box, wxHORIZONTAL);

	m_exec_box_sizer->Add(m_exec_loc_sizer, 1, wxEXPAND | wxALL, 2);

	m_sync_box_sizer->Add(m_sync_loc_sizer, 1, wxEXPAND | wxALL, 2);

	m_aconf_sizer->AddStretchSpacer();
	m_aconf_sizer->Add(m_restore_btn);
	//	m_aconf_sizer->Add( m_datadir_btn );

	m_main_sizer->Add(m_exec_box_sizer, 0, wxEXPAND | wxALL, 5);
	m_main_sizer->Add(m_sync_box_sizer, 0, wxEXPAND | wxALL, 5);
	m_main_sizer->Add(m_aconf_sizer, 0, wxEXPAND | wxALL, 5);
	m_main_sizer->AddStretchSpacer();

	windowSizer->Add(groupListSizer, 0, wxEXPAND | wxBOTTOM | wxLEFT);
	windowSizer->Add(m_main_sizer, 1, wxEXPAND);
	SetSizer(windowSizer);

	SetScrollRate(SCROLL_RATE, SCROLL_RATE);

	ReloadSpringList();
	Layout();
	DoRestore();
}


SpringOptionsTab::~SpringOptionsTab()
{
        delete this->GetSizer();
}

void SpringOptionsTab::EnableSpringBox(bool enabled)
{
	m_exec_box->Enable(enabled);
	m_exec_loc_text->Enable(enabled);
	m_exec_edit->Enable(enabled);
	m_exec_browse_btn->Enable(enabled);
	m_exec_find_btn->Enable(enabled);
}

void SpringOptionsTab::EnableUnitsyncBox(bool enabled)
{
	m_sync_box->Enable(enabled);
	m_sync_loc_text->Enable(enabled);
	m_sync_edit->Enable(enabled);
	m_sync_browse_btn->Enable(enabled);
	m_sync_find_btn->Enable(enabled);
}

void SpringOptionsTab::DoRestore()
{
	const std::string index = STD_STRING(m_spring_list->GetStringSelection());
	m_sync_edit->SetValue(TowxString(SlPaths::GetUnitSync(index)));
	m_exec_edit->SetValue(TowxString(SlPaths::GetSpringBinary(index)));
	if (index.empty()) {
		m_remove_spring_button->Enable(false);
	} else {
		m_remove_spring_button->Enable(true);
	}
}


void SpringOptionsTab::OnAutoConf(wxCommandEvent& /*unused*/)
{
	SlPaths::RefreshSpringVersionList(true);
	ReloadSpringList();
}


void SpringOptionsTab::OnFindExec(wxCommandEvent& /*unused*/)
{
	const std::string index = STD_STRING(m_spring_list->GetStringSelection());
	const std::string found = SlPaths::GetSpringBinary(index);
	if (!found.empty())
		m_exec_edit->SetValue(TowxString(found));
}


void SpringOptionsTab::OnFindSync(wxCommandEvent& /*unused*/)
{
	const std::string index = STD_STRING(m_spring_list->GetStringSelection());
	const std::string found = SlPaths::GetUnitSync(index);
	if (!found.empty())
		m_sync_edit->SetValue(TowxString(found));
}

static wxString GetUnitsyncFilter()
{
#ifdef __APPLE__
	return wxString(_T("Spring application or unitsync (*.app;*.dylib)|*.app;*.dylib"));
#else
	return wxString(_T("unitsync")) << _T("(*") << TowxString(LIBEXT) << _T(")|*") << TowxString(LIBEXT);
#endif
}

static wxString GetSpringFilter()
{
	return wxString(_T("Spring executable")) << _T("(*") << TowxString(EXEEXT) << _T(")|*") << TowxString(EXEEXT);
}

void SpringOptionsTab::OnBrowseExec(wxCommandEvent& /*unused*/)
{
	wxFileDialog pick(this, _("Choose a Spring executable"),
			  wxPathOnly(TowxString(SlPaths::GetSpringBinary())),
			  wxFileName::FileName(TowxString(SlPaths::GetSpringBinary())).GetFullName(),
			  GetSpringFilter());
	if (pick.ShowModal() == wxID_OK)
		m_exec_edit->SetValue(pick.GetPath());
}

void SpringOptionsTab::OnBrowseSync(wxCommandEvent& /*unused*/)
{
	wxFileDialog pick(this, _("Choose UnitSync library"),
			  wxPathOnly(TowxString(SlPaths::GetUnitSync())),
			  wxFileName::FileName(TowxString(SlPaths::GetUnitSync())).GetFullName(),
			  GetUnitsyncFilter());
	if (pick.ShowModal() == wxID_OK)
		m_sync_edit->SetValue(pick.GetPath());
}

void SpringOptionsTab::OnAddBundle(wxCommandEvent& /*event*/)
{
	wxFileDialog pick(this, _("Choose UnitSync library"),
			  wxPathOnly(TowxString(SlPaths::GetUnitSync())),
			  wxFileName::FileName(TowxString(SlPaths::GetUnitSync())).GetFullName(),
			  GetUnitsyncFilter());
	if (pick.ShowModal() == wxID_OK) {
		//get unitsync version & add to list
		bool failed = false;
		wxString failMessage;
		LSL::SpringBundle bundle;
#ifdef __APPLE__
		wxString path = pick.GetPath();
		bundle.unitsync = STD_STRING(path + (path.EndsWith("libunitsync.dylib") ? "" : "/Contents/MacOS/libunitsync.dylib"));
#else
		bundle.unitsync = STD_STRING(pick.GetPath());
#endif
		wxString version;
		try {
			bundle.AutoComplete();
			version = TowxString(bundle.version);
			if (!bundle.IsValid() || version.IsEmpty()) { // couldn't detect paths automaticly, allow user to select spring executable
				wxFileDialog pick(this, _("Choose a Spring executable"),
						  wxPathOnly(TowxString(bundle.unitsync)),
						  wxFileName::FileName(TowxString(SlPaths::GetSpringBinary())).GetFullName(),
						  GetSpringFilter());
				if (pick.ShowModal() == wxID_OK) {
					bundle.spring = STD_STRING(pick.GetPath());
				}
			}

			if (!bundle.IsValid() || version.IsEmpty()) {
				failed = true;
				failMessage = wxString::Format(_T("%s did not find engine and unitsync executables at %s\n\nPlease ensure that both exist and that you have appropriate access privileges."), getSpringlobbyName().c_str(), bundle.path.c_str());
			}
		} catch (const LSL::Exceptions::unitsync& e) {
			failed = true;
			failMessage = wxString::Format(_T("%s could not obtain the version string from the shared library file %s\n\nPlease provide a valid unitsync file."), getSpringlobbyName().c_str(), bundle.unitsync.c_str());
		}
		if (failed) {
			customMessageBox(SL_MAIN_ICON, failMessage, _("Configuration error"), wxOK);
			return;
		}

		m_spring_list->Append(version);
		m_spring_list->SetStringSelection(version);
		m_sync_edit->SetValue(TowxString(bundle.unitsync));
		m_exec_edit->SetValue(TowxString(bundle.spring));
		SlPaths::RefreshSpringVersionList(false, &bundle);
		DoRestore();
	}
}


void SpringOptionsTab::OnApply(wxCommandEvent& /*unused*/)
{
	const std::string index = STD_STRING(m_spring_list->GetStringSelection());
	if (index.empty()) {
		LSL::usync().FreeUnitSyncLib();
		return;
	}

	// Save old values first so we know whether or not to reload unitsync within the process.
	const std::string oldIndex = SlPaths::GetCurrentUsedSpringIndex();
	const std::string oldUnitsync = SlPaths::GetUnitSync(index);

	// Ensure that SlPaths model is consistent with the edit fields
	SlPaths::SetSpringBinary(index, STD_STRING(m_exec_edit->GetValue()));
	SlPaths::SetUnitSync(index, STD_STRING(m_sync_edit->GetValue()));

	// Ensure that the SlPaths model is consistent with the currently selected index
	SlPaths::SetUsedSpringIndex(index);

	// Determine whether to reload unitsync within the process
	const bool same_index = (index == oldIndex);
	const bool same_usync = (SlPaths::GetUnitSync(index) == oldUnitsync);
	const bool reload_usync = !same_index || !same_usync;
	if (!reload_usync) {
		return;
	}

	SwitchUnitsync(index, oldIndex);
}


void SpringOptionsTab::OnRestore(wxCommandEvent& /*unused*/)
{
	DoRestore();
}

void SpringOptionsTab::OnDontSearch(wxCommandEvent& /*unused*/)
{
	EnableSpringBox(true);
	EnableUnitsyncBox(true);
}

void SpringOptionsTab::OnDataDir(wxCommandEvent& /*unused*/)
{
	int result = wxMessageBox(_("Do you want to change spring's datadir location? (select yes only if you know what you're doing)"), _("Data dir wizard"), wxICON_QUESTION | wxYES_NO, &ui().mw());

	if (result != wxYES) {
		return;
	}

	const wxString dir = wxDirSelector(_("Choose a folder"), TowxString(SlPaths::GetDataDir()));
	const bool res = SlPaths::CreateSpringDataDir(STD_STRING(dir));
	if (!res) {
		wxMessageBox(wxString::Format(_("Something went wrong when creating the directory: %s"), dir.c_str()));
		return;
	}
}

void SpringOptionsTab::OnGroupListSelectionChange(wxCommandEvent& /*event*/)
{
	const std::string selection = STD_STRING(m_spring_list->GetStringSelection());
	if (!selection.empty()) {
		DoRestore();
	}
}

void SpringOptionsTab::ReloadSpringList()
{
	std::map<std::string, LSL::SpringBundle> springlist = SlPaths::GetSpringVersionList();
	m_spring_list->Clear();
	for (auto bundle : springlist) {
		m_spring_list->Append(TowxString(bundle.first));
	}
	m_spring_list->SetStringSelection(TowxString(SlPaths::GetCurrentUsedSpringIndex()));
	DoRestore();
}

void SpringOptionsTab::SwitchUnitsync(const std::string& newIndex, const std::string& oldIndex /* can be empty */)
{
	UiEvents::ScopedStatusMessage(_("Reloading unitsync"), 0);
	SlPaths::SetUsedSpringIndex(newIndex);
	if (!LSL::usync().LoadUnitSyncLib(SlPaths::GetUnitSync(newIndex))) { //FIXME: make LoadUnitSyncLib() async (partly done)
		wxLogWarning(_T( "Cannot load UnitSync" ));
		customMessageBox(SL_MAIN_ICON,
				 wxString::Format(_T("%s is unable to load your UnitSync library into the process.\n\nYou might want to take another look at your unitsync setting."), getSpringlobbyName().c_str()),
				 _("Spring error"), wxOK);
		SlPaths::SetUsedSpringIndex(oldIndex);
		DoRestore();
	}
}

void SpringOptionsTab::OnRemoveBundle(wxCommandEvent& /*event*/)
{
	const std::string usedIndex = SlPaths::GetCurrentUsedSpringIndex();
	const std::string index = STD_STRING(m_spring_list->GetStringSelection());
	if (index.empty()) {
		return;
	}
	SlPaths::DeleteSpringVersionbyIndex(index);
	SlPaths::RefreshSpringVersionList(false);
	ReloadSpringList();
	const std::string newIndex = STD_STRING(m_spring_list->GetStringSelection());
}
