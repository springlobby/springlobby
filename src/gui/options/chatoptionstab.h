/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_CHATOPTIONSTAB_H
#define SPRINGLOBBY_HEADERGUARD_CHATOPTIONSTAB_H

#include <wx/scrolwin.h>

class wxStaticBoxSizer;
class wxStaticBox;
class wxStaticText;
class wxRadioButton;
class wxButton;
class wxTextCtrl;
class wxBoxSizer;
class wxCheckBox;
class Ui;
class ColorButton;
class wxSpinCtrl;


class ChatOptionsTab : public wxPanel
{
public:
	ChatOptionsTab(wxWindow* parent);
	~ChatOptionsTab();

	void OnApply(wxCommandEvent& event);
	void OnRestore(wxCommandEvent& event);

private:
	void DoRestore();


	void OnSelectFont(wxCommandEvent& event);
	void OnNormalSelect(wxCommandEvent& event);
	void OnBGSelect(wxCommandEvent& event);
	void OnActionSelect(wxCommandEvent& event);
	void OnHighlightSelect(wxCommandEvent& event);
	void OnJoinLeaveSelect(wxCommandEvent& event);
	void OnNotificationSelect(wxCommandEvent& event);
	void OnMyMessSelect(wxCommandEvent& event);
	void OnServerSelect(wxCommandEvent& event);
	void OnClientSelect(wxCommandEvent& event);
	void OnErrorSelect(wxCommandEvent& event);
	void OnTimestampSelect(wxCommandEvent& event);
	void OnSaveLogs(wxCommandEvent& event);

	void UpdateTextSample();
	void UpdateFontLabel();

	enum {
		ID_SELFONT = 1000,
		ID_SYSCOLS,
		ID_NORMAL,
		ID_BG,
		ID_ACTION,
		ID_HIGHLIGHT,
		ID_JOINLEAVE,
		ID_NOTIFICATION,
		ID_MYMESS,
		ID_SERVER,
		ID_CLIENT,
		ID_ERROR,
		ID_TIMESTAMP,
		ID_HIWORDS,
		ID_PLAY_SOUNDS,
		ID_HL_REQ
	};

	//    wxStaticText* m_text_sample;
	wxStaticText* m_fontname;
	wxStaticText* m_font_label;
	wxButton* m_select_font;
	wxPanel* m_custom_colors;
	ColorButton* m_normal_color;
	wxStaticText* m_normal_label;
	ColorButton* m_bg_color;
	wxStaticText* m_bg_label;
	ColorButton* m_action_color;
	wxStaticText* m_action_label;
	ColorButton* m_highlight_color;
	wxStaticText* m_highlight_label;
	ColorButton* m_joinleave_color;
	wxStaticText* m_joinleave_label;
	ColorButton* m_note_color;
	wxStaticText* m_note_label;
	ColorButton* m_my_color;
	wxStaticText* m_my_label;
	ColorButton* m_server_color;
	wxStaticText* m_server_label;
	ColorButton* m_client_color;
	wxStaticText* m_client_label;
	ColorButton* m_error_color;
	wxStaticText* m_error_label;
	ColorButton* m_ts_color;
	wxStaticText* m_ts_label;
	wxTextCtrl* m_test_text;
	wxCheckBox* m_save_logs;
	//    wxStaticText* m_chat_save_label;
	//    wxTextCtrl* m_log_save;
	//    wxButton* m_browse_log;
	wxStaticText* m_hilight_words_label;
	wxCheckBox* m_play_sounds;
	wxCheckBox* m_highlight_req;
	wxCheckBox* m_broadcast_check;

	wxTextCtrl* m_highlight_words;

	wxCheckBox* m_irc_colors;

	wxStaticText* m_num_lines_lbl;
	wxSpinCtrl* m_num_lines;

	wxFont m_chat_font;

	/// generic func used in all handlers
	void OnColorChange(ColorButton* button);

	DECLARE_EVENT_TABLE()
};

// enum
// {
//     SPRING_DIRBROWSE = wxID_HIGHEST,
//     SPRING_EXECBROWSE,
//     SPRING_SYNCBROWSE,
//     SPRING_WEBBROWSE,
//     SPRING_DEFEXE,
//     SPRING_DEFUSYNC,
//     SPRING_DEFWEB,
//     SPRING_AUTOCONF,
//     SPRING_DIRFIND,
//     SPRING_EXECFIND,
//     SPRING_SYNCFIND,
//
//     //Chat Log
//     CHATLOG_ENABLE,
//
// };

#endif // SPRINGLOBBY_HEADERGUARD_CHATOPTIONSTAB_H
