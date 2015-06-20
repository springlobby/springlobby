/*
 * PlaybackDataModel.h
 *
 *  Created on: 19 июня 2015 г.
 *      Author: Руслан
 */

#ifndef SRC_GUI_PLAYBACK_PLAYBACKDATAMODEL_H_
#define SRC_GUI_PLAYBACK_PLAYBACKDATAMODEL_H_

#include <wx/wx.h>
#include <wx/dataview.h>
#include "gui/basedataviewmodel.h"

class StoredGame;

class PlaybackDataModel: public BaseDataViewModel<StoredGame>{
public:
	PlaybackDataModel();
	virtual ~PlaybackDataModel();

    virtual void GetValue( wxVariant &variant, const wxDataViewItem &item, unsigned int col ) const override;

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
		FILENAME
	};
};

#endif /* SRC_GUI_PLAYBACK_PLAYBACKDATAMODEL_H_ */
