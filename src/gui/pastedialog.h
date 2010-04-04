/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/


#ifndef PASTEDIALOG_H
#define PASTEDIALOG_H

#include <wx/dialog.h>

/** \brief let the user decide to pastebin a message
 */
class PasteDialog : public wxDialog
{
public:
	PasteDialog( wxWindow *parent, const wxString& message );
	virtual ~PasteDialog();

//	void OnOptionsNo(wxCommandEvent& event);

protected:

	const wxString& m_message;
	 DECLARE_EVENT_TABLE()

};

#endif // PASTEDIALOG_H
