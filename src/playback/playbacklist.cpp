
#include "../tdfcontainer.h"
#include "../offlinebattle.h"
#include "../globalsmanager.h"

template <class PlaybackImp>
PlaybackList<PlaybackImp>& playbacklist()
{
    static GlobalObjectHolder<PlaybackList<PlaybackImp> > m_replay_list;
    return m_replay_list;
}

template <class PlaybackType>
PlaybackType& PlaybackList<PlaybackType>::AddReplay( const PlaybackType& replay )
{
    m_replays[replay.id] = replay;
    return m_replays[replay.id];
}

template <class PlaybackType>
void PlaybackList<PlaybackType>::RemoveReplay( playback_id_t const& id )
{
    m_replays.erase(id);
}

template <class PlaybackType>
typename PlaybackList<PlaybackType>::playback_map_t::size_type PlaybackList<PlaybackType>::GetNumReplays()
{
    return m_replays.size();
}

template <class PlaybackType>
PlaybackType& PlaybackList<PlaybackType>::GetReplayById( playback_id_t const& id )
{
//TODO catch
    playback_iter_t b = m_replays.find(id);
    if (b == m_replays.end())
        throw std::runtime_error("ReplayList_Iter::GetReplay(): no such replay");

    return b->second;
}

template <class PlaybackType>
bool PlaybackList<PlaybackType>::ReplayExists( playback_id_t const& id )
{
    return m_replays.find(id) != m_replays.end();
}

template <class PlaybackType>
bool PlaybackList<PlaybackType>::DeleteReplay( playback_id_t const& id )
{
    PlaybackType rep = m_replays[id];
    if ( wxRemoveFile( rep.Filename ) ) {

        //m_filenames.resize(std::remove(m_filenames.begin(), m_filenames.end(), rep.Filename)-m_filenames.begin());

        m_replays.erase(id);
        return true;
    }
    return false;
}

template <class PlaybackType>
void PlaybackList<PlaybackType>::RemoveAll()
{
//    m_filenames.clear();
    m_replays.clear();
    m_fails = 0;
}

template <class PlaybackType>
const typename PlaybackList<PlaybackType>::playback_map_t& PlaybackList<PlaybackType>::GetReplaysMap() const {
    return m_replays;
}


