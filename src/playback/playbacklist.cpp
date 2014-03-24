/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "../offlinebattle.h"
#include <lsl/battle/tdfcontainer.h>
#include <lslutils/globalsmanager.h>

template <class ListImp>
ListImp& playbacklist()
{
    static LSL::Util::LineInfo<ListImp> m( AT );
    static LSL::Util::GlobalObjectHolder<ListImp, LSL::Util::LineInfo<ListImp> > m_replay_list( m );
    return m_replay_list;
}

template <class PlaybackImp>
typename PlaybackList<PlaybackImp>::PlaybackType&
	PlaybackList<PlaybackImp>::AddPlayback( const size_t index )
{
	m_replays[index] = PlaybackType( index );
	return m_replays[index];
}

template <class PlaybackImp>
void PlaybackList<PlaybackImp>::RemovePlayback( playback_id_t const& id )
{
    m_replays.erase(id);
}

template <class PlaybackImp>
typename PlaybackList<PlaybackImp>::playback_map_t::size_type PlaybackList<PlaybackImp>::GetNumPlaybacks() const
{
    return m_replays.size();
}

template <class PlaybackImp>
typename PlaybackList<PlaybackImp>::PlaybackType&
    PlaybackList<PlaybackImp>::GetPlaybackById( playback_id_t const& id )
{
//TODO catch
    playback_iter_t b = m_replays.find(id);
    if (b == m_replays.end())
        throw std::runtime_error("PlaybackList_Iter::GetPlayback(): no such replay");

    return b->second;
}

template <class PlaybackImp>
bool PlaybackList<PlaybackImp>::PlaybackExists( playback_id_t const& id ) const
{
    return m_replays.find(id) != m_replays.end();
}

template <class PlaybackImp>
bool PlaybackList<PlaybackImp>::DeletePlayback( playback_id_t const& id )
{
    PlaybackType rep = m_replays[id];
    if ( wxRemoveFile( rep.Filename ) ) {

        //m_filenames.resize(std::remove(m_filenames.begin(), m_filenames.end(), rep.Filename)-m_filenames.begin());

        m_replays.erase(id);
        return true;
    }
    return false;
}

template <class PlaybackImp>
void PlaybackList<PlaybackImp>::RemoveAll()
{
//    m_filenames.clear();
    m_replays.clear();
    m_fails = 0;
}

template <class PlaybackImp>
const typename PlaybackList<PlaybackImp>::playback_map_t& PlaybackList<PlaybackImp>::GetPlaybacksMap() const {
    return m_replays;
}


