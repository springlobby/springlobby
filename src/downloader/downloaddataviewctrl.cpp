/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "downloaddataviewctrl.h"

#include "downloaddataviewmodel.h"
#include "downloadinfo.h"

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

	for(auto item : GetItemsContainer())
	{
		const_cast<DownloadInfo*>(item)->UpdateInfo();
		RefreshItem(*item);
	}
}

void DownloadDataViewCtrl::HighlightItem(long /*item*/) {
	//TODO: implement!
}

void DownloadDataViewCtrl::OnCancel(wxCommandEvent& /*event*/) {
	//TODO: implement download cancellation in ContentManager
}

void DownloadDataViewCtrl::OnRetry(wxCommandEvent& /*event*/) {

	//TODO: this is just a stub! Need implementation!
}

void DownloadDataViewCtrl::AddDownloadInfo(DownloadInfo* dInfo) {
	static_cast<DownloadDataViewModel*>(m_DataModel)->AddItem(*dInfo);
}
