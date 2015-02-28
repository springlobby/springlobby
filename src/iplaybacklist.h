/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SL_PLAYBACKLIST_H_INCLUDED
#define SL_PLAYBACKLIST_H_INCLUDED

#include <map>
#include <vector>
#include <wx/event.h>
#include "storedgame.h"

class IPlaybackList : public wxEvtHandler
{
  public:
	IPlaybackList():
		wxEvtHandler()
	{}

    //! @brief mapping from playback id number to playback object
    typedef std::map<unsigned int, StoredGame> playback_map_t;
    //! @brief iterator for playback map
    typedef typename playback_map_t::iterator playback_iter_t;
    //! @brief const iterator for playback map
    typedef typename playback_map_t::const_iterator playback_const_iter_t;

    virtual void LoadPlaybacks( const std::vector<std::string>& filenames ) = 0;

	StoredGame& AddPlayback( const size_t index );
    void RemovePlayback( unsigned int const id );

    StoredGame &GetPlaybackById( unsigned int const id );

	bool PlaybackExists( unsigned int const id ) const;
    bool DeletePlayback( unsigned int const id );
	typename playback_map_t::size_type GetNumPlaybacks() const;

    void RemoveAll();

    const playback_map_t& GetPlaybacksMap() const;


protected:
    playback_map_t m_replays;
};

#endif // SL_PLAYBACKLIST_H_INCLUDED

