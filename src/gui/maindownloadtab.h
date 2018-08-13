/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef MAINTORRENTTAB_H
#define MAINTORRENTTAB_H

#include <wx/scrolwin.h>
#include <map>
class wxStaticText;
class wxButton;
class DownloadDataViewCtrl;
class Ui;
class DownloadInfo;
class wxBoxSizer;
class ColorButton;
class WidgetDownloadDialog;
class ContentDownloadDialog;

typedef std::map<wxString, DownloadInfo> map_infos;
typedef map_infos::iterator map_infos_iter;

class MainDownloadTab : public wxPanel
{
public:
	MainDownloadTab(wxWindow* parent);
	~MainDownloadTab();

private:
	void OnCancelButton(wxCommandEvent& event);
	void OnDownloadDialog(wxCommandEvent& event);
	void OnDLWidgets(wxCommandEvent& event);
	void OnClearFinished(wxCommandEvent& event);

	void OnDownloadFailed(wxCommandEvent& /*event*/);
	void OnUnitsyncReloaded(wxCommandEvent& /*event*/);

	void UpdateDownloadDir();

private:
	enum {
		ID_LIST = wxID_HIGHEST,
		ID_BUTTON_CANCEL,
		ID_OUTGOING,
		ID_INCOMING,
		ID_OUTGOING_LBL,
		ID_INCOMING_LBL,
		ID_DOWNLOAD_DIALOG,
		ID_BUTTON_CLEAR,
		ID_BUTTON_WIDGETS,
	};

	wxButton* m_but_cancel;
	wxButton* m_but_clear;
	DownloadDataViewCtrl* m_DownloadDataView;
	WidgetDownloadDialog* m_widgets_dialog;
	wxStaticText* m_currDownloadDirText;
	wxBoxSizer* m_main_sizer;
	ContentDownloadDialog* m_download_dialog;
	wxButton* m_but_download;

private:
	DECLARE_EVENT_TABLE()
};

#endif
