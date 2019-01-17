/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_HOSTBATTLEDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_HOSTBATTLEDIALOG_H


#include <wx/dialog.h>
#include "gui/windowattributespickle.h"
#include "generated/HostBattleDialogBase.h"

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
class wxPanel;

class HostBattleDialog : public HostBattleDialogBase, public WindowHintsPickle
{
public:
	explicit HostBattleDialog(wxWindow* parent);
	static void RunHostBattleDialog(wxWindow* parent);

private:
	int GetSelectedRank();

	void OnOk(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnNatChange(wxCommandEvent& event);
	void OnReloadMods(wxCommandEvent& event);
	void OnRelayChoice(wxCommandEvent& event);
	void OnUseRelay(wxCommandEvent& event);
	void OnPickRelayHost(wxCommandEvent& event);
	void OnEngineSelect(wxCommandEvent& event);

	void ReloadEngineList();
	void ReloadModList();

	wxMenu* m_relayhost_list;

	enum {
		AUTO_PICK_HOST = wxID_HIGHEST,
		MANUAL_PICK_HOST,
	};

	wxString m_last_relayhost;
	wxArrayString m_relayhost_array_list;

	DECLARE_EVENT_TABLE()
};

namespace SL
{
//! this shows an instance and handles everything wrt data feeding and input handling
}


#endif // SPRINGLOBBY_HEADERGUARD_HOSTBATTLEDIALOG_H
