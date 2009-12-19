#ifndef BATTLEROOMMMOPTIONSTAB_H_
#define BATTLEROOMMMOPTIONSTAB_H_

#include <wx/scrolwin.h>
#include <map>
#include "mmoptionswrapper.h"

const int BOOL_START_ID = 3000;
const int FLOAT_START_ID = 4000;
const int LIST_START_ID = 5000;
const int STRING_START_ID = 6000;
const int BUTTON_ID_OFFSET = 7000;

class wxBoxSizer;
class wxStaticBoxSizer;
class OptionsWrapper;
class wxCheckBox;
class wxComboBox;
class wxCommandEvent;
class Battle;
template <class P>
class SlSpinCtrlDouble;
class wxTextCtrl;
class SlSpinDoubleEvent;
class wxStaticText;
class wxButton;


/** \brief a panel displaying programmatically generated gui elements to manipulate mmOptions
 * Since storing of data is mixed in with gui elements, this is a very delicate place to apply changes to.
 * If there's ever any need / desire to refactor this, EXTENSIVE testing should be applied afterwards! \n
 * There's one map per control type, storing  the mapping between name and pointer, where name has the format
 * optionFlag (decimal) + seperator + mmOptionKey \n
 * That way we can use the inverse mapping in the event handlers to go from the name of event-generating gui element
 * to the mmOptionKey that needs to be changed.
 */
template < class BattleType >
class BattleroomMMOptionsTab : public wxScrolledWindow
{
	public:
		BattleroomMMOptionsTab( BattleType* battle, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~BattleroomMMOptionsTab();

		void UpdateOptControls(wxString controlName);
		//!relaod single category
		void OnReloadControls(OptionsWrapper::GameOption flag);
		//!reload all categories
		void OnReloadControls();

        void UpdatePresetList();

        void OnButton( wxCommandEvent& event );
        void OnLoadPreset( wxCommandEvent& event );
        void OnSavePreset( wxCommandEvent& event );
        void OnDeletePreset( wxCommandEvent& event );
        void OnSetModDefaultPreset( wxCommandEvent& event );
        void OnInfoButton( wxCommandEvent& event );

        BattleType* GetBattle();
        void SetBattle( BattleType* battle );

	protected:
        BattleType* m_battle;

		wxBoxSizer* m_main_sizer;
		wxStaticBoxSizer* m_mod_options_sizer;
		wxBoxSizer* m_mod_layout;
		wxStaticBoxSizer* m_map_options_sizer;
		wxBoxSizer* m_map_layout;
		wxBoxSizer* m_map_mod_container;

        wxButton* m_load_btn;
        wxButton* m_save_btn;
        wxButton* m_delete_btn;
        wxButton* m_default_btn;
        wxComboBox* m_options_preset_sel;

		OptionsWrapper* m_mapmodoptions;

        typedef BattleroomMMOptionsTab<BattleType>
            ThisType;

        friend class SlSpinCtrlDouble<ThisType>; //so we don't have to make the event handler public for everyone
        //totally ok to store pointers here, since wx takes care of gui element destruction for us
        typedef std::map<wxString,wxCheckBox*>
            chkBoxMap;
        typedef std::map<wxString,wxComboBox*>
            comboBoxMap;
        typedef std::map<wxString,SlSpinCtrlDouble<ThisType> * >
            spinCtrlMap;
        typedef std::map<wxString,wxTextCtrl*>
            textCtrlMap;
        typedef std::map<wxString,wxStaticText*>
            staticTextMap;
        typedef std::map<wxString,wxButton*>
            buttonMap;
        typedef std::map<wxString,wxString>
            nameInfoMap; //! map control name <-> info (description)

		chkBoxMap m_chkbox_map;
		comboBoxMap m_combox_map;
		spinCtrlMap m_spinctrl_map;
		textCtrlMap m_textctrl_map;
		staticTextMap m_statictext_map;
		buttonMap m_button_map;
		nameInfoMap m_name_info_map;

		/** \brief setup toplevel sizer per GameOption with all child sizers ( sections )
            */
		void setupOptionsSizer( wxBoxSizer* parent_sizer, OptionsWrapper::GameOption optFlag );


        /** \brief generate Gui elements from loaded MMoptions
         * for all values in all option maps create a control (pointer),
         * set the controls name to the option key and add it to the appropiate map and sizer.
         * \return the total num of controls in the sizer
         */
		int setupOptionsSectionSizer(const mmOptionSection& section, wxBoxSizer* parent_sizer, OptionsWrapper::GameOption optFlag);

		/** \name Event handlers
		 * @{
		 * one event handler per gui element type \n
		 * these trigger a SendHostInfo(key) for the changed option
		 */
		void OnComBoxChange(wxCommandEvent&);
		void OnChkBoxChange(wxCommandEvent&);
		void OnTextCtrlChange(wxCommandEvent& event);
		void OnSpinCtrlDoubleChange(SlSpinDoubleEvent& event);
		/** @} */

		wxButton* getButton( const wxWindowID id, const wxString& name );


		DECLARE_EVENT_TABLE();
};

enum
{
  BOPTS_LOADPRES =  wxID_HIGHEST,
  BOPTS_SAVEPRES,
  BOPTS_DELETEPRES,
  BOPTS_SETDEFAULTPRES,
  BOPTS_CHOSEPRES
};

#include "battleroommmoptionstab.cpp"

#endif /*BATTLEROOMMMOPTIONSTAB_H_*/

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

