/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SAVEGAMELIST_H
#define SAVEGAMELIST_H

#include "playbacklist.h"

struct Savegame;

template <class PB, class T>
class GlobalObjectHolder;

class SavegameList : public PlaybackList<Savegame>
{
  public:

    typedef Savegame
        PlaybackType;

   virtual  void LoadPlaybacks( const std::vector<std::string>& filenames );

  protected:
    SavegameList();

    template <class PB, class I >
    friend class LSL::Util::GlobalObjectHolder;
private:
	bool GetSavegameInfos ( const wxString& SavegamePath, Savegame& ret ) const;
	wxString GetScriptFromSavegame ( const wxString& SavegamePath ) const;
};

#endif // SAVEGAMELIST_H
