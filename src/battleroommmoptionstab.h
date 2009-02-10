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
class IBattle;
class wxSpinCtrlDbl;
class wxTextCtrl;
class wxSpinEvent;
class wxStaticText;
class wxButton;

//totally ok to store pointers here, since wx takes care of gui element destruction for us
typedef std::map<wxString,wxCheckBox*> chkBoxMap;
typedef std::map<wxString,wxComboBox*> comboBoxMap;
typedef std::map<wxString,wxSpinCtrlDbl*> spinCtrlMap;
typedef std::map<wxString,wxTextCtrl*> textCtrlMap;
typedef std::map<wxString,wxStaticText*> staticTextMap;
typedef std::map<wxWindowID,wxString> idInfoMap;

/** \brief a panel displaying programmatically generated gui elements to manipulate mmOptions
 * Since storing of data is mixed in with gui elements, this is a very delicate place to apply changes to.
 * If there's ever any need / desire to refactor this, EXTENSIVE testing should be applied afterwards! \n
 * There's one map per control type, storing  the mapping between name and pointer, where name has the format
 * optionFlag (decimal) + seperator + mmOptionKey \n
 * That way we can use the inverse mapping in the event handlers to go from the name of event-generating gui element
 * to the mmOptionKey that needs to be changed.
 */
class BattleroomMMOptionsTab : public wxScrolledWindow
{
	public:
		BattleroomMMOptionsTab( IBattle& battle, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~BattleroomMMOptionsTab();

		void UpdateOptControls(wxString controlName);
		void OnReloadControls(OptionsWrapper::GameOption flag);
		void Update( const wxString& Tag );

        void UpdatePresetList();

        void OnButton( wxCommandEvent& event );
        void OnLoadPreset( wxCommandEvent& event );
        void OnSavePreset( wxCommandEvent& event );
        void OnDeletePreset( wxCommandEvent& event );
        void OnSetModDefaultPreset( wxCommandEvent& event );
        void OnInfoButton( wxCommandEvent& event );

	protected:
		 IBattle& m_battle;

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

		chkBoxMap m_chkbox_map;
		comboBoxMap m_combox_map;
		spinCtrlMap m_spinctrl_map;
		textCtrlMap m_textctrl_map;
		staticTextMap m_statictext_map;
		idInfoMap m_id_info_map;

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
		void OnSpinCtrlChange(wxSpinEvent& event);
		/** @} */

		wxButton* getButton( wxWindowID id );


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


#endif /*BATTLEROOMMMOPTIONSTAB_H_*/

