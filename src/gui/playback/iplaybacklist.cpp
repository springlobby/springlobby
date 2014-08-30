/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "iplaybacklist.h"
#include "offlinebattle.h"
#include "storedgame.h"
#include "utils/conversion.h"

#include <lslutils/globalsmanager.h>

StoredGame& IPlaybackList::AddPlayback( const size_t index )
{
	assert(!PlaybackExists(index)); //no duplicate add
	m_replays[index].id = index;
	return m_replays[index];
}

void IPlaybackList::RemovePlayback( unsigned int const id )
{
    m_replays.erase(id);
}

IPlaybackList::playback_map_t::size_type IPlaybackList::GetNumPlaybacks() const
{
    return m_replays.size();
}

StoredGame& IPlaybackList::GetPlaybackById( unsigned int const id )
{
    playback_iter_t b = m_replays.find(id);
    if (b == m_replays.end())
        throw std::runtime_error("PlaybackList_Iter::GetPlayback(): no such replay");

    return b->second;
}

bool IPlaybackList::PlaybackExists( unsigned int const id ) const
{
    return m_replays.find(id) != m_replays.end();
}

bool IPlaybackList::DeletePlayback( unsigned int const id )
{
	const StoredGame& rep = m_replays[id];
    if ( wxRemoveFile( TowxString(rep.Filename) ) ) {
        m_replays.erase(id);
        return true;
    }
    return false;
}

void IPlaybackList::RemoveAll()
{
    m_replays.clear();
    m_fails = 0;
}

const typename IPlaybackList::playback_map_t& IPlaybackList::GetPlaybacksMap() const {
    return m_replays;
}
