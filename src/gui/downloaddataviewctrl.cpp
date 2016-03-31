/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "downloaddataviewctrl.h"
#include "downloaddataviewmodel.h"
#include "utils/globalevents.h"

BEGIN_EVENT_TABLE(DownloadDataViewCtrl, BaseDataViewCtrl) EVT_MENU(DOWNLOAD_DATAVIEW_CANCEL, DownloadDataViewCtrl::OnCancel)
EVT_MENU(DOWNLOAD_DATAVIEW_RETRY, DownloadDataViewCtrl::OnRetry)
END_EVENT_TABLE()

DownloadDataViewCtrl::DownloadDataViewCtrl(const wxString dataViewName,
		wxWindow* parent) :
		BaseDataViewCtrl(dataViewName, parent, DOWNLOAD_DATAVIEW_ID) {

	DownloadDataViewModel* model = new DownloadDataViewModel();
	AssociateModel(model);

	const int DEFAULT_WIDTH = wxCOL_WIDTH_AUTOSIZE;
	AppendTextColumn(_("Name"), NAME, wxDATAVIEW_CELL_INERT, DEFAULT_WIDTH,
			wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Status"), STATUS, wxDATAVIEW_CELL_INERT, DEFAULT_WIDTH,
			wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("% complete"), P_COMPLETE, wxDATAVIEW_CELL_INERT,
			DEFAULT_WIDTH, wxALIGN_CENTER,
			wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("KB/s down"), SPEED, wxDATAVIEW_CELL_INERT,
			DEFAULT_WIDTH, wxALIGN_CENTER,
			wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("ETA"), ETA, wxDATAVIEW_CELL_INERT, DEFAULT_WIDTH,
			wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Filesize (MB)"), FILESIZE, wxDATAVIEW_CELL_INERT,
			DEFAULT_WIDTH, wxALIGN_CENTER,
			wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

	LoadColumnProperties();

//FIXME!!!!
#ifndef WIN32
	GlobalEventManager::Instance()->Subscribe(this,
			GlobalEventManager::OnDownloadStarted,
			wxObjectEventFunction(&DownloadDataViewCtrl::OnDownloadStarted));
	GlobalEventManager::Instance()->Subscribe(this,
			GlobalEventManager::OnDownloadProgress,
			wxObjectEventFunction(&DownloadDataViewCtrl::OnDownloadProgress));
#endif
}

DownloadDataViewCtrl::~DownloadDataViewCtrl()
{
//FIXME!!!!
#ifndef WIN32
	GlobalEventManager::Instance()->UnSubscribeAll(this);
#endif
	Clear();
}

void DownloadDataViewCtrl::SetTipWindowText(const long /*item_hit*/,
		const wxPoint& /*position*/) {
	//TODO: implement!
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

void DownloadDataViewCtrl::OnDownloadStarted() {
	PrDownloader::DownloadProgress *p = new PrDownloader::DownloadProgress;
	prDownloader().GetProgress(*p);
	AddItem(p);
}

void DownloadDataViewCtrl::OnDownloadProgress() {
	PrDownloader::DownloadProgress *existingItem;
	PrDownloader::DownloadProgress p;
	prDownloader().GetProgress(p);

	auto item = itemsIndex.find(p.name);
	if (item == itemsIndex.end() || item->second == nullptr) {
		existingItem = new PrDownloader::DownloadProgress(p);
		AddItem(existingItem);
		return;
	}

	existingItem = item->second;
	p.CopyTo(*existingItem);
	RefreshItem(*existingItem);
}

void DownloadDataViewCtrl::AddItem(PrDownloader::DownloadProgress* p) {
	BaseDataViewCtrl::AddItem(*p, true);

	itemsIndex[p->name] = p;
}

void DownloadDataViewCtrl::Clear() {
	UnselectAll();

	std::vector<const PrDownloader::DownloadProgress*> toBeRemoved;

	for (const PrDownloader::DownloadProgress *p : m_DataModel->GetItemsContainer()) {
		toBeRemoved.push_back(p);
	}

	BaseDataViewCtrl::Clear();

	for (const PrDownloader::DownloadProgress *p : toBeRemoved) {
		delete p;
	}
}

void DownloadDataViewCtrl::ClearFinished() {
	UnselectAll();

	std::vector<const PrDownloader::DownloadProgress*> toBeRemoved;

	for (const PrDownloader::DownloadProgress *p : m_DataModel->GetItemsContainer()) {
		if (p->IsFinished()) {
			toBeRemoved.push_back(p);
		}
	}

	for (const PrDownloader::DownloadProgress *pp : toBeRemoved) {
		RemoveItem(*pp);
		auto item = itemsIndex.find(pp->name);
		assert(item != itemsIndex.end());
		itemsIndex.erase(item);
		delete pp;
	}
}
