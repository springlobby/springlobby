/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "downloaddataviewctrl.h"

#include "downloaddataviewmodel.h"
#include "downloadsobserver.h"

BEGIN_EVENT_TABLE(DownloadDataViewCtrl, BaseDataViewCtrl)
	EVT_MENU(DOWNLOAD_DATAVIEW_CANCEL, DownloadDataViewCtrl::OnCancel)
	EVT_MENU(DOWNLOAD_DATAVIEW_RETRY, DownloadDataViewCtrl::OnRetry)
END_EVENT_TABLE()

DownloadDataViewCtrl::DownloadDataViewCtrl(const wxString dataViewName, wxWindow* parent) : BaseDataViewCtrl(dataViewName, parent, DOWNLOAD_DATAVIEW_ID) {

	AppendTextColumn(_("Name"), NAME, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_WIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Status"), STATUS, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_WIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("% complete"), P_COMPLETE, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_MINWIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("KB/s down"), SPEED, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_MINWIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("ETA"), ETA, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_MINWIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Filesize (MB)"), FILESIZE, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_WIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

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

void DownloadDataViewCtrl::UpdateDownloadsList() {

	/* Make new list every time */
	m_DataModel->Clear();
	downloadInfoList.clear();

	DownloadsObserver& observ = downloadsObserver();
	observ.GetList(downloadInfoList);

	for(const ObserverDownloadInfo& downloadItem : downloadInfoList)
	{
		UpdateDownloadInfo(downloadItem);
	}
}

void DownloadDataViewCtrl::UpdateDownloadInfo(const ObserverDownloadInfo& downloadInfo) {
	if (ContainsItem(downloadInfo) == false) {
		if (IsTorrentActive(downloadInfo))
		{
			AddDownloadInfo(downloadInfo);
		}
		return;
	} else {
		RefreshItem(downloadInfo);
	}
}

bool DownloadDataViewCtrl::AddDownloadInfo(const ObserverDownloadInfo& downloadInfo) {
	if (ContainsItem(downloadInfo)) {
		return false;
	} else {
		m_DataModel->AddItem(downloadInfo);
		return true;
	}
}

bool DownloadDataViewCtrl::RemoveDownloadInfo(const ObserverDownloadInfo& downloadInfo) {
	if (ContainsItem(downloadInfo) == false) {
		return false;
	} else {
		m_DataModel->RemoveItem(downloadInfo);
		return true;
	}
}

bool DownloadDataViewCtrl::IsTorrentActive(const ObserverDownloadInfo& downloadInfo) const {
	return (downloadInfo.finished == false);
}

void DownloadDataViewCtrl::HighlightItem(long /*item*/) {
	//TODO: implement!
}

void DownloadDataViewCtrl::OnCancel(wxCommandEvent& /*event*/) {
	const ObserverDownloadInfo * downloadInfo = GetSelectedItem();

	if (downloadInfo == nullptr) {
		return;
	} else {
		RemoveDownloadInfo(*downloadInfo);
	}
}

void DownloadDataViewCtrl::OnRetry(wxCommandEvent& /*event*/) {

	//TODO: this is just a stub! Need implementation!
	const ObserverDownloadInfo * downloadInfo = GetSelectedItem();

	if (downloadInfo == nullptr) {
		return;
	} else {
		RemoveDownloadInfo(*downloadInfo);
	}
}
