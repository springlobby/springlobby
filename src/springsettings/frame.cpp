/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/notebook.h>
#include <wx/menu.h>
#include <wx/icon.h>

#include "frame.h"
#include "settings.h"
#include "gui/mainwindow.h"
#include "tab_render_detail.h"
#include "tab_quality_video.h"
#include "tab_abstract.h"
#include "tab_audio.h"
//#include "hotkeys/hotkey_panel.h"
#include "tab_ui.h"
#include "tab_simple.h"
#include "ctrlconstants.h"
#include "gui/customdialogs.h"
#include "gui/controls.h"
#include "images/springsettings.xpm"
#include "utils/platform.h"
#include "utils/version.h"
#include "utils/conversion.h"
#include "gui/aboutbox.h"

#include "gui/mainwindow.h"

const wxString simpleTabCap = _("Combined Options");
const wxString qualityTabCap = _("Render quality / Video mode");
const wxString detailTabCap = _("Render detail");
const wxString uiTabCap = _("UI options");
const wxString audioTabCap = _("Audio");
const wxString hotkeyTabCap = _("Hotkeys");
const wxString expertModeWarning = _("Changes made on Quality/Detail tab in expert mode\n"
				     "will be lost if you change simple options again.\n"
				     "Also these changes WILL NOT be reflected by the \n"
				     "selected choices on the Combined options tab.\n"
				     "(this message can be disabled in the \"File\" menu)");

BEGIN_EVENT_TABLE(settings_frame, wxFrame)
EVT_CLOSE(settings_frame::OnClose)
EVT_MENU(wxID_ANY, settings_frame::OnMenuChoice)
EVT_SET_FOCUS(settings_frame::OnSetFocus)
EVT_KILL_FOCUS(settings_frame::OnKillFocus)
END_EVENT_TABLE()

settings_frame::settings_frame(wxWindow* parent, const wxString& title, wxWindowID id)
    : wxFrame(parent, id, title)
    , WindowAttributesPickle(_T("SETTINGSFRAME"), this, wxSize(DEFSETT_SW_WIDTH, DEFSETT_SW_HEIGHT))
    , simpleTab(0)
    , uiTab(0)
    , audioTab(0)
    , detailTab(0)
    , qualityTab(0)
    ,
    //	hotkeyTab(0),
    settingsIcon(new wxIcon(springsettings_xpm))
    , m_has_focus(true)
{
	SetMinSize(wxSize(DEFSETT_SW_WIDTH, DEFSETT_SW_HEIGHT));

	alreadyCalled = false;
	parentWindow = parent;

	notebook = new wxNotebook(this, ID_OPTIONS);
	//	notebook->SetFont(wxFont(8, wxSWISS, wxNORMAL,wxNORMAL, false, _T("Tahoma")));


	if (abstract_panel::loadValuesIntoMap()) {
		CreateGUIControls();
		initMenuBar();
	}

	//Dialog just created, no setting were changed yet
	abstract_panel::settingsChanged = false;

	Layout();
	Center();

	if (!parentWindow)
		UpdateMainAppHasFocus(m_has_focus); // only do if not being a slave of main SL app
}

void settings_frame::buildGuiFromErrorPanel()
{
	notebook->DeletePage(0);

	//to be safe we'll try again
	if (abstract_panel::loadValuesIntoMap()) {
		CreateGUIControls();
		initMenuBar();
	}
}

settings_frame::~settings_frame()
{
}

void settings_frame::OnSetFocus(wxFocusEvent&)
{
	m_has_focus = true;
	UpdateMainAppHasFocus(m_has_focus);
}

void settings_frame::OnKillFocus(wxFocusEvent&)
{
	m_has_focus = false;
	UpdateMainAppHasFocus(m_has_focus);
}

void settings_frame::handleExternExit()
{
	if (alreadyCalled) {
		return;
	}
	alreadyCalled = true;

	if (!settingsChangedAbstract()) {
		return;
	}

	const int choice = customMessageBox(SS_MAIN_ICON, _("Save Spring settings before exiting?"), _("Confirmation needed"), wxYES | wxNO | wxICON_QUESTION);
	if (choice == wxYES) {
		saveSettingsAbstract();
		if (simpleTab != 0) {
			simpleTab->saveCbxChoices();
		}
		sett().SaveSettings();
	}
}

