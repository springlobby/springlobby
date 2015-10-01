/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef BATTLEROOMMMOPTIONSTAB_H_
#define BATTLEROOMMMOPTIONSTAB_H_

#include <wx/scrolwin.h>
#include <map>
#include <lslunitsync/enum.h>

const int BOOL_START_ID = 3000;
const int FLOAT_START_ID = 4000;
const int LIST_START_ID = 5000;
const int STRING_START_ID = 6000;
const int BUTTON_ID_OFFSET = 7000;

class wxBoxSizer;
class wxStaticBoxSizer;

namespace LSL
{
struct mmOptionSection;
}

class wxCheckBox;
class wxComboBox;
class wxCommandEvent;
class wxSpinCtrlDouble;
class wxTextCtrl;
class wxSpinDoubleEvent;
class wxStaticText;
class wxButton;
class IBattle;


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
	BattleroomMMOptionsTab(IBattle* battle, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 300), long style = wxTAB_TRAVERSAL);
	~BattleroomMMOptionsTab();

	void UpdateOptControls(const wxString& controlName);
	//!reload single category
	void OnReloadControls(const LSL::Enum::GameOption& flag);
	//!reload all categories
	void OnReloadControls();

	void UpdatePresetList();
	void SetBattle(IBattle* battle);

private:
	void OnButton(wxCommandEvent& event);
	void OnLoadPreset(wxCommandEvent& event);
	void OnSavePreset(wxCommandEvent& event);
	void OnDeletePreset(wxCommandEvent& event);
	void OnSetModDefaultPreset(wxCommandEvent& event);
	void OnInfoButton(wxCommandEvent& event);

	IBattle* GetBattle();
	IBattle* m_battle;

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

	//        LSL::OptionsWrapper* m_mapmodoptions;

	//        typedef BattleroomMMOptionsTab<IBattle> ThisType;

	//        friend class wxSpinCtrlDouble<ThisType>; //so we don't have to make the event handler public for everyone
	//totally ok to store pointers here, since wx takes care of gui element destruction for us
	typedef std::map<wxString, wxCheckBox*> chkBoxMap;
	typedef std::map<wxString, wxComboBox*> comboBoxMap;
	typedef std::map<wxString, wxSpinCtrlDouble*> spinCtrlMap;
	typedef std::map<wxString, wxTextCtrl*> textCtrlMap;
	typedef std::map<wxString, wxStaticText*> staticTextMap;
	typedef std::map<wxString, wxButton*> buttonMap;
	typedef std::map<wxString, wxString> nameInfoMap; //! map control name <-> info (description)

	chkBoxMap m_chkbox_map;
	comboBoxMap m_combox_map;
	spinCtrlMap m_spinctrl_map;
	textCtrlMap m_textctrl_map;
	staticTextMap m_statictext_map;
	buttonMap m_button_map;
	nameInfoMap m_name_info_map;

	/** \brief setup toplevel sizer per GameOption with all child sizers ( sections )
            */
	void setupOptionsSizer(wxBoxSizer* parent_sizer, LSL::Enum::GameOption optFlag);


	/** \brief generate Gui elements from loaded MMoptions
         * for all values in all option maps create a control (pointer),
         * set the controls name to the option key and add it to the appropriate map and sizer.
         * \return the total num of controls in the sizer
         */
	int setupOptionsSectionSizer(const LSL::mmOptionSection& section, wxBoxSizer* parent_sizer,
				     LSL::Enum::GameOption optFlag);

	/** \name Event handlers
		 * @{
		 * one event handler per gui element type \n
		 * these trigger a SendHostInfo(key) for the changed option
		 */
	void OnComBoxChange(wxCommandEvent&);
	void OnChkBoxChange(wxCommandEvent&);
	void OnTextCtrlChange(wxCommandEvent& event);
	void OnSpinCtrlDoubleChange(wxSpinDoubleEvent& event);
	/** @} */

	wxButton* getButton(const wxWindowID id, const wxString& name);


	DECLARE_EVENT_TABLE()
};

enum {
	BOPTS_LOADPRES = wxID_HIGHEST,
	BOPTS_SAVEPRES,
	BOPTS_DELETEPRES,
	BOPTS_SETDEFAULTPRES,
	BOPTS_CHOSEPRES
};

#endif /*BATTLEROOMMMOPTIONSTAB_H_*/
