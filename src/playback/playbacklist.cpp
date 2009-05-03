
#include "../tdfcontainer.h"
#include "../offlinebattle.h"
#include "../globalsmanager.h"

template <class ListImp>
ListImp& playbacklist()
{
    static GlobalObjectHolder<ListImp> m_replay_list;
    return m_replay_list;
}

template <class PlaybackImp>
typename PlaybackList<PlaybackImp>::PlaybackType&
    PlaybackList<PlaybackImp>::AddReplay( const PlaybackType& replay )
{
    m_replays[replay.id] = replay;
    return m_replays[replay.id];
}

template <class PlaybackImp>
void PlaybackList<PlaybackImp>::RemoveReplay( playback_id_t const& id )
{
    m_replays.erase(id);
}

template <class PlaybackImp>
typename PlaybackList<PlaybackImp>::playback_map_t::size_type PlaybackList<PlaybackImp>::GetNumReplays()
{
    return m_replays.size();
}

template <class PlaybackImp>
typename PlaybackList<PlaybackImp>::PlaybackType&
    PlaybackList<PlaybackImp>::GetReplayById( playback_id_t const& id )
{
//TODO catch
    playback_iter_t b = m_replays.find(id);
    if (b == m_replays.end())
        throw std::runtime_error("ReplayList_Iter::GetReplay(): no such replay");

    return b->second;
}

template <class PlaybackImp>
bool PlaybackList<PlaybackImp>::ReplayExists( playback_id_t const& id )
{
    return m_replays.find(id) != m_replays.end();
}

template <class PlaybackImp>
bool PlaybackList<PlaybackImp>::DeleteReplay( playback_id_t const& id )
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
const typename PlaybackList<PlaybackImp>::playback_map_t& PlaybackList<PlaybackImp>::GetReplaysMap() const {
    return m_replays;
}


