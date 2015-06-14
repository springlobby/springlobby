/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_MAINOPTIONSTAB_H
#define SPRINGLOBBY_HEADERGUARD_MAINOPTIONSTAB_H

#include <wx/scrolwin.h>
#include <wx/dialog.h>
#include <gui/windowattributespickle.h>

class Ui;
class wxCommandEvent;
class wxBoxSizer;
class wxImageList;
class wxNotebook;
class SLNotebook;
class SpringOptionsTab;
class ChatOptionsTab;
class wxButton;
class wxBoxSizer;
class DownloadOptionsPanel;
class GroupOptionsPanel;
class LobbyOptionsTab;

/** \brief A container for the various option panels
 * Contains a notebook holding the real option panels as pages. Handles "apply" and "restore" events for those pages,
 * rather then those having to implement (and duplicate) this functionality. \n
 * See SpringOptionsTab, TorrentOptionsPanel, ChatOptionsTab, LobbyOptionsTab
 */
class MainOptionsTab : public wxScrolledWindow
{
public:
	MainOptionsTab(wxWindow* parent);
	~MainOptionsTab();

	/** \brief delegate the data setting to memeber panels */
	void OnApply(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);
	void OnRestore(wxCommandEvent& event);

	void OnOpenGroupsTab();
	void SetSelection(const unsigned int page);
	GroupOptionsPanel& GetGroupOptionsPanel();

	void LoadPerspective(const wxString& perspective_name = wxEmptyString);
	void SavePerspective(const wxString& perspective_name = wxEmptyString);

private:
	wxBoxSizer* m_main_sizer;

	//		wxImageList* m_imagelist;

	SLNotebook* m_tabs;

	SpringOptionsTab* m_spring_opts;
	ChatOptionsTab* m_chat_opts;
	DownloadOptionsPanel* m_download_opts;
	GroupOptionsPanel* m_groups_opts;
	LobbyOptionsTab* m_lobby_opts;

	wxButton* m_apply_btn;
	wxButton* m_cancel_btn;
	wxButton* m_ok_btn;

	wxBoxSizer* m_button_sizer;

	enum {
		OPTIONS_TABS = wxID_HIGHEST
	};

private:
	wxWindow* frame;

	DECLARE_EVENT_TABLE()
};

class OptionsDialog : public wxDialog, public WindowAttributesPickle
{
public:
	OptionsDialog(wxWindow* parent);
	void SetSelection(const unsigned int page);
	bool Show(bool show = true);

private:
	MainOptionsTab* m_options;
	wxBoxSizer* m_main_sizer;
};


#endif // SPRINGLOBBY_HEADERGUARD_MAINOPTIONSTAB_H
