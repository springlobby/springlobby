/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "contentsearchresultview.h"

#include "contentsearchresultdatamodel.h"

BEGIN_EVENT_TABLE(ContentSearchResultView, BaseDataViewCtrl)

END_EVENT_TABLE()

ContentSearchResultView::ContentSearchResultView(wxWindow* parent, wxWindowID id, const wxString& dataViewName) : BaseDataViewCtrl(dataViewName, parent, id) {

	AppendTextColumn(_("Name"), NAME, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_WIDTH, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Preview"), PREVIEW, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_WIDTH, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Type"), TYPE, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_WIDTH, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Size"), SIZE, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_WIDTH, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Downloaded"), DOWNLOADED, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_WIDTH, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

	ContentSearchResultDataModel* model = new ContentSearchResultDataModel();
	AssociateModel(model);

	LoadColumnProperties();
}

ContentSearchResultView::~ContentSearchResultView() {
}

void ContentSearchResultView::AddContent(ContentSearchResult& content) {
	AddItem(content);
}
