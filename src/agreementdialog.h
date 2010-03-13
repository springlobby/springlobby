#ifndef SPRINGLOBBY_HEADERGUARD_AGREEMENTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_AGREEMENTDIALOG_H

#include <wx/dialog.h>

class wxTextCtrl;
class wxStaticText;
class wxStaticLine;
class wxButton;

/** \brief dialog showing agreement on first start/ register that user needs to accept
 * \todo DOCMEMORE */
class AgreementDialog : public wxDialog
{
  public:
    AgreementDialog( wxWindow* parent, const wxString& agreement );

    void OnYes( wxCommandEvent& event );
    void OnNo( wxCommandEvent& event );

  protected:
    wxTextCtrl* m_text;
    wxStaticText* m_accept_lbl;
    wxStaticLine* m_button_sep;
    wxButton* m_yes_btn;
    wxButton* m_no_btn;

    enum {
        AGREE_YES = wxID_HIGHEST,
        AGREE_NO,
    };

    DECLARE_EVENT_TABLE()

};

#endif // SPRINGLOBBY_HEADERGUARD_AGREEMENTDIALOG_H

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