void settings_frame::doQuit()
{
	sett().SaveSettings();
	Destroy();
	//we can only compile this in non-standalone
	MainWindow* m = dynamic_cast<MainWindow*>(m_parent);
	if (m) {
		m->se_frame_active = false;
	}
}

void settings_frame::handleExit()
{
	if (settingsChangedAbstract()) {
		int action = customMessageBox(SS_MAIN_ICON, _("Save Spring settings before exiting?"), _("Confirmation needed"), wxYES_NO | wxCANCEL | wxICON_QUESTION);
		switch (action) {
			case wxYES:
				if (saveSettingsAbstract())
					(abstract_panel::settingsChanged) = false;
				if (simpleTab != 0)
					simpleTab->saveCbxChoices();
				doQuit();
				return;
			case wxNO:
				doQuit();
				return;

			case wxCANCEL:
			default:
				return;
		}
	} else {
		doQuit();
	}
}

void settings_frame::CreateGUIControls()
{
	switch (sett().getMode()) {
		case SET_MODE_EXPERT:

			qualityTab = new tab_quality_video(notebook, ID_QUALITY_VIDEO);
			detailTab = new tab_render_detail(notebook, ID_RENDER_DETAIL);
			uiTab = new tab_ui(notebook, ID_UI);
			audioTab = new audio_panel(notebook, ID_AUDIO);
			//                                hotkeyTab = new hotkey_panel(notebook, ID_HOTKEY);
			simpleTab = 0;
			notebook->AddPage(uiTab, uiTabCap);
			notebook->AddPage(qualityTab, qualityTabCap);
			notebook->AddPage(detailTab, detailTabCap);
			notebook->AddPage(audioTab, audioTabCap);
			//								notebook->AddPage(hotkeyTab,hotkeyTabCap);
			break;
		default:
		case SET_MODE_SIMPLE:
			simpleTab = new tab_simple(this, notebook, ID_SIMPLE);
			uiTab = new tab_ui(notebook, ID_UI);
			notebook->AddPage(simpleTab, simpleTabCap);
			notebook->AddPage(uiTab, uiTabCap);
			SetTitle(_("SpringSettings (simple mode)"));
			break;
	}
	notebook->SetSelection(0);

	abstract_panel::settingsChanged = false;
}

void settings_frame::initMenuBar()
{
	menuFile = new wxMenu();

	menuFile->Append(ID_MENUITEM_SAVE, _("Save settings"));
	menuFile->Append(ID_MENUITEM_RESET, _("Reset settings to default values"));
	menuFile->AppendCheckItem(ID_MENUITEM_DISABLE_WARN, _("Disable expert mode warning"));
	menuFile->AppendSeparator();
	menuFile->Append(ID_MENUITEM_QUIT, _("Quit"));

	menuFile->Check(ID_MENUITEM_DISABLE_WARN, sett().getDisableWarning());

	menuMode = new wxMenu();
	menuMode->AppendRadioItem(ID_MENUITEM_SIMPLE, _("Simple (few options)"));
	menuMode->AppendRadioItem(ID_MENUITEM_EXPERT, _("Expert (all options"));

	menuHelp = new wxMenu();
	menuHelp->Append(ID_MENUITEM_ABOUT, _("About"));
	menuHelp->Append(ID_MENUITEM_CONTACT, _("Contact"));
	menuHelp->Append(ID_MENUITEM_CREDITS, _("Credits"));
	menuHelp->Append(ID_MENUITEM_BUGREPORT, _("Report a bug"));

	switch (sett().getMode()) {
		case SET_MODE_EXPERT: {
			menuMode->Check(ID_MENUITEM_EXPERT, true);
		} break;
		default:
		case SET_MODE_SIMPLE: {
			menuMode->Check(ID_MENUITEM_SIMPLE, true);
		} break;
	}
	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(menuFile, _("File"));
	menuBar->Append(menuMode, _("Mode"));
	menuBar->Append(menuHelp, _("Info/Help"));

	//TODO PROFILES!!!

	SetMenuBar(menuBar);
}

