/*
 * ALURE  OpenAL utility library
 * Copyright (C) 2009 by Chris Robinson.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

/* Title: Asynchronous Playback */

#include "alure_config.h"

#include "main.h"

#include <list>
#include <vector>

#ifdef HAVE_WINDOWS_H

typedef struct {
    ALuint (*func)(ALvoid*);
    ALvoid *ptr;
    HANDLE thread;
} ThreadInfo;

static DWORD CALLBACK StarterFunc(void *ptr)
{
    ThreadInfo *inf = (ThreadInfo*)ptr;
    ALint ret;

    ret = inf->func(inf->ptr);
    ExitThread((DWORD)ret);

    return (DWORD)ret;
}

static ThreadInfo *StartThread(ALuint (*func)(ALvoid*), ALvoid *ptr)
{
    DWORD dummy;
    ThreadInfo *inf = new ThreadInfo;
    inf->func = func;
    inf->ptr = ptr;

    inf->thread = CreateThread(NULL, 0, StarterFunc, inf, 0, &dummy);
    if(!inf->thread)
    {
        delete inf;
        return NULL;
    }

    return inf;
}

static ALuint StopThread(ThreadInfo *inf)
{
    CloseHandle(inf->thread);
    delete inf;

    return 0;
}

#else

typedef struct {
    ALuint (*func)(ALvoid*);
    ALvoid *ptr;
    ALuint ret;
    pthread_t thread;
} ThreadInfo;

static void *StarterFunc(void *ptr)
{
    ThreadInfo *inf = (ThreadInfo*)ptr;
    inf->ret = inf->func(inf->ptr);
    return NULL;
}

static ThreadInfo *StartThread(ALuint (*func)(ALvoid*), ALvoid *ptr)
{
    ThreadInfo *inf = new ThreadInfo;
    inf->func = func;
    inf->ptr = ptr;

    if(pthread_create(&inf->thread, NULL, StarterFunc, inf) != 0)
    {
        delete inf;
        return NULL;
    }

    return inf;
}

static ALuint StopThread(ThreadInfo *inf)
{
    pthread_detach(inf->thread);
    delete inf;

    return 0;
}

#endif

struct AsyncPlayEntry {
	ALuint source;
	alureStream *stream;
	std::vector<ALuint> buffers;
	ALsizei loopcount;
	ALsizei maxloops;
	void (*eos_callback)(void*,ALuint);
	void *user_data;
	bool finished;
	alureUInt64 base_time;
	alureUInt64 max_time;

	AsyncPlayEntry() : source(0), stream(NULL), loopcount(0), maxloops(0),
	                   eos_callback(NULL), user_data(NULL), finished(false),
	                   base_time(0), max_time(0)
	{ }
	AsyncPlayEntry(const AsyncPlayEntry &rhs)
	  : source(rhs.source), stream(rhs.stream), buffers(rhs.buffers),
	    loopcount(rhs.loopcount), maxloops(rhs.maxloops),
	    eos_callback(rhs.eos_callback), user_data(rhs.user_data),
	    finished(rhs.finished), base_time(rhs.base_time),
	    max_time(rhs.max_time)
	{ }
};
static std::list<AsyncPlayEntry> AsyncPlayList;
static ThreadInfo *PlayThreadHandle;

