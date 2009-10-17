#ifndef MMOPTIONWINDOWS_H_INCLUDED
#define MMOPTIONWINDOWS_H_INCLUDED

#include <wx/dialog.h>

class wxCheckBox;
class wxComboBox;
class wxCommandEvent;
class IBattle;
class wxSpinCtrlDbl;
class wxTextCtrl;
class wxStaticText;
class wxButton;

class SingleOptionDialog: public wxDialog
{

	public:
			SingleOptionDialog( IBattle& battle, const wxString& optiontag );

			void OnOk(wxCommandEvent& event);
			void OnCancel(wxCommandEvent& event);

	protected:
			IBattle& m_battle;
			wxString m_tag;

			wxCheckBox* m_checkbox;
			wxComboBox* m_combobox;
			wxSpinCtrlDbl* m_spinctrl;
			wxTextCtrl* m_textctrl;
			wxButton* m_cancel_button;
			wxButton* m_ok_button;

			DECLARE_EVENT_TABLE();
};

enum
{
	ID_CANCEL = wxID_HIGHEST,
	ID_OK
};

#endif // MMOPTIONWINDOWS_H_INCLUDED

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

