/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_PLAYBACK_PLAYBACKDATAVIEW_H_
#define SRC_GUI_PLAYBACK_PLAYBACKDATAVIEW_H_

#include "gui/basedataviewctrl.h"

class wxWindow;
class PlaybackDataModel;
class StoredGame;
class wxListEvent;
class wxCommandEvent;
class wxPoint;

class PlaybackDataView : public BaseDataViewCtrl<StoredGame>
{
public:
	PlaybackDataView(wxString& dataViewName, wxWindow* parent);
	virtual ~PlaybackDataView();

	void AddPlayback(const StoredGame& replay);
	void RemovePlayback(const StoredGame& replay);
	void OnContextMenu(wxDataViewEvent& event);
	void OnDLMap(wxCommandEvent& event);
	void OnDLMod(wxCommandEvent& event);
	void DeletePlayback(); //deletes the current selected playback from disk & list
	virtual void SetTipWindowText(const long item_hit, const wxPoint& position);
	virtual void HighlightItem(long /*unused*/);

private:
	void OnKeyDown(wxKeyEvent& event);
	
public:
	enum {
		REPLAY_DATAVIEW_ID,
		REPLAY_DATAVIEW_DLMOD_ID,
		REPLAY_DATAVIEW_DLMAP_ID
	};

private:
	wxMenu* m_ContextMenu;
	wxWindow* m_Parent;
	
private:
	enum ColumnIndexes {
		DATE = 0,
		GAME,
		MAP,
		PLAYERS,
		DURATION,
		VERSION,
		FILESIZE,
		FILENAME
	};

	DECLARE_EVENT_TABLE()
};

#endif /* SRC_GUI_PLAYBACK_PLAYBACKDATAVIEW_H_ */