ALuint AsyncPlayFunc(ALvoid*)
{
	while(1)
	{
		EnterCriticalSection(&cs_StreamPlay);
		if(AsyncPlayList.size() == 0)
		{
			StopThread(PlayThreadHandle);
			PlayThreadHandle = NULL;
			LeaveCriticalSection(&cs_StreamPlay);
			break;
		}

		std::list<AsyncPlayEntry>::iterator i = AsyncPlayList.begin(),
		                                    end = AsyncPlayList.end();
		while(i != end)
		{
			ALuint buf;
			ALint processed;
			ALint queued;
			ALint state;

			if(i->stream == NULL)
			{
				alGetSourcei(i->source, AL_SOURCE_STATE, &state);
				if(state != AL_PLAYING && state != AL_PAUSED)
				{
					i->eos_callback(i->user_data, i->source);
					i = AsyncPlayList.erase(i);
				}
				else i++;
				continue;
			}

			alGetSourcei(i->source, AL_SOURCE_STATE, &state);
			alGetSourcei(i->source, AL_BUFFERS_QUEUED, &queued);
			alGetSourcei(i->source, AL_BUFFERS_PROCESSED, &processed);
			while(processed > 0)
			{
				queued--;
				processed--;
				alSourceUnqueueBuffers(i->source, 1, &buf);

				ALint size, channels, bits;
				alGetBufferi(buf, AL_SIZE, &size);
				alGetBufferi(buf, AL_CHANNELS, &channels);
				alGetBufferi(buf, AL_BITS, &bits);
				i->base_time += size / channels * 8 / bits;
				i->base_time %= i->max_time;

				while(!i->finished)
				{
					ALint filled = alureBufferDataFromStream(i->stream, 1, &buf);
					if(filled > 0)
					{
						queued++;
						alSourceQueueBuffers(i->source, 1, &buf);
						if(i->loopcount == 0)
						{
							alGetBufferi(buf, AL_SIZE, &size);
							alGetBufferi(buf, AL_CHANNELS, &channels);
							alGetBufferi(buf, AL_BITS, &bits);
							i->max_time += size / channels * 8 / bits;
						}
						break;
					}
					if(i->loopcount == i->maxloops)
					{
						i->finished = true;
						break;
					}
					if(i->maxloops != -1)
						i->loopcount++;
					i->finished = !alureRewindStream(i->stream);
				}
			}
			if(state != AL_PLAYING && state != AL_PAUSED)
			{
				if(queued == 0)
				{
					alSourcei(i->source, AL_BUFFER, 0);
					alDeleteBuffers(i->buffers.size(), &i->buffers[0]);
					if(i->eos_callback)
						i->eos_callback(i->user_data, i->source);
					i = AsyncPlayList.erase(i);
					continue;
				}
				alSourcePlay(i->source);
			}
			i++;
		}
		LeaveCriticalSection(&cs_StreamPlay);

		alureSleep(0.0078125f);
	}

	return 0;
}


