/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "groupoptionspanel.h"

#include <wx/textdlg.h>
#include <wx/colour.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/listbox.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/statbox.h>

#include "useractions.h"
#include "settings.h"
#include "gui/uiutils.h"
#include "gui/controls.h"
#include "gui/colorbutton.h"
#include "gui/selectusersdialog.h"

BEGIN_EVENT_TABLE(GroupOptionsPanel, wxPanel)
EVT_BUTTON(REMOVE_GROUP, GroupOptionsPanel::OnRemoveGroup)
EVT_BUTTON(RENAME_GROUP, GroupOptionsPanel::OnRenameGroup)
EVT_BUTTON(ADD_GROUP, GroupOptionsPanel::OnAddNewGroup)
EVT_LISTBOX(GROUPS_LIST, GroupOptionsPanel::OnGroupListSelectionChange)
EVT_CHECKBOX(NOTIFY_LOGIN, GroupOptionsPanel::OnGroupActionsChange)
EVT_CHECKBOX(IGNORE_CHAT, GroupOptionsPanel::OnGroupActionsChange)
EVT_CHECKBOX(NOTIFY_HOST, GroupOptionsPanel::OnGroupActionsChange)
EVT_CHECKBOX(IGNORE_PM, GroupOptionsPanel::OnGroupActionsChange)
EVT_CHECKBOX(NOTIFY_STATUS, GroupOptionsPanel::OnGroupActionsChange)
EVT_CHECKBOX(AUTOCKICK, GroupOptionsPanel::OnGroupActionsChange)
EVT_CHECKBOX(NOTIFY_HIGHLIGHT, GroupOptionsPanel::OnGroupActionsChange)
EVT_BUTTON(HIGHLIGHT_COLOR, GroupOptionsPanel::OnHighlightColorClick)
EVT_LISTBOX(USERS_LIST, GroupOptionsPanel::OnUsersListSelectionChange)
EVT_BUTTON(ADD_USER, GroupOptionsPanel::OnAddUsers)
EVT_BUTTON(REMOVE_USER, GroupOptionsPanel::OnRemoveUser)
END_EVENT_TABLE()

