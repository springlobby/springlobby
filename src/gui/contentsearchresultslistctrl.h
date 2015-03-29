/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef CONTENTSEARCHRESULTSLISTCTRL_H
#define CONTENTSEARCHRESULTSLISTCTRL_H
#include "gui/customvirtlistctrl.h"
#include "contentsearchresult.h"

class ContentSearchResultsListctrl : public CustomVirtListCtrl<ContentSearchResult*, ContentSearchResultsListctrl>
{
public:
	ContentSearchResultsListctrl(wxWindow* parent, wxWindowID id, const wxString& name = _T("ContentSearchResultsListctrl"),
				     long style = wxSUNKEN_BORDER | wxLC_REPORT | wxLC_ALIGN_LEFT, const wxPoint& pt = wxDefaultPosition,
				     const wxSize& sz = wxDefaultSize);
	wxString GetItemText(long item, long column) const;
	int GetItemImage(long item) const;
	int GetItemColumnImage(long item, long column) const;
	wxListItemAttr* GetItemAttr(long /*unused*/) const
	{
		return 0;
	}

	virtual int GetIndexFromData(ContentSearchResult* const& data) const;
	virtual void Sort();
	virtual ~ContentSearchResultsListctrl();
	void AddContent(DataType& content);

public:
	enum {
		WIDGETLISTCTRL_ID
	};
	int CompareOneCrit(DataType u1, DataType u2, int col, int dir) const;

private:
	DECLARE_EVENT_TABLE()
};

#endif // CONTENTSEARCHRESULTSLISTCTRL_H