extern "C" {

/* Function: alurePlaySourceStream
 *
 * Plays a stream asynchronously, using the specified source ID. A stream can
 * only be played asynchronously if it is not already playing. It is important
 * that the current context is NOT changed while a stream is playing, otherwise
 * the asynchronous method used to play may start calling OpenAL with invalid
 * IDs. Also note that checking the state of the specified source is not a good
 * method to determine if a stream is playing. If an underrun occurs, the
 * source will enter a stopped state until it is automatically restarted.
 * Instead, set a flag using the callback to indicate the stream being stopped.
 *
 * Parameters:
 * source - The source ID to play the stream with. Any buffers on the source
 *          will be unqueued. It is valid to set source properties not related
 *          to the buffer queue or playback state (ie. you may change the
 *          source's position, pitch, gain, etc, but you must not stop the
 *          source or queue/unqueue buffers on it). The exception is that you
 *          may pause the source, and play the paused source. ALURE will not
 *          attempt to restart a paused source automatically, while a stopped
 *          source is indicative of an underrun and *will* be restarted
 *          automatically.
 * stream - The stream to play asynchronously. Any valid stream will work,
 *          although looping will only work if the stream can be rewound (eg.
 *          streams made with <alureCreateStreamFromCallback> cannot loop, but
 *          will play for as long as the callback provides data).
 * numBufs - The number of buffers used to queue with the OpenAL source. Each
 *           buffer will be filled with the chunk length specified when the
 *           source was created. This value must be at least 2.
 * loopcount - The number of times to loop the stream. When the stream reaches
 *             the end of processing, it will be rewound to continue buffering
 *             data. A value of -1 will cause the stream to loop indefinitely
 *             (or until <alureStopStream> is called).
 * eos_callback - This callback will be called when the stream reaches the end,
 *                no more loops are pending, and the source reaches a stopped
 *                state. It will also be called if an error occured and
 *                playback terminated.
 * userdata - An opaque user pointer passed to the callback.
 *
 * Returns:
 * AL_FALSE on error.
 *
 * See Also:
 * <alureStopSource>, <alureGetSourceOffset>
 */
ALURE_API ALboolean ALURE_APIENTRY alurePlaySourceStream(ALuint source,
    alureStream *stream, ALsizei numBufs, ALsizei loopcount,
    void (*eos_callback)(void *userdata, ALuint source), void *userdata)
{
	if(alGetError() != AL_NO_ERROR)
	{
		SetError("Existing OpenAL error");
		return AL_FALSE;
	}

	if(!alureStream::Verify(stream))
	{
		SetError("Invalid stream pointer");
		return AL_FALSE;
	}

	if(numBufs < 2)
	{
		SetError("Invalid buffer count");
		return AL_FALSE;
	}

	if(!alIsSource(source))
	{
		SetError("Invalid source ID");
		return AL_FALSE;
	}

	EnterCriticalSection(&cs_StreamPlay);

	std::list<AsyncPlayEntry>::iterator i = AsyncPlayList.begin(),
	                                    end = AsyncPlayList.end();
	while(i != end)
	{
		if(i->stream == stream)
		{
			SetError("Stream is already playing");
			LeaveCriticalSection(&cs_StreamPlay);
			return AL_FALSE;
		}
		if(i->source == source)
		{
			SetError("Source is already playing");
			LeaveCriticalSection(&cs_StreamPlay);
			return AL_FALSE;
		}
		i++;
	}

	AsyncPlayEntry ent;
	ent.stream = stream;
	ent.source = source;
	ent.maxloops = loopcount;
	ent.eos_callback = eos_callback;
	ent.user_data = userdata;

	ent.buffers.resize(numBufs);
	alGenBuffers(numBufs, &ent.buffers[0]);
	if(alGetError() != AL_NO_ERROR)
	{
		LeaveCriticalSection(&cs_StreamPlay);
		SetError("Error generating buffers");
		return AL_FALSE;
	}

	if(alureBufferDataFromStream(stream, numBufs, &ent.buffers[0]) < numBufs)
	{
		alDeleteBuffers(numBufs, &ent.buffers[0]);
		alGetError();
		LeaveCriticalSection(&cs_StreamPlay);
		SetError("Error buffering from stream (perhaps too short)");
		return AL_FALSE;
	}

	for(ALsizei idx = 0;idx < numBufs;idx++)
	{
		ALint size, channels, bits;
		alGetBufferi(ent.buffers[idx], AL_SIZE, &size);
		alGetBufferi(ent.buffers[idx], AL_CHANNELS, &channels);
		alGetBufferi(ent.buffers[idx], AL_BITS, &bits);
		ent.max_time += size / channels * 8 / bits;
	}

	if((alSourcei(source, AL_BUFFER, 0),alGetError()) != AL_NO_ERROR ||
	   (alSourceQueueBuffers(source, numBufs, &ent.buffers[0]),alGetError()) != AL_NO_ERROR)
	{
		alDeleteBuffers(numBufs, &ent.buffers[0]);
		alGetError();
		LeaveCriticalSection(&cs_StreamPlay);
		SetError("Error starting source");
		return AL_FALSE;
	}

	if(!PlayThreadHandle)
		PlayThreadHandle = StartThread(AsyncPlayFunc, NULL);
	if(!PlayThreadHandle)
	{
		alSourcei(source, AL_BUFFER, 0);
		alDeleteBuffers(numBufs, &ent.buffers[0]);
		alGetError();
		LeaveCriticalSection(&cs_StreamPlay);
		SetError("Error starting async thread");
		return AL_FALSE;
	}
	AsyncPlayList.push_front(ent);

	LeaveCriticalSection(&cs_StreamPlay);

	return AL_TRUE;
}

/* Function: alurePlaySource
 *
 * Plays the specified source ID and watches for it to stop. When a source
 * enters the AL_STOPPED state, the specified callback is called to alert the
 * application. As with <alurePlaySourceStream>, the current context must not
 * be changed while the source is being watched (before the callback is called
 * or <alureStopSource> is called). It also must not be deleted while being
 * watched.
 *
 * Parameters:
 * source - The source ID to play. As with <alurePlaySourceStream>, it is valid
 *          to set source properties not related to the playback state (ie. you
 *          may change a source's position, pitch, gain, etc). Pausing a source
 *          and restarting a paused source is allowed, and the callback will
 *          still be invoked when the source naturally reaches an AL_STOPPED
 *          state.
 * callback - The callback to be called when the source stops.
 * userdata - An opaque user pointer passed to the callback.
 *
 * Returns:
 * AL_FALSE on error.
 *
 * See Also:
 * <alureStopSource>
 */
ALURE_API ALboolean ALURE_APIENTRY alurePlaySource(ALuint source,
    void (*callback)(void *userdata, ALuint source), void *userdata)
{
	if(alGetError() != AL_NO_ERROR)
	{
		SetError("Existing OpenAL error");
		return AL_FALSE;
	}

	EnterCriticalSection(&cs_StreamPlay);

	std::list<AsyncPlayEntry>::iterator i = AsyncPlayList.begin(),
	                                    end = AsyncPlayList.end();
	while(i != end)
	{
		if(i->source == source)
		{
			SetError("Source is already playing");
			LeaveCriticalSection(&cs_StreamPlay);
			return AL_FALSE;
		}
		i++;
	}

	if((alSourcePlay(source),alGetError()) != AL_NO_ERROR)
	{
		LeaveCriticalSection(&cs_StreamPlay);
		SetError("Error starting source");
		return AL_FALSE;
	}

	if(callback == NULL)
	{
		LeaveCriticalSection(&cs_StreamPlay);
		return AL_TRUE;
	}

	if(!PlayThreadHandle)
		PlayThreadHandle = StartThread(AsyncPlayFunc, NULL);
	if(!PlayThreadHandle)
	{
		alSourceStop(source);
		alGetError();
		LeaveCriticalSection(&cs_StreamPlay);
		SetError("Error starting async thread");
		return AL_FALSE;
	}

	AsyncPlayEntry ent;
	ent.source = source;
	ent.eos_callback = callback;
	ent.user_data = userdata;
	AsyncPlayList.push_front(ent);

	LeaveCriticalSection(&cs_StreamPlay);

	return AL_TRUE;
}

/* Function: alureStopSource
 *
 * Stops the specified source ID, and any associated asynchronous stream. The
 * previously specified callback will be invoked if 'run_callback' is not
 * AL_FALSE. Sources that were not started with <alurePlaySourceStream> or
 * <alurePlaySource> will still be stopped, but will not have any callback
 * called for them.
 *
 * Returns:
 * AL_FALSE on error.
 *
 * See Also:
 * <alurePlaySourceStream>, <alurePlaySource>
 */
ALURE_API ALboolean ALURE_APIENTRY alureStopSource(ALuint source, ALboolean run_callback)
{
	if(alGetError() != AL_NO_ERROR)
	{
		SetError("Existing OpenAL error");
		return AL_FALSE;
	}

	EnterCriticalSection(&cs_StreamPlay);

	if((alSourceStop(source),alGetError()) != AL_NO_ERROR)
	{
		LeaveCriticalSection(&cs_StreamPlay);
		SetError("Error stopping source");
		return AL_FALSE;
	}

	std::list<AsyncPlayEntry>::iterator i = AsyncPlayList.begin(),
	                                    end = AsyncPlayList.end();
	while(i != end)
	{
		if(i->source == source)
		{
			if(i->buffers.size() > 0)
			{
				alSourcei(i->source, AL_BUFFER, 0);
				alDeleteBuffers(i->buffers.size(), &i->buffers[0]);
				alGetError();
			}

			if(run_callback && i->eos_callback)
				i->eos_callback(i->user_data, i->source);
			AsyncPlayList.erase(i);
			break;
		}
		i++;
	}

	LeaveCriticalSection(&cs_StreamPlay);

	return AL_TRUE;
}

/* Function: alureGetSourceOffset
 *
 * Gets the sample offset of the specified source. For sources started with
 * <alurePlaySourceStream>, this will be the total samples played. The offset
 * will loop back to 0 when the stream rewinds for any specified loopcount. For
 * non-streamed sources, the function will behave as if retrieving the
 * AL_SAMPLE_OFFSET source value.
 *
 * Returns:
 * (alureUInt)-1 on error.
 *
 * See Also:
 * <alurePlaySourceStream>
 */
ALURE_API alureUInt64 ALURE_APIENTRY alureGetSourceOffset(ALuint source)
{
	if(alGetError() != AL_NO_ERROR)
	{
		SetError("Existing OpenAL error");
		return (alureUInt64)-1;
	}

	EnterCriticalSection(&cs_StreamPlay);

	ALint pos;
	if((alGetSourcei(source, AL_SAMPLE_OFFSET, &pos),alGetError()) != AL_NO_ERROR)
	{
		LeaveCriticalSection(&cs_StreamPlay);
		SetError("Error retrieving source offset");
		return (alureUInt64)-1;
	}

	alureUInt64 retval = static_cast<ALuint>(pos);
	std::list<AsyncPlayEntry>::iterator i = AsyncPlayList.begin(),
	                                    end = AsyncPlayList.end();
	while(i != end)
	{
		if(i->source == source)
		{
			retval += i->base_time;
			if(i->max_time)
				retval %= i->max_time;
			break;
		}
		i++;
	}

	LeaveCriticalSection(&cs_StreamPlay);

	return retval;
}

} // extern "C"
