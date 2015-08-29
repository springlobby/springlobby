/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef CONTENTDOWNLOADDIALOG_H
#define CONTENTDOWNLOADDIALOG_H

#include "windowattributespickle.h"

#include <wx/dialog.h>
#include <wx/listbase.h>

class wxBoxSizer;
class wxStaticText;
class wxButton;
class wxTextCtrl;
class ContentSearchResultView;
class wxDataViewEvent;

class ContentDownloadDialog : public wxDialog, public WindowAttributesPickle
{
public:
	ContentDownloadDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long int style = wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxDEFAULT_DIALOG_STYLE, const wxString& name = wxDialogNameStr);
	virtual ~ContentDownloadDialog();
	virtual bool Show(bool show = true);
	void OnSearch(wxCommandEvent& event);
	void OnSearchCompleted(wxCommandEvent& event);
	void OnDownloadButton(wxCommandEvent& event);
	void OnCloseButton(wxCommandEvent& event);
	void OnListDownload(wxDataViewEvent& event);

private:
	DECLARE_EVENT_TABLE()
	void Search(const wxString& str);

	wxBoxSizer* m_main_sizer;
	ContentSearchResultView* m_search_res_w;
	wxBoxSizer* m_searchsizer;
	wxTextCtrl* m_searchbox;
	wxButton* m_searchbutton;
	wxButton* m_download_button;
	wxButton* m_close_button;

	bool wildcardsearch;

public:
	enum {
		SEARCH_BUTTON = wxID_HIGHEST,
		ID_SEARCH_FINISHED,
		CLOSE_BUTTON,
		LAUNCH_DOWNLOAD,
		DOWNLOAD_BUTTON
	};
};

#endif // CONTENTDOWNLOADDIALOG_H
