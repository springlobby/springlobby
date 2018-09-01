/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_CONTENTSEARCHRESULTVIEW_H_
#define SRC_GUI_CONTENTSEARCHRESULTVIEW_H_

#include "basedataviewctrl.h"
#include "contentsearchresult.h"
class ContentSearchResultView : public BaseDataViewCtrl<ContentSearchResult>
{
public:
	ContentSearchResultView(wxWindow* parent, wxWindowID id, const wxString& dataViewName);
	virtual ~ContentSearchResultView();
	void AddContent(ContentSearchResult& content);

public:
	enum {
		WIDGETLISTCTRL_ID
	};

	enum {
		NAME,
		PREVIEW,
		TYPE,
		SIZE,
		DOWNLOADED,
		COLUMNS_COUNT
	};

	DECLARE_EVENT_TABLE()
};

#endif /* SRC_GUI_CONTENTSEARCHRESULTVIEW_H_ */
