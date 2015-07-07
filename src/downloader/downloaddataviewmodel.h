/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_DOWNLOADER_DOWNLOADDATAVIEWMODEL_H_
#define SRC_DOWNLOADER_DOWNLOADDATAVIEWMODEL_H_

#include "gui/basedataviewmodel.h"

class ObserverDownloadInfo;

class DownloadDataViewModel: public BaseDataViewModel<ObserverDownloadInfo> {
public:
	DownloadDataViewModel();
	virtual ~DownloadDataViewModel();

    virtual void GetValue( wxVariant &variant, const wxDataViewItem &item, unsigned int col ) const override;
    virtual int Compare(const wxDataViewItem& itemA, const wxDataViewItem& itemB, unsigned int column, bool ascending) const override;

private:
	enum ColumnIndexes
	{
		NAME = 0,
		STATUS,
		P_COMPLETE,
		SPEED,
		ETA,
		FILESIZE
	};
};

#endif /* SRC_DOWNLOADER_DOWNLOADDATAVIEWMODEL_H_ */
