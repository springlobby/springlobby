/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "ChangeMetaDlg.h"
#include "gui/customdialogs.h"
#include "springsettings/wxkeybinder/keybinder.h"

ChangeMetaDlg::ChangeMetaDlg(const wxString& curMeta, wxWindow* parent)
    : ChangeMetaDlgBase(parent)
    , m_curMetaKey(curMeta)
    , m_newMetaKey(curMeta)
{
}

void ChangeMetaDlg::OnInitDialog(wxInitDialogEvent&)
{
	m_buttonOk->Enable(false);
	m_textCtrlCurKey->SetValue(m_curMetaKey);

	m_ctrlNewKey->Connect(wxEVT_KEY_UP, wxCharEventHandler(ChangeMetaDlg::OnKeyPressed), NULL, this);
}

void ChangeMetaDlg::OnKeyPressed(wxKeyEvent&)
{
	bool enableOk = false;
	m_newMetaKey = m_curMetaKey;
	if (m_ctrlNewKey->IsValidKeyComb() && (m_ctrlNewKey->GetValue() != m_curMetaKey)) {
		m_newMetaKey = m_ctrlNewKey->GetValue();
		enableOk = true;
	}

	m_buttonOk->Enable(enableOk);
}

const wxString& ChangeMetaDlg::GetNewMetaKey() const
{
	return m_newMetaKey;
}
