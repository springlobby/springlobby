/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: ChatOptionsTab
//

#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/intl.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/msgdlg.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/stdpaths.h>
#include <wx/settings.h>
#include <wx/colordlg.h>
#include <wx/fontdlg.h>
#include <wx/checkbox.h>
#include <wx/tokenzr.h>

#include "chatoptionstab.h"
#include "gui/controls.h"
#include "gui/uiutils.h"
#include "settings.h"
#include "spring.h"
#include "gui/mainwindow.h"
#include "gui/colorbutton.h"
#include "aui/auimanager.h"
#include "utils/slconfig.h"

BEGIN_EVENT_TABLE(ChatOptionsTab, wxPanel)

EVT_BUTTON(ID_SELFONT, ChatOptionsTab::OnSelectFont)
EVT_BUTTON(ID_NORMAL, ChatOptionsTab::OnNormalSelect)
EVT_BUTTON(ID_BG, ChatOptionsTab::OnBGSelect)
EVT_BUTTON(ID_ACTION, ChatOptionsTab::OnActionSelect)
EVT_BUTTON(ID_HIGHLIGHT, ChatOptionsTab::OnHighlightSelect)
EVT_BUTTON(ID_JOINLEAVE, ChatOptionsTab::OnJoinLeaveSelect)
EVT_BUTTON(ID_NOTIFICATION, ChatOptionsTab::OnNotificationSelect)
EVT_BUTTON(ID_MYMESS, ChatOptionsTab::OnMyMessSelect)
EVT_BUTTON(ID_SERVER, ChatOptionsTab::OnServerSelect)
EVT_BUTTON(ID_CLIENT, ChatOptionsTab::OnClientSelect)
EVT_BUTTON(ID_ERROR, ChatOptionsTab::OnErrorSelect)
EVT_BUTTON(ID_TIMESTAMP, ChatOptionsTab::OnTimestampSelect)

END_EVENT_TABLE()


