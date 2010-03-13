#ifndef AUTOJOINCHANNELDIALOG_H
#define AUTOJOINCHANNELDIALOG_H

#include <wx/dialog.h>
#include <wx/string.h>
class wxCommandEvent;
class wxTextCtrl;
class wxWindow;

/** \brief A dialog that lets the user manullay edit his list of channels to join on startup (possibly with passwords)
 * Passwords for existing channels aren't displayed. The dialog expects a [chanName|password] pair per line.
 * \todo See that parsing input doesn't crap out when user enters rubbish
 */
class AutojoinChannelDialog : public wxDialog
{
	public:
		/** Default constructor */
		AutojoinChannelDialog( wxWindow* parent );
		/** Default destructor */
		virtual ~AutojoinChannelDialog();
	protected:

		wxTextCtrl* m_channel_list;

		void OnOk( wxCommandEvent& event );
		void OnCancel( wxCommandEvent& event );
		DECLARE_EVENT_TABLE()
};

#endif // AUTOJOINCHANNELDIALOG_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

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

