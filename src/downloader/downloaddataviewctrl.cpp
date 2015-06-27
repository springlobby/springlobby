/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "downloaddataviewctrl.h"

#include "downloaddataviewmodel.h"

BEGIN_EVENT_TABLE(DownloadDataViewCtrl, BaseDataViewCtrl)
END_EVENT_TABLE()

DownloadDataViewCtrl::DownloadDataViewCtrl(const wxString dataViewName, wxWindow* parent) : BaseDataViewCtrl(dataViewName, parent, DOWNLOAD_DATAVIEW_ID) {
	m_popup = nullptr;

	DownloadDataViewModel* model = new DownloadDataViewModel();
	AssociateModel(model);

	LoadColumnProperties();
}

DownloadDataViewCtrl::~DownloadDataViewCtrl() {
}

void DownloadDataViewCtrl::SetTipWindowText(const long /*item_hit*/,
		const wxPoint& /*position*/) {
	//TODO: implement!
}

void DownloadDataViewCtrl::UpdateTorrentsList() {
	//TODO: implement!
}

void DownloadDataViewCtrl::HighlightItem(long /*item*/) {
	//TODO: implement!
}

void DownloadDataViewCtrl::OnCancel(wxCommandEvent& event) {
	//TODO: implement!
}

void DownloadDataViewCtrl::OnRetry(wxCommandEvent& event) {
	//TODO: implement!
}
