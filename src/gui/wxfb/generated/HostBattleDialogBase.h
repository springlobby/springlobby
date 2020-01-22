///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Jan 16 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/bmpcbox.h>
#include <wx/statbox.h>
#include <wx/slider.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class HostBattleDialogBase
///////////////////////////////////////////////////////////////////////////////
class HostBattleDialogBase : public wxDialog
{
	private:

	protected:
		wxBoxSizer* m_main_sizer;
		wxFlexGridSizer* m_top_sizer;
		wxStaticText* m_battle_name_label;
		wxTextCtrl* m_battle_name_text;
		wxStaticText* m_game_label;
		wxChoice* m_game_choice;
		wxStaticText* m_engine_label;
		wxChoice* m_engine_choice;
		wxStaticText* m_password_label;
		wxTextCtrl* m_password_text;
		wxStaticText* m_port_label;
		wxSpinCtrl* m_port_spinctrl;
		wxCheckBox* m_use_relayhost_check;
		wxButton* m_relayhost_pick;
		wxRadioBox* m_nat_traversal_radios;
		wxStaticBoxSizer* m_rank_sizer;
		wxChoice* m_rank_direction;
		wxBitmapComboBox* m_rank_combo;
		wxStaticBoxSizer* m_max_players_box;
		wxSlider* m_max_players_slider;
		wxStaticLine* m_buttons_sep;
		wxButton* m_cancel_button;
		wxButton* m_host_button;

		// Virtual event handlers, overide them in your derived class
		virtual void OnEngineSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUseRelay( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPickRelayHost( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNatChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOk( wxCommandEvent& event ) { event.Skip(); }


	public:

		HostBattleDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Host new battle"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~HostBattleDialogBase();

};

