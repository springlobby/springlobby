/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "downloaddataviewctrl.h"

#include "downloaddataviewmodel.h"
#include "downloadinfo.h"

BEGIN_EVENT_TABLE(DownloadDataViewCtrl, BaseDataViewCtrl)
	EVT_MENU(DOWNLOAD_DATAVIEW_CANCEL, DownloadDataViewCtrl::OnCancel)
	EVT_MENU(DOWNLOAD_DATAVIEW_RETRY, DownloadDataViewCtrl::OnRetry)
END_EVENT_TABLE()

DownloadDataViewCtrl::DownloadDataViewCtrl(const wxString dataViewName, wxWindow* parent) : BaseDataViewCtrl(dataViewName, parent, DOWNLOAD_DATAVIEW_ID) {

	DownloadDataViewModel* model = new DownloadDataViewModel();
	AssociateModel(model);

	const int DEFAULT_WIDTH = wxCOL_WIDTH_AUTOSIZE;
	AppendTextColumn(_("Name"), NAME, wxDATAVIEW_CELL_INERT, DEFAULT_WIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Status"), STATUS, wxDATAVIEW_CELL_INERT, DEFAULT_WIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("% complete"), P_COMPLETE, wxDATAVIEW_CELL_INERT, DEFAULT_WIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("KB/s down"), SPEED, wxDATAVIEW_CELL_INERT, DEFAULT_WIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("ETA"), ETA, wxDATAVIEW_CELL_INERT, DEFAULT_WIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Filesize (MB)"), FILESIZE, wxDATAVIEW_CELL_INERT, DEFAULT_WIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

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

    wxString dName = dInfo->GetName().Lower();

    //Ignore/do not add repository content list downloads
    //(this is not useful for users to such weird things in download list)
    if (dName.Contains(_T("versions.gz")))
    {
        return;
    }

	AddItem(*dInfo);
}
