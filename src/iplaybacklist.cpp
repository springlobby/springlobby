/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "iplaybacklist.h"
#include "offlinebattle.h"
#include "storedgame.h"
#include "utils/conversion.h"

#include <lslutils/globalsmanager.h>

int IPlaybackList::FindPlayback(const std::string& filename) const //returns id when the filename already exists in m_replays
{
	for (const auto& playback: m_replays){
		if (playback.second.Filename == filename)
			return playback.first;
	}
	return -1;
}

bool IPlaybackList::FindFilename(const std::vector<std::string>& filenames, const std::string& filename) const //search
{
	for (size_t i = 0; i < filenames.size(); ++i) {
		if (filenames[i] == filename)
			return true;
	}
	return false;
}

void IPlaybackList::LoadPlaybacks(const std::vector<std::string>& filenames)
{
	//FIXME: speed the functions FindPlayback / FindFilename up
	for (size_t i = 0; i < filenames.size(); ++i) { //add replays which doesn't exist yet
		const std::string filename = filenames[i];
		const int pos = FindPlayback(filename);
		if (pos == -1) {
			StoredGame& playback = AddPlayback();
			GetReplayInfos(filename, playback);
		}
	}

	std::list<unsigned int> todel;
	for (const auto& playback: m_replays){ //remove not re-added playbacks (deleted?!)
		if (!FindFilename(filenames, playback.second.Filename)) {
			todel.push_back(playback.first);
		}
	}

	for (unsigned int id: todel) {
		DeletePlayback(id);
	}

	assert(m_replays.size() == filenames.size());
}

StoredGame& IPlaybackList::AddPlayback()
{
	const size_t replays = m_replays.size();
	if (!PlaybackExists(replays)) { //last id should be mostly free
		m_replays[replays].id = replays;
		return m_replays[replays];
	}

	for(size_t i = 0; i < replays; i++) { //item was deleted, try to reuse id's / fill gaps
		if (!PlaybackExists(i)) {
			m_replays[i].id = i;
			return m_replays[i];
		}
	}
	assert(false); //should never happen
	return m_replays[0];
}

void IPlaybackList::RemovePlayback(unsigned int const id)
{
	m_replays.erase(id);
}

size_t IPlaybackList::GetNumPlaybacks() const
{
	return m_replays.size();
}

StoredGame& IPlaybackList::GetPlaybackById(unsigned int const id)
{
	auto b = m_replays.find(id);
	if (b == m_replays.end())
		throw std::runtime_error("PlaybackList_Iter::GetPlayback(): no such replay");

	return b->second;
}

bool IPlaybackList::PlaybackExists(unsigned int const id) const
{
	return m_replays.find(id) != m_replays.end();
}

bool IPlaybackList::DeletePlayback(unsigned int const id)
{
	const StoredGame& rep = m_replays[id];
	if (wxRemoveFile(TowxString(rep.Filename))) {
		m_replays.erase(id);
		return true;
	}
	return false;
}

void IPlaybackList::RemoveAll()
{
	m_replays.clear();
}

const std::map<size_t, StoredGame>& IPlaybackList::GetPlaybacksMap() const
{
	return m_replays;
}
