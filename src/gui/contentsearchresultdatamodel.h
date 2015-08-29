/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_CONTENTSEARCHRESULTDATAMODEL_H_
#define SRC_GUI_CONTENTSEARCHRESULTDATAMODEL_H_

#include "contentsearchresult.h"
#include "basedataviewmodel.h"

class ContentSearchResultDataModel: public BaseDataViewModel<ContentSearchResult> {
public:
	ContentSearchResultDataModel();
	virtual ~ContentSearchResultDataModel();

	virtual void GetValue( wxVariant &variant, const wxDataViewItem &item, unsigned int col ) const override;
    virtual int Compare(const wxDataViewItem& itemA, const wxDataViewItem& itemB, unsigned int column, bool ascending) const override;
    virtual wxString GetColumnType(unsigned int column) const override;

public:
	enum{
		NAME,
		PREVIEW,
		TYPE,
		SIZE,
		DOWNLOADED,
		COLUMNS_COUNT
	};
};

#endif /* SRC_GUI_CONTENTSEARCHRESULTDATAMODEL_H_ */