ChatOptionsTab::ChatOptionsTab(wxWindow* parent)
    : wxScrolledWindow(parent, -1)
{
	GetAui().manager->AddPane(this, wxLEFT, _T( "chatoptionstab" ));

	wxBoxSizer* bMainSizerV;
	bMainSizerV = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sbColorsSizer;
	sbColorsSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, _("Colors and font")), wxHORIZONTAL);

	wxBoxSizer* bColorsVSizer;
	bColorsVSizer = new wxBoxSizer(wxVERTICAL);

	m_custom_colors = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxTAB_TRAVERSAL);
	m_custom_colors->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));

	wxBoxSizer* bCustomColorsSizer;
	bCustomColorsSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bColorSizer;
	bColorSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bColorsSizer1;
	bColorsSizer1 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bNormlColorSizer;
	bNormlColorSizer = new wxBoxSizer(wxHORIZONTAL);

	m_normal_color = new ColorButton(m_custom_colors, ID_NORMAL, wxBitmap(), wxDefaultPosition, wxSize(20, 20), 0);
	m_normal_color->SetColor(wxColour(0, 0, 0));

	bNormlColorSizer->Add(m_normal_color, 0, wxALL, 5);

	m_normal_label = new wxStaticText(m_custom_colors, wxID_ANY, _("Normal"), wxDefaultPosition, wxDefaultSize, 0);
	m_normal_label->Wrap(-1);
	bNormlColorSizer->Add(m_normal_label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	bColorsSizer1->Add(bNormlColorSizer, 0, wxEXPAND, 5);

	wxBoxSizer* bBGColorSizer;
	bBGColorSizer = new wxBoxSizer(wxHORIZONTAL);

	m_bg_color = new ColorButton(m_custom_colors, ID_BG, wxBitmap(), wxDefaultPosition, wxSize(20, 20), 0);
	m_bg_color->SetColor(wxColour(255, 255, 255));

	bBGColorSizer->Add(m_bg_color, 0, wxALL, 5);

	m_bg_label = new wxStaticText(m_custom_colors, wxID_ANY, _("Background"), wxDefaultPosition, wxDefaultSize, 0);
	m_bg_label->Wrap(-1);
	bBGColorSizer->Add(m_bg_label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	bColorsSizer1->Add(bBGColorSizer, 0, wxEXPAND, 5);

	wxBoxSizer* bActionColorSizer;
	bActionColorSizer = new wxBoxSizer(wxHORIZONTAL);

	m_action_color = new ColorButton(m_custom_colors, ID_ACTION, wxBitmap(), wxDefaultPosition, wxSize(20, 20), 0);
	m_action_color->SetColor(wxColour(255, 0, 249));

	bActionColorSizer->Add(m_action_color, 0, wxALL, 5);

	m_action_label = new wxStaticText(m_custom_colors, wxID_ANY, _("Action"), wxDefaultPosition, wxDefaultSize, 0);
	m_action_label->Wrap(-1);
	bActionColorSizer->Add(m_action_label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	bColorsSizer1->Add(bActionColorSizer, 0, wxEXPAND, 5);

	wxBoxSizer* bHighlightColorSizer;
	bHighlightColorSizer = new wxBoxSizer(wxHORIZONTAL);

	m_highlight_color = new ColorButton(m_custom_colors, ID_HIGHLIGHT, wxBitmap(), wxDefaultPosition, wxSize(20, 20), 0);
	m_highlight_color->SetColor(wxColour(255, 0, 44));

	bHighlightColorSizer->Add(m_highlight_color, 0, wxALL, 5);

	m_highlight_label = new wxStaticText(m_custom_colors, wxID_ANY, _("Highlight"), wxDefaultPosition, wxDefaultSize, 0);
	m_highlight_label->Wrap(-1);
	bHighlightColorSizer->Add(m_highlight_label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	bColorsSizer1->Add(bHighlightColorSizer, 0, wxEXPAND, 5);

	wxBoxSizer* bJoinLeaveColorSizer;
	bJoinLeaveColorSizer = new wxBoxSizer(wxHORIZONTAL);

	m_joinleave_color = new ColorButton(m_custom_colors, ID_JOINLEAVE, wxBitmap(), wxDefaultPosition, wxSize(20, 20), 0);
	m_joinleave_color->SetColor(wxColour(24, 255, 0));

	bJoinLeaveColorSizer->Add(m_joinleave_color, 0, wxALL, 5);

	m_joinleave_label = new wxStaticText(m_custom_colors, wxID_ANY, _("Join/Leave"), wxDefaultPosition, wxDefaultSize, 0);
	m_joinleave_label->Wrap(-1);
	bJoinLeaveColorSizer->Add(m_joinleave_label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	bColorsSizer1->Add(bJoinLeaveColorSizer, 0, wxEXPAND, 5);

	wxBoxSizer* bMyColorSizer;
	bMyColorSizer = new wxBoxSizer(wxHORIZONTAL);

	m_my_color = new ColorButton(m_custom_colors, ID_MYMESS, wxBitmap(), wxDefaultPosition, wxSize(20, 20), 0);
	m_my_color->SetColor(wxColour(160, 160, 160));

	bMyColorSizer->Add(m_my_color, 0, wxALL, 5);

	m_my_label = new wxStaticText(m_custom_colors, wxID_ANY, _("My messages"), wxDefaultPosition, wxDefaultSize, 0);
	m_my_label->Wrap(-1);
	bMyColorSizer->Add(m_my_label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	bColorsSizer1->Add(bMyColorSizer, 1, wxEXPAND, 5);

	bColorSizer->Add(bColorsSizer1, 1, wxEXPAND, 5);

	wxBoxSizer* bColorSizer2;
	bColorSizer2 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bServerColorSizer;
	bServerColorSizer = new wxBoxSizer(wxHORIZONTAL);

	m_server_color = new ColorButton(m_custom_colors, ID_SERVER, wxBitmap(), wxDefaultPosition, wxSize(20, 20), 0);
	m_server_color->SetColor(wxColour(255, 189, 0));

	bServerColorSizer->Add(m_server_color, 0, wxALL, 5);

	m_server_label = new wxStaticText(m_custom_colors, wxID_ANY, _("Server"), wxDefaultPosition, wxDefaultSize, 0);
	m_server_label->Wrap(-1);
	bServerColorSizer->Add(m_server_label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	bColorSizer2->Add(bServerColorSizer, 0, wxEXPAND, 5);

	wxBoxSizer* bClientolorSizer;
	bClientolorSizer = new wxBoxSizer(wxHORIZONTAL);

	m_client_color = new ColorButton(m_custom_colors, ID_CLIENT, wxBitmap(), wxDefaultPosition, wxSize(20, 20), 0);
	m_client_color->SetColor(wxColour(255, 189, 0));

	bClientolorSizer->Add(m_client_color, 0, wxALL, 5);

	m_client_label = new wxStaticText(m_custom_colors, wxID_ANY, _("Client"), wxDefaultPosition, wxDefaultSize, 0);
	m_client_label->Wrap(-1);
	bClientolorSizer->Add(m_client_label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	bColorSizer2->Add(bClientolorSizer, 0, wxEXPAND, 5);

	wxBoxSizer* bErrorColorSizer;
	bErrorColorSizer = new wxBoxSizer(wxHORIZONTAL);

	m_error_color = new ColorButton(m_custom_colors, ID_ERROR, wxBitmap(), wxDefaultPosition, wxSize(20, 20), 0);
	m_error_color->SetColor(wxColour(255, 0, 0));

	bErrorColorSizer->Add(m_error_color, 0, wxALL, 5);

	m_error_label = new wxStaticText(m_custom_colors, wxID_ANY, _("Error"), wxDefaultPosition, wxDefaultSize, 0);
	m_error_label->Wrap(-1);
	bErrorColorSizer->Add(m_error_label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	bColorSizer2->Add(bErrorColorSizer, 0, wxEXPAND, 5);

	wxBoxSizer* bTSColorSizer;
	bTSColorSizer = new wxBoxSizer(wxHORIZONTAL);

	m_ts_color = new ColorButton(m_custom_colors, ID_TIMESTAMP, wxBitmap(), wxDefaultPosition, wxSize(20, 20), 0);
	m_ts_color->SetColor(wxColour(160, 160, 160));

	bTSColorSizer->Add(m_ts_color, 0, wxALL, 5);

	m_ts_label = new wxStaticText(m_custom_colors, wxID_ANY, _("Timestamp"), wxDefaultPosition, wxDefaultSize, 0);
	m_ts_label->Wrap(-1);
	bTSColorSizer->Add(m_ts_label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	bColorSizer2->Add(bTSColorSizer, 0, wxEXPAND, 5);

	wxBoxSizer* bNoteColorSizer;
	bNoteColorSizer = new wxBoxSizer(wxHORIZONTAL);

	m_note_color = new ColorButton(m_custom_colors, ID_NOTIFICATION, wxBitmap(), wxDefaultPosition, wxSize(20, 20), 0);
	m_note_color->SetColor(wxColour(255, 191, 0));

	bNoteColorSizer->Add(m_note_color, 0, wxALL, 5);

	m_note_label = new wxStaticText(m_custom_colors, wxID_ANY, _("Notification"), wxDefaultPosition, wxDefaultSize, 0);
	m_note_label->Wrap(-1);
	bNoteColorSizer->Add(m_note_label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	bColorSizer2->Add(bNoteColorSizer, 0, wxEXPAND, 5);

	bColorSizer->Add(bColorSizer2, 1, wxEXPAND, 5);

	bCustomColorsSizer->Add(bColorSizer, 1, wxEXPAND, 5);

	m_test_text = new wxTextCtrl(m_custom_colors, wxID_ANY, _("[19:35] ** Server ** Connected to Server.\n[22:30] <Dude> hi everyone\n[22:30] ** Dude2 joined the channel.\n[22:30] * Dude2 thinks his colors looks nice\n[22:45] <Dude> Dude2: orl?\n[22:46] <Dude2> But could be better, should tweak them some more...\n"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH);
	bCustomColorsSizer->Add(m_test_text, 1, wxALL | wxEXPAND, 5);

	m_custom_colors->SetSizer(bCustomColorsSizer);
	m_custom_colors->Layout();
	bCustomColorsSizer->Fit(m_custom_colors);
	bColorsVSizer->Add(m_custom_colors, 1, wxEXPAND | wxALL, 0);

	wxBoxSizer* bFontNameSizer;
	bFontNameSizer = new wxBoxSizer(wxHORIZONTAL);

	m_font_label = new wxStaticText(this, wxID_ANY, _("Font:"), wxDefaultPosition, wxDefaultSize, 0);
	m_font_label->Wrap(-1);
	bFontNameSizer->Add(m_font_label, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_fontname = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_fontname->Wrap(-1);
	bFontNameSizer->Add(m_fontname, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_select_font = new wxButton(this, ID_SELFONT, _("Select..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	bFontNameSizer->Add(m_select_font, 0, wxALL, 5);

	bColorsVSizer->Add(bFontNameSizer, 0, wxEXPAND, 5);

	sbColorsSizer->Add(bColorsVSizer, 1, wxEXPAND, 5);

	bMainSizerV->Add(sbColorsSizer, 0, wxEXPAND | wxBOTTOM | wxRIGHT | wxLEFT | wxTOP, 5);


	wxStaticBoxSizer* sbBehaviorSizer;
	sbBehaviorSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, _("Behavior")), wxHORIZONTAL);

	m_irc_colors = new wxCheckBox(this, wxID_ANY, _("Enable Irc colors in chat messages"), wxDefaultPosition, wxDefaultSize, 0);
	m_irc_colors->SetValue(sett().GetUseIrcColors());

	sbBehaviorSizer->Add(m_irc_colors, 0, wxALL, 5);
#ifndef DISABLE_SOUND
	m_play_sounds = new wxCheckBox(this, ID_PLAY_SOUNDS, _("Play notification sounds"), wxDefaultPosition, wxDefaultSize, 0);
	m_play_sounds->SetValue(sett().GetChatPMSoundNotificationEnabled());
	sbBehaviorSizer->Add(m_play_sounds, 0, wxALL, 5);
#endif

	m_broadcast_check = new wxCheckBox(this, wxID_ANY, _("Copy server messages in current channel"), wxDefaultPosition, wxDefaultSize, 0);
	m_broadcast_check->SetValue(cfg().ReadBool(_T("/Chat/BroadcastEverywhere")));
	sbBehaviorSizer->Add(m_broadcast_check, 0, wxALL, 5);

	bMainSizerV->Add(sbBehaviorSizer, 0, wxEXPAND | wxBOTTOM | wxRIGHT | wxLEFT, 5);

	wxBoxSizer* bBotomSizer;
	bBotomSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBoxSizer* sbChatLogSizer;
	wxStaticBox* sbChatLog = new wxStaticBox(this, -1, _("Chat logs"));
	sbChatLogSizer = new wxStaticBoxSizer(sbChatLog, wxVERTICAL);

	m_save_logs = new wxCheckBox(this, wxID_ANY, _("Save chat logs"), wxDefaultPosition, wxDefaultSize, 0);

	m_save_logs->SetValue(cfg().ReadBool(_T("/ChatLog/chatlog_enable")));

	sbChatLogSizer->Add(m_save_logs, 0, wxALL, 5);

	wxBoxSizer* m_num_lines_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_num_lines = new wxSpinCtrl(this, wxID_ANY);
	m_num_lines_lbl = new wxStaticText(this, wxID_ANY, _("approx. number of lines loaded from log into chat"), wxDefaultPosition, wxDefaultSize, 0);
	m_num_lines_sizer->Add(m_num_lines);
	m_num_lines_sizer->Add(m_num_lines_lbl, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
	sbChatLogSizer->Add(m_num_lines_sizer, 0, wxALL, 5);

	bBotomSizer->Add(sbChatLogSizer, 1, wxEXPAND | wxRIGHT, 5);

	wxStaticBoxSizer* sbHighlightSizer;
	sbHighlightSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, _("Highlight words")), wxVERTICAL);

	m_hilight_words_label = new wxStaticText(this, wxID_ANY, _("Words to highlight in chat:"), wxDefaultPosition, wxDefaultSize, 0);
	m_hilight_words_label->Wrap(-1);

	sbHighlightSizer->Add(m_hilight_words_label, 0, wxALL, 5);


	sbHighlightSizer->Add(0, 0, 1, wxEXPAND, 5);

	m_highlight_words = new wxTextCtrl(this, ID_HIWORDS, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_highlight_words->SetToolTip(_("enter a ; seperated list"));

	sbHighlightSizer->Add(m_highlight_words, 0, wxALL | wxEXPAND, 5);

	m_highlight_req = new wxCheckBox(this, ID_HL_REQ, _("Additionally play sound/flash titlebar "), wxDefaultPosition, wxDefaultSize, 0);
	sbHighlightSizer->Add(m_highlight_req, 0, wxALL | wxEXPAND, 5);

	bBotomSizer->Add(sbHighlightSizer, 1, wxEXPAND, 5);

	bMainSizerV->Add(bBotomSizer, 0, wxEXPAND | wxBOTTOM | wxRIGHT | wxLEFT, 5);

	bMainSizerV->Add(0, 0, 1, wxEXPAND | wxALL, 5);

	SetScrollRate(SCROLL_RATE, SCROLL_RATE);

	SetSizer(bMainSizerV);
	DoRestore();
	UpdateTextSample();
	UpdateFontLabel();
	Layout();
}


ChatOptionsTab::~ChatOptionsTab()
{
}


void ChatOptionsTab::UpdateTextSample()
{
	m_test_text->Clear();
	m_test_text->SetFont(m_chat_font);
	m_test_text->SetBackgroundColour(m_bg_color->GetColor());

	m_test_text->SetDefaultStyle(wxTextAttr(m_ts_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( "[19:35]" ));
	m_test_text->SetDefaultStyle(wxTextAttr(m_server_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( " ** Server ** Connected to Server.\n" ));

	m_test_text->SetDefaultStyle(wxTextAttr(m_ts_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( "[22:30]" ));
	m_test_text->SetDefaultStyle(wxTextAttr(m_normal_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( " <Dude> hi everyone\n" ));

	m_test_text->SetDefaultStyle(wxTextAttr(m_ts_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( "[22:30]" ));
	m_test_text->SetDefaultStyle(wxTextAttr(m_joinleave_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( " ** Dude2 joined the channel.\n" ));

	m_test_text->SetDefaultStyle(wxTextAttr(m_ts_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( "[22:31]" ));
	m_test_text->SetDefaultStyle(wxTextAttr(m_action_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( " * Dude2 thinks his colors looks nice\n" ));

	m_test_text->SetDefaultStyle(wxTextAttr(m_ts_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( "[22:33]" ));
	m_test_text->SetDefaultStyle(wxTextAttr(m_note_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( "<Dude> Dude2: orl?\n" ));

	m_test_text->SetDefaultStyle(wxTextAttr(m_ts_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( "[22:33]" ));
	m_test_text->SetDefaultStyle(wxTextAttr(m_my_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( " <Dude2> Yeah, but could be better, should tweak them some more...\n" ));

	m_test_text->SetDefaultStyle(wxTextAttr(m_ts_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( "[22:33]" ));
	m_test_text->SetDefaultStyle(wxTextAttr(m_highlight_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( " <Dude> bla bla bla Highlighted word bla bla.\n" ));

	m_test_text->SetDefaultStyle(wxTextAttr(m_ts_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( "[22:33]" ));
	m_test_text->SetDefaultStyle(wxTextAttr(m_error_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( " !! Error error.\n" ));

	m_test_text->SetDefaultStyle(wxTextAttr(m_ts_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( "[22:33]" ));
	m_test_text->SetDefaultStyle(wxTextAttr(m_client_color->GetColor(), m_bg_color->GetColor(), m_chat_font));
	m_test_text->AppendText(_T( " ** Client message." ));
}

void ChatOptionsTab::UpdateFontLabel()
{
	m_fontname->SetLabel(m_chat_font.GetNativeFontInfoUserDesc());
	m_fontname->SetFont(m_chat_font);
}


void ChatOptionsTab::DoRestore()
{
	m_normal_color->SetColor(sett().GetChatColorNormal());
	m_bg_color->SetColor(sett().GetChatColorBackground());
	m_action_color->SetColor(sett().GetChatColorAction());
	m_highlight_color->SetColor(sett().GetChatColorHighlight());
	m_joinleave_color->SetColor(sett().GetChatColorJoinPart());
	m_note_color->SetColor(sett().GetChatColorNotification());
	m_my_color->SetColor(sett().GetChatColorMine());
	m_server_color->SetColor(sett().GetChatColorServer());
	m_client_color->SetColor(sett().GetChatColorClient());
	m_error_color->SetColor(sett().GetChatColorError());
	m_ts_color->SetColor(sett().GetChatColorTime());
	m_chat_font = sett().GetChatFont();
	m_save_logs->SetValue(cfg().ReadBool(_T("/ChatLog/chatlog_enable")));
	m_irc_colors->SetValue(sett().GetUseIrcColors());
	wxString highlightstring;
	wxArrayString highlights = sett().GetHighlightedWords();
	for (unsigned int i = 0; i < highlights.GetCount(); i++)
		highlightstring << highlights[i] << _T( ";" );
	m_highlight_words->SetValue(highlightstring);
	m_highlight_req->SetValue(sett().GetRequestAttOnHighlight());
#ifndef DISABLE_SOUND
	m_play_sounds->SetValue(sett().GetChatPMSoundNotificationEnabled());
#endif
	m_num_lines->SetValue(sett().GetAutoloadedChatlogLinesCount());
	m_broadcast_check->SetValue(cfg().ReadBool(_T("/Chat/BroadcastEverywhere")));
	UpdateFontLabel();
}

void ChatOptionsTab::OnApply(wxCommandEvent& /*unused*/)
{
	sett().SetChatColorNormal(m_normal_color->GetColor());
	sett().SetChatColorBackground(m_bg_color->GetColor());
	sett().SetChatColorAction(m_action_color->GetColor());
	sett().SetChatColorHighlight(m_highlight_color->GetColor());
	sett().SetChatColorJoinPart(m_joinleave_color->GetColor());
	sett().SetChatColorNotification(m_note_color->GetColor());
	sett().SetChatColorMine(m_my_color->GetColor());
	sett().SetChatColorServer(m_server_color->GetColor());
	sett().SetChatColorClient(m_client_color->GetColor());
	sett().SetChatColorError(m_error_color->GetColor());
	sett().SetChatColorTime(m_ts_color->GetColor());
	sett().SetChatFont(m_chat_font);
	sett().SetUseIrcColors(m_irc_colors->IsChecked());
	//m_ui.mw().GetChatTab().ChangeUnreadChannelColour( m_note_color->GetBackgroundColour() );
	//m_ui.mw().GetChatTab().ChangeUnreadPMColour( m_note_color->GetBackgroundColour() );
	sett().SetHighlightedWords(wxStringTokenize(m_highlight_words->GetValue(), _T( ";" )));
	sett().SetRequestAttOnHighlight(m_highlight_req->IsChecked());

	//Chat Log
	cfg().Write(_T("/ChatLog/chatlog_enable"), m_save_logs->GetValue());

	cfg().Write(_T("/Chat/BroadcastEverywhere"), m_broadcast_check->GetValue());

// Behavior
#ifndef DISABLE_SOUND
	sett().SetChatPMSoundNotificationEnabled(m_play_sounds->IsChecked());
#endif
	sett().SetAutoloadedChatlogLinesCount(m_num_lines->GetValue());
}


void ChatOptionsTab::OnRestore(wxCommandEvent& /*unused*/)
{
	DoRestore();
}


void ChatOptionsTab::OnSelectFont(wxCommandEvent& /*unused*/)
{
	wxFontData data;
	data.SetChosenFont(m_chat_font);
	data.SetInitialFont(m_chat_font);
	wxFontDialog dlg(this->GetParent(), data);
	if (dlg.ShowModal() == wxID_OK) {
		m_chat_font = dlg.GetFontData().GetChosenFont();
		UpdateTextSample();
		UpdateFontLabel();
		Layout();
	}
}

void ChatOptionsTab::OnColorChange(ColorButton* button)
{
	wxColour c = GetColourFromUser(this, button->GetColor());
	if (c.IsOk()) {
		button->SetColor(c);
	}
	UpdateTextSample();
}

void ChatOptionsTab::OnNormalSelect(wxCommandEvent& /*unused*/)
{
	OnColorChange(m_normal_color);
}

void ChatOptionsTab::OnBGSelect(wxCommandEvent& /*unused*/)
{
	OnColorChange(m_bg_color);
}

void ChatOptionsTab::OnActionSelect(wxCommandEvent& /*unused*/)
{
	OnColorChange(m_action_color);
}

void ChatOptionsTab::OnHighlightSelect(wxCommandEvent& /*unused*/)
{
	OnColorChange(m_highlight_color);
}

void ChatOptionsTab::OnJoinLeaveSelect(wxCommandEvent& /*unused*/)
{
	OnColorChange(m_joinleave_color);
}

void ChatOptionsTab::OnNotificationSelect(wxCommandEvent& /*unused*/)
{
	OnColorChange(m_note_color);
}

void ChatOptionsTab::OnMyMessSelect(wxCommandEvent& /*unused*/)
{
	OnColorChange(m_my_color);
}

void ChatOptionsTab::OnServerSelect(wxCommandEvent& /*unused*/)
{
	OnColorChange(m_server_color);
}

void ChatOptionsTab::OnClientSelect(wxCommandEvent& /*unused*/)
{
	OnColorChange(m_client_color);
}

void ChatOptionsTab::OnErrorSelect(wxCommandEvent& /*unused*/)
{
	OnColorChange(m_error_color);
}

void ChatOptionsTab::OnTimestampSelect(wxCommandEvent& /*unused*/)
{
	OnColorChange(m_ts_color);
}
