/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_PLAYBACK_PLAYBACKDATAMODEL_H_
#define SRC_GUI_PLAYBACK_PLAYBACKDATAMODEL_H_

#include <wx/dataview.h>
#include "gui/basedataviewmodel.h"

struct StoredGame;

class PlaybackDataModel: public BaseDataViewModel<StoredGame>{
public:
	PlaybackDataModel();
	virtual ~PlaybackDataModel();

    virtual void GetValue( wxVariant &variant, const wxDataViewItem &item, unsigned int col ) const override;
    virtual wxString GetColumnType(unsigned int columnt) const override;

private:
	enum ColumnIndexes
	{
		DATE = 0,
		GAME,
		MAP,
		PLAYERS,
		DURATION,
		VERSION,
		FILESIZE,
		FILENAME,
		COLUMN_COUNT
	};
};

#endif /* SRC_GUI_PLAYBACK_PLAYBACKDATAMODEL_H_ */