void settings_frame::OnMenuChoice(wxCommandEvent& event)
{
	switch (event.GetId()) {
		case ID_MENUITEM_SAVE:
			if (saveSettingsAbstract())
				(abstract_panel::settingsChanged) = false;
			if (simpleTab != 0)
				simpleTab->saveCbxChoices();
			break;

		case ID_MENUITEM_QUIT:
			handleExit();
			break;

		case ID_MENUITEM_RESET:
			if ((customMessageBox(SS_MAIN_ICON, _("Reset ALL settings to default values?"), _("Confirmation needed"), wxYES_NO | wxCANCEL)) == wxYES) {
				resetSettings();
			}
			break;

		case ID_MENUITEM_SIMPLE:
			if (sett().getMode() == SET_MODE_EXPERT) {
				sett().setMode(SET_MODE_SIMPLE);

				simpleTab = new tab_simple(this, notebook, ID_SIMPLE);
				notebook->InsertPage(0, simpleTab, simpleTabCap);
				simpleTab->updateControls(UPDATE_ALL);

				//if not on ui page goto simple
				if (notebook->GetSelection() != 1) {
					notebook->SetSelection(0);
				}
				for (int i = notebook->GetPageCount() - 1; i > 1; i--) { //delete all pages expect page 0 + 1
					notebook->DeletePage(i);
				}
				qualityTab = 0;
				detailTab = 0;
				audioTab = 0;
				//				hotkeyTab = 0;
				SetTitle(TowxString(getSpringlobbyName()) + _("(simple mode)"));
				if (!sett().getDisableWarning()) {
					customMessageBox(SS_MAIN_ICON, expertModeWarning, _("Hint"), wxOK);
				}
			}
			break;

		case ID_MENUITEM_EXPERT:
			if (sett().getMode() == SET_MODE_SIMPLE) {
				switchToExpertMode();
			}
			break;
		case ID_MENUITEM_DISABLE_WARN:
			sett().setDisableWarning(menuFile->IsChecked(ID_MENUITEM_DISABLE_WARN));
			break;

		case ID_MENUITEM_ABOUT:
			aboutbox().Show();
			break;
		case ID_MENUITEM_CONTACT:
			aboutbox().openContactPage();
			break;
		case ID_MENUITEM_BUGREPORT:
			aboutbox().openNewTicket();
			break;
	}
}
void settings_frame::resetSettings()
{
	abstract_panel::loadDefaults();
	updateAllControls();
}

void settings_frame::switchToExpertMode()
{
	sett().setMode(SET_MODE_EXPERT);
	menuMode->Check(ID_MENUITEM_EXPERT, true);

	qualityTab = new tab_quality_video(notebook, ID_QUALITY_VIDEO);
	detailTab = new tab_render_detail(notebook, ID_RENDER_DETAIL);
	audioTab = new audio_panel(notebook, ID_AUDIO);
	//	hotkeyTab = new hotkey_panel(notebook,ID_HOTKEY);
	notebook->AddPage(qualityTab, qualityTabCap);
	notebook->AddPage(detailTab, detailTabCap);
	notebook->AddPage(audioTab, audioTabCap);
	//	notebook->AddPage(hotkeyTab,hotkeyTabCap);

	notebook->DeletePage(0);
	simpleTab = 0;
	SetTitle(TowxString(getSpringlobbyName()) + _("(expert mode)"));
	/*uiTab->updateControls(UPDATE_ALL);
	detailTab->updateControls(UPDATE_ALL);
	qualityTab->updateControls(UPDATE_ALL);
	audioTab->updateControls(UPDATE_ALL);
	hotkeyTab->UpdateControls(UPDATE_ALL);*/
}

void settings_frame::updateAllControls()
{
	if (uiTab)
		uiTab->updateControls(UPDATE_ALL);
	if (simpleTab != 0)
		simpleTab->updateControls(UPDATE_ALL);
	if (detailTab)
		detailTab->updateControls(UPDATE_ALL);
	if (qualityTab)
		qualityTab->updateControls(UPDATE_ALL);
	if (audioTab)
		audioTab->updateControls(UPDATE_ALL);
	//	if (hotkeyTab)
	//		hotkeyTab->UpdateControls(UPDATE_ALL);
}
void settings_frame::OnClose(wxCloseEvent& /*unused*/)
{
	if (!alreadyCalled) {
		handleExit();
	}
}

bool settings_frame::saveSettingsAbstract()
{
	//	if ( hotkeyTab )
	//		hotkeyTab->SaveSettings();

	return abstract_panel::saveSettings();
}

bool settings_frame::settingsChangedAbstract()
{
	bool rc = false;

	//	rc |= ( hotkeyTab && hotkeyTab->HasProfileBeenModifiedOrSelected() );
	rc |= abstract_panel::settingsChanged;

	return rc;
}
