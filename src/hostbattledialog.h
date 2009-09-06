#ifndef SPRINGLOBBY_HEADERGUARD_HOSTBATTLEDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_HOSTBATTLEDIALOG_H


#include <wx/dialog.h>

class wxStaticText;
class wxTextCtrl;
class wxChoice;
class wxSlider;
class wxRadioBox;
class wxRadioButton;
class wxStaticBitmap;
class wxStaticLine;
class wxButton;
class wxBitmapButton;
class wxCheckBox;
class wxMenu;

class HostBattleDialog : public wxDialog
{
	public:
		HostBattleDialog( wxWindow* parent );

		void ReloadModList();


	protected:

		void OnOk           ( wxCommandEvent& event );
		void OnCancel       ( wxCommandEvent& event );
		void OnNatChange    ( wxCommandEvent& event );
		void OnReloadMods   ( wxCommandEvent& event );
		void OnRelayChoice	( wxCommandEvent& event );
		void OnUseRelay     ( wxCommandEvent& event );
        void OnPickRelayHost( wxCommandEvent& event );

		int GetSelectedRank();

		wxStaticText* m_desc_lbl;
		wxTextCtrl* m_desc_text;
		wxStaticText* m_mod_lbl;
		wxChoice* m_mod_pic;
		wxStaticText* m_pwd_lbl;
		wxTextCtrl* m_pwd_text;

		wxStaticText* m_port_lbl;
		wxTextCtrl* m_port_text;
        wxTextCtrl* m_relayhost_name;
//    entirely disabled until functionality is in server
//    wxCheckBox* m_port_test_check;
		wxCheckBox* m_relayed_host_check;
		wxCheckBox* m_desc_check;
		wxSlider* m_players_slide;
		wxRadioBox* m_nat_radios;
		wxRadioButton* m_rank0_radio;
		wxStaticBitmap* m_rank0_img;
		wxRadioButton* m_rank1_radio;
		wxStaticBitmap* m_rank1_img;
		wxRadioButton* m_rank2_radio;
		wxStaticBitmap* m_rank2_img;
		wxRadioButton* m_rank3_radio;
		wxStaticBitmap* m_rank3_img;
		wxRadioButton* m_rank4_radio;
		wxStaticBitmap* m_rank4_img;
		wxRadioButton* m_rank5_radio;
		wxStaticBitmap* m_rank5_img;
		wxRadioButton* m_rank6_radio;
		wxStaticBitmap* m_rank6_img;

		wxStaticLine* m_buttons_sep;
		wxButton* m_cancel_btn;

		wxButton* m_host_btn;

		wxMenu* m_relayhost_list;
		wxButton* m_relayed_host_pick;

        wxBitmapButton* m_refresh_btn;


		enum {
			HOST_CANCEL = wxID_HIGHEST,
			HOST_OK,
			CHOSE_NAT,
			PICK_RELAYHOST,
			AUTO_PICK_HOST,
			MANUAL_PICK_HOST,
			BTN_REFRESH,
			CHK_USE_RELAY
		};

		wxString m_last_relayhost;
		wxArrayString m_relayhost_array_list;

		DECLARE_EVENT_TABLE()
};


#endif // SPRINGLOBBY_HEADERGUARD_HOSTBATTLEDIALOG_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

