/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SL_PLAYBACKLIST_H_INCLUDED
#define SL_PLAYBACKLIST_H_INCLUDED

#include <map>
#include <vector>
#include <wx/event.h>

class wxArrayString;

template <class PlaybackImp>
class PlaybackList : public wxEvtHandler
{
  public:
	PlaybackList():
		wxEvtHandler(),
		m_fails(0)
	{}

    typedef PlaybackImp PlaybackType;

    //! @brief mapping from playback id number to playback object
    typedef std::map<unsigned int, PlaybackType> playback_map_t;
    //! @brief iterator for playback map
    typedef typename playback_map_t::iterator playback_iter_t;
    //! @brief const iterator for playback map
    typedef typename playback_map_t::const_iterator playback_const_iter_t;

    virtual void LoadPlaybacks( const std::vector<std::string>& filenames ) = 0;

	PlaybackType& AddPlayback( const size_t index );
    void AddPlayback(PlaybackType& p, const unsigned int index );
    void RemovePlayback( unsigned int const id );

    PlaybackType &GetPlaybackById( unsigned int const id );

	bool PlaybackExists( unsigned int const id ) const;
    bool DeletePlayback( unsigned int const id );
	typename playback_map_t::size_type GetNumPlaybacks() const;

    void RemoveAll();

    const playback_map_t& GetPlaybacksMap() const;


  protected:

    playback_map_t m_replays;

    unsigned long m_fails;

};

template <class ListImp>
ListImp& playbacklist();

#include "playbacklist.cpp"

#endif // SL_PLAYBACKLIST_H_INCLUDED

