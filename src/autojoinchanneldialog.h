#ifndef AUTOJOINCHANNELDIALOG_H
#define AUTOJOINCHANNELDIALOG_H

#include <wx/dialog.h>
#include <wx/intl.h>
class wxCommandEvent;
class wxTextCtrl;
class wxWindow;


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
        wxString m_old_channels;
		DECLARE_EVENT_TABLE()
};

#endif // AUTOJOINCHANNELDIALOG_H
