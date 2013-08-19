/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2013  <copyright holder> <email>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef CONTENTSEARCHRESULTSLISTCTRL_H
#define CONTENTSEARCHRESULTSLISTCTRL_H
#include "../customvirtlistctrl.h"
#include "contentsearchresult.h"

class ContentSearchResultsListctrl : public CustomVirtListCtrl<ContentSearchResult*,ContentSearchResultsListctrl >
{
public:
    ContentSearchResultsListctrl(wxWindow* parent, wxWindowID id, const wxString& name = _T("ContentSearchResultsListctrl"),
            long style = wxSUNKEN_BORDER | wxLC_REPORT | wxLC_ALIGN_LEFT, const wxPoint& pt = wxDefaultPosition,
                    const wxSize& sz = wxDefaultSize);
     wxString GetItemText(long item, long column) const;
        int GetItemImage(long item) const;
        int GetItemColumnImage(long item, long column) const;
        wxListItemAttr* GetItemAttr(long /*unused*/) const {return 0;}

virtual int GetIndexFromData(ContentSearchResult*const& data) const;
virtual void Sort();
virtual ~ContentSearchResultsListctrl();

public:
    enum {
	WIDGETLISTCTRL_ID
    };
    int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) const;
    protected:
        DECLARE_EVENT_TABLE()
};

#endif // CONTENTSEARCHRESULTSLISTCTRL_H