GroupOptionsPanel::GroupOptionsPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* groupListSizer;
	groupListSizer = new wxBoxSizer(wxVERTICAL);

	m_group_list = new wxListBox(this, GROUPS_LIST, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_ALWAYS_SB | wxLB_SINGLE | wxLB_SORT);
	groupListSizer->Add(m_group_list, 1, wxEXPAND | wxTOP | wxRIGHT | wxLEFT, 5);

	wxBoxSizer* groupListButtonsSizer;
	groupListButtonsSizer = new wxBoxSizer(wxHORIZONTAL);

	m_remove_group_button = new wxButton(this, REMOVE_GROUP, _("Remove"), wxDefaultPosition, wxSize(-1, -1), wxBU_EXACTFIT);
	m_remove_group_button->Enable(false);
	m_remove_group_button->SetToolTip(_("Remove an existing group"));

	groupListButtonsSizer->Add(m_remove_group_button, 0, wxTOP | wxRIGHT | wxLEFT, 5);

	m_rename_group_button = new wxButton(this, RENAME_GROUP, _("Rename.."), wxDefaultPosition, wxSize(-1, -1), wxBU_EXACTFIT);
	m_rename_group_button->Enable(false);
	m_rename_group_button->SetToolTip(_("Rename an existing group"));

	groupListButtonsSizer->Add(m_rename_group_button, 0, wxTOP | wxRIGHT | wxLEFT, 5);


	groupListButtonsSizer->Add(0, 0, 1, wxEXPAND, 5);

	m_add_group_button = new wxButton(this, ADD_GROUP, _("Add New.."), wxDefaultPosition, wxSize(-1, -1), wxBU_EXACTFIT);
	m_add_group_button->SetToolTip(_("Add new group"));

	groupListButtonsSizer->Add(m_add_group_button, 0, wxTOP | wxRIGHT | wxLEFT, 5);

	groupListSizer->Add(groupListButtonsSizer, 0, wxEXPAND | wxBOTTOM, 5);

	mainSizer->Add(groupListSizer, 2, wxEXPAND, 5);

	m_group_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxTAB_TRAVERSAL);
	wxBoxSizer* mainGroupPanelSizer;
	mainGroupPanelSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* actionsSizer;
	actionsSizer = new wxStaticBoxSizer(new wxStaticBox(m_group_panel, wxID_ANY, _("Group Actions")), wxVERTICAL);

	wxGridSizer* actionCheckSizer;
	actionCheckSizer = new wxGridSizer(0, 2, 0, 0);

	m_login_notify_check = new wxCheckBox(m_group_panel, NOTIFY_LOGIN, _("Notify login/logout"), wxDefaultPosition, wxDefaultSize, 0);

	m_login_notify_check->SetToolTip(_("Notify when users of this group go online or offline"));

	actionCheckSizer->Add(m_login_notify_check, 0, wxTOP | wxRIGHT | wxLEFT, 5);

	m_ignore_chat_check = new wxCheckBox(m_group_panel, IGNORE_CHAT, _("Ignore Chat"), wxDefaultPosition, wxDefaultSize, 0);

	m_ignore_chat_check->SetToolTip(_("Ignore anything said in channel by any of the users in this group"));

	actionCheckSizer->Add(m_ignore_chat_check, 0, wxTOP | wxRIGHT | wxLEFT, 5);

	m_notify_host_check = new wxCheckBox(m_group_panel, NOTIFY_HOST, _("Notify Hosted Battles"), wxDefaultPosition, wxDefaultSize, 0);

	m_notify_host_check->SetToolTip(_("Notify when users of this group hosts a battle"));

	actionCheckSizer->Add(m_notify_host_check, 0, wxTOP | wxRIGHT | wxLEFT, 5);

	m_ignore_pm_check = new wxCheckBox(m_group_panel, IGNORE_PM, _("Ignore PM"), wxDefaultPosition, wxDefaultSize, 0);

	m_ignore_pm_check->SetToolTip(_("Ignore anything said in private chat by any of the users in this group"));

	actionCheckSizer->Add(m_ignore_pm_check, 0, wxTOP | wxRIGHT | wxLEFT, 5);

	m_notify_status_check = new wxCheckBox(m_group_panel, NOTIFY_STATUS, _("Notify Status Change"), wxDefaultPosition, wxDefaultSize, 0);

	m_notify_status_check->SetToolTip(_("Notify when the status of a users in this group changes"));

	actionCheckSizer->Add(m_notify_status_check, 0, wxTOP | wxRIGHT | wxLEFT, 5);

	m_autokick_check = new wxCheckBox(m_group_panel, AUTOCKICK, _("Autokick"), wxDefaultPosition, wxDefaultSize, 0);

	m_autokick_check->SetToolTip(_("Auto kick any of the users in this group from battles hosted"));

	actionCheckSizer->Add(m_autokick_check, 0, wxALL, 5);

	m_highlight_check = new wxCheckBox(m_group_panel, NOTIFY_HIGHLIGHT, _("Highlight"), wxDefaultPosition, wxDefaultSize, 0);

	m_highlight_check->SetToolTip(_("Highlight battles and the names of users in this group"));

	actionCheckSizer->Add(m_highlight_check, 0, wxTOP | wxRIGHT | wxLEFT, 5);

	wxBoxSizer* colorSizer;
	colorSizer = new wxBoxSizer(wxHORIZONTAL);

	m_highlight_colorstaticText = new wxStaticText(m_group_panel, wxID_ANY, _("Highlight Color"), wxDefaultPosition, wxDefaultSize, 0);
	m_highlight_colorstaticText->Wrap(-1);
	colorSizer->Add(m_highlight_colorstaticText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_highlight_color_button = new ColorButton(m_group_panel, HIGHLIGHT_COLOR, wxColour(0, 0, 0), wxDefaultPosition, wxSize(20, 20));
	m_highlight_color_button->SetToolTip(_("Select highlight color"));

	m_highlight_color_button->SetToolTip(_("Select highlight color"));

	colorSizer->Add(m_highlight_color_button, 0, wxALL, 5);

	actionCheckSizer->Add(colorSizer, 1, wxEXPAND, 5);

	actionsSizer->Add(actionCheckSizer, 1, wxEXPAND, 5);

	mainGroupPanelSizer->Add(actionsSizer, 0, wxEXPAND | wxBOTTOM | wxRIGHT | wxLEFT, 5);

	wxStaticBoxSizer* usersSizer;
	usersSizer = new wxStaticBoxSizer(new wxStaticBox(m_group_panel, wxID_ANY, _("Users in group")), wxVERTICAL);

	wxBoxSizer* userListSizer;
	userListSizer = new wxBoxSizer(wxHORIZONTAL);

	m_user_list = new wxListBox(m_group_panel, USERS_LIST, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_MULTIPLE);
	userListSizer->Add(m_user_list, 1, wxALL | wxEXPAND, 5);

	wxBoxSizer* userButtonSizer;
	userButtonSizer = new wxBoxSizer(wxVERTICAL);

	m_add_user_button = new wxButton(m_group_panel, ADD_USER, _("Add.."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_add_user_button->SetToolTip(_("Add users to group"));

	userButtonSizer->Add(m_add_user_button, 0, wxEXPAND | wxTOP | wxRIGHT, 5);

	m_remove_user_button = new wxButton(m_group_panel, REMOVE_USER, _("Remove"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_remove_user_button->Enable(false);
	m_remove_user_button->SetToolTip(_("Remove users from group"));

	userButtonSizer->Add(m_remove_user_button, 0, wxEXPAND | wxTOP | wxRIGHT, 5);

	userListSizer->Add(userButtonSizer, 0, wxEXPAND, 5);

	usersSizer->Add(userListSizer, 1, wxEXPAND, 5);

	mainGroupPanelSizer->Add(usersSizer, 1, wxEXPAND | wxBOTTOM | wxRIGHT | wxLEFT, 5);

	m_group_panel->SetSizer(mainGroupPanelSizer);
	m_group_panel->Layout();
	mainGroupPanelSizer->Fit(m_group_panel);
	mainSizer->Add(m_group_panel, 3, wxALL | wxEXPAND, 5);

	this->SetSizer(mainSizer);
	this->Layout();

	Initialize();
}

void GroupOptionsPanel::Initialize()
{
	m_user_dialog = 0;
	ReloadGroupsList();
	ShowGroup(wxEmptyString);
}

GroupOptionsPanel::~GroupOptionsPanel()
{
        delete this->GetSizer();
}

void GroupOptionsPanel::ShowGroup(const wxString& group)
{
	if (group == wxEmptyString)
		m_current_group = GetFirstGroupName();
	else
		m_current_group = group;

	m_group_list->SetStringSelection(m_current_group);

	UserActions::ActionType act = useractions().GetGroupAction(m_current_group);

	m_login_notify_check->SetValue((act & UserActions::ActNotifLogin) != 0);
	m_ignore_chat_check->SetValue((act & UserActions::ActIgnoreChat) != 0);
	m_notify_host_check->SetValue((act & UserActions::ActNotifBattle) != 0);
	m_ignore_pm_check->SetValue((act & UserActions::ActIgnorePM) != 0);
	m_notify_status_check->SetValue((act & UserActions::ActNotifStatus) != 0);
	m_autokick_check->SetValue((act & UserActions::ActAutokick) != 0);
	m_highlight_check->SetValue((act & UserActions::ActHighlight) != 0);

	m_highlight_color_button->SetColor(useractions().GetGroupColor(m_current_group));

	ReloadUsersList();
	m_remove_group_button->Enable(m_group_list->GetStringSelection() != wxEmptyString);
}

void GroupOptionsPanel::ReloadUsersList()
{
	wxArrayString groupuser = useractions().GetPeopleList(m_current_group);
	m_user_list->Clear();
	m_user_list->Append(groupuser);
	m_remove_user_button->Enable(false);
}

void GroupOptionsPanel::ReloadGroupsList()
{
	wxArrayString groupnames = useractions().GetGroupNames();
	m_group_list->Clear();
	m_group_list->Append(groupnames);
	m_group_list->SetStringSelection(m_current_group);
}

wxString GroupOptionsPanel::GetFirstGroupName()
{
	wxSortedArrayString groupnames = useractions().GetGroupNames();
	if (groupnames.Count() <= 0)
		return wxEmptyString;
	return groupnames[0];
}

void GroupOptionsPanel::OnRemoveGroup(wxCommandEvent& /*unused*/)
{
	if (m_current_group == _T("Default"))
		return;
	useractions().DeleteGroup(m_current_group);
	ReloadGroupsList();
	ShowGroup(wxEmptyString);
}


void GroupOptionsPanel::OnRenameGroup(wxCommandEvent& /*unused*/)
{
}


void GroupOptionsPanel::OnAddNewGroup(wxCommandEvent& /*unused*/)
{
	wxTextEntryDialog* ted = new wxTextEntryDialog(this, _("Name of new group:"), _("Add New Group"));
	if (ted->ShowModal() == wxID_OK) {
		wxString newgroup = ted->GetValue();
		//!TODO: Check if group exists already.
		if (newgroup != wxEmptyString)
			useractions().AddGroup(newgroup);
		ReloadGroupsList();
		ShowGroup(newgroup);
	}
	delete ted;
}


void GroupOptionsPanel::OnGroupListSelectionChange(wxCommandEvent& /*unused*/)
{
	wxString newgroup = m_group_list->GetStringSelection();
	wxSortedArrayString groupnames = useractions().GetGroupNames();
	if (groupnames.Index(newgroup) == wxNOT_FOUND) {
		return;
	}
	ShowGroup(newgroup);
}


void GroupOptionsPanel::OnGroupActionsChange(wxCommandEvent&)
{
	useractions().ChangeAction(m_current_group, UserActions::ActNotifLogin, m_login_notify_check->GetValue());
	useractions().ChangeAction(m_current_group, UserActions::ActIgnoreChat, m_ignore_chat_check->GetValue());
	useractions().ChangeAction(m_current_group, UserActions::ActNotifBattle, m_notify_host_check->GetValue());
	useractions().ChangeAction(m_current_group, UserActions::ActIgnorePM, m_ignore_pm_check->GetValue());
	useractions().ChangeAction(m_current_group, UserActions::ActNotifStatus, m_notify_status_check->GetValue());
	useractions().ChangeAction(m_current_group, UserActions::ActAutokick, m_autokick_check->GetValue());
	useractions().ChangeAction(m_current_group, UserActions::ActHighlight, m_highlight_check->GetValue());
}


void GroupOptionsPanel::OnHighlightColorClick(wxCommandEvent& event)
{
	ColorButton* origin = (ColorButton*)event.GetEventObject();
	wxColour c = GetColourFromUser(this, origin->GetColor());
	if (c.IsOk()) {
		origin->SetColor(c);
		useractions().SetGroupColor(m_current_group, c);
	}
}


void GroupOptionsPanel::OnUsersListSelectionChange(wxCommandEvent& /*unused*/)
{
	wxArrayInt sel;
	m_user_list->GetSelections(sel);
	m_remove_user_button->Enable(sel.Count() > 0);
}


void GroupOptionsPanel::OnAddUsers(wxCommandEvent& /*unused*/)
{
	wxSortedArrayString users = SelectUsersDialog::GetUsers(this);
	for (unsigned int i = 0; i < users.Count(); i++) {
		useractions().AddUserToGroup(m_current_group, users[i]);
	}
	if (users.Count() > 0)
		ReloadUsersList();
}


void GroupOptionsPanel::OnRemoveUser(wxCommandEvent& /*unused*/)
{
	wxArrayInt sel;
	int num = m_user_list->GetSelections(sel);
	for (int i = 0; i < num; i++) {
		wxString name = m_user_list->GetString(sel[i]);
		useractions().RemoveUser(name);
	}
	ReloadUsersList();
}


void GroupOptionsPanel::Update()
{
	ReloadGroupsList();
	ReloadUsersList();
}
