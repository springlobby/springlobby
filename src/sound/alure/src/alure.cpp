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

/* Title: Main and Miscellanious */

#include "alure_config.h"

#include "main.h"

#include <string.h>
#include <errno.h>
#include <time.h>
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#include <vector>
#include <string>
#include <map>

std::map<ALint,UserCallbacks> InstalledCallbacks;
std::map<std::string,void*> FunctionList;
CRITICAL_SECTION cs_StreamPlay;
alureStream::ListType alureStream::StreamList;


void init_alure(void)
{
    InitializeCriticalSection(&cs_StreamPlay);

#ifdef HAS_MPG123
    mpg123_init();
#endif
#ifdef HAS_GSTREAMER
    gst_init(NULL, NULL);
#endif

#define ADD_FUNCTION(x) FunctionList[#x] = (void*)(x)
    ADD_FUNCTION(alureGetVersion);
    ADD_FUNCTION(alureGetErrorString);
    ADD_FUNCTION(alureGetDeviceNames);
    ADD_FUNCTION(alureFreeDeviceNames);
    ADD_FUNCTION(alureInitDevice);
    ADD_FUNCTION(alureShutdownDevice);
    ADD_FUNCTION(alureGetSampleFormat);
    ADD_FUNCTION(alureSleep);
    ADD_FUNCTION(alureCreateBufferFromFile);
    ADD_FUNCTION(alureCreateBufferFromMemory);
    ADD_FUNCTION(alureBufferDataFromFile);
    ADD_FUNCTION(alureBufferDataFromMemory);
    ADD_FUNCTION(alureCreateStreamFromFile);
    ADD_FUNCTION(alureCreateStreamFromMemory);
    ADD_FUNCTION(alureCreateStreamFromStaticMemory);
    ADD_FUNCTION(alureCreateStreamFromCallback);
    ADD_FUNCTION(alureRewindStream);
    ADD_FUNCTION(alureDestroyStream);
    ADD_FUNCTION(alureInstallDecodeCallbacks);
    ADD_FUNCTION(alureSetIOCallbacks);
    ADD_FUNCTION(alureGetProcAddress);
    ADD_FUNCTION(alurePlaySourceStream);
    ADD_FUNCTION(alurePlaySource);
    ADD_FUNCTION(alureStopSource);
    ADD_FUNCTION(alureGetSourceOffset);
#undef ADD_FUNCTION
}

static void deinit_alure(void)
{
    alureStream::Clear();

#ifdef HAS_MPG123
    mpg123_exit();
#endif
#ifdef HAS_GSTREAMER
    gst_deinit();
#endif

    DeleteCriticalSection(&cs_StreamPlay);
}

static struct MyConstructorClass {
    MyConstructorClass()
    { init_alure(); };
    ~MyConstructorClass()
    { deinit_alure(); };
} MyConstructor;

static const ALchar *last_error = "No error";

void SetError(const char *err)
{
    last_error = err;
}

extern "C" {

/* Function: alureGetVersion
 *
 * Stores the major and minor version of the library. If either major or minor
 * are NULL, that value is not provided.
 */
ALURE_API void ALURE_APIENTRY alureGetVersion(ALuint *major, ALuint *minor)
{
    if(major) *major = ALURE_VER_MAJOR;
    if(minor) *minor = ALURE_VER_MINOR;
}

/* Function: alureGetErrorString
 *
 * Returns a string describing the last error encountered.
 */
ALURE_API const ALchar* ALURE_APIENTRY alureGetErrorString(void)
{
    const ALchar *ret = last_error;
    last_error = "No error";
    return ret;
}


/* Function: alureGetDeviceNames
 *
 * Gets an array of device name strings from OpenAL. This encapsulates
 * AL_ENUMERATE_ALL_EXT (if supported and 'all' is true) and standard
 * enumeration, with 'count' being set to the number of returned device
 * names.
 *
 * Returns:
 * An array of device name strings, or NULL on error.
 *
 * See Also:
 * <alureFreeDeviceNames>
 */
ALURE_API const ALCchar** ALURE_APIENTRY alureGetDeviceNames(ALCboolean all, ALCsizei *count)
{
    const ALCchar *list = NULL;
    if(all && alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT"))
        list = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
    else
        list = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
    if(!list)
    {
        alcGetError(NULL);
        SetError("No device names found");
        return NULL;
    }

    const ALCchar *cur = list;
    ALuint retlistLen = 0;
    while(*cur)
    {
        cur += strlen(cur)+1;
        retlistLen++;
    }

    const ALCchar **retlist = new const ALCchar*[retlistLen+1];
    retlistLen = 0;
    cur = list;
    while(*cur)
    {
        ALCuint len = strlen(cur)+1;
        ALCchar *newstr = new ALCchar[len];

        memcpy(newstr, cur, len);
        cur += len;

        retlist[retlistLen] = newstr;
        retlistLen++;
    }
    retlist[retlistLen] = NULL;

    *count = retlistLen;
    return retlist;
}

/* Function: alureFreeDeviceNames
 *
 * Frees the device name array returned from alureGetDeviceNames.
 *
 * See Also:
 * <alureGetDeviceNames>
 */
ALURE_API ALvoid ALURE_APIENTRY alureFreeDeviceNames(const ALCchar **names)
{
    if(names)
    {
        for(ALCuint i = 0;names[i];i++)
            delete[] const_cast<ALCchar*>(names[i]);
        delete[] names;
    }
}


/* Function: alureInitDevice
 *
 * Opens the named device, creates a context with the given attributes, and
 * sets that context as current. The name and attribute list would be the same
 * as what's passed to alcOpenDevice and alcCreateContext respectively.
 *
 * Returns:
 * AL_FALSE on error.
 *
 * See Also:
 * <alureShutdownDevice>
 */
ALURE_API ALboolean ALURE_APIENTRY alureInitDevice(const ALCchar *name, const ALCint *attribs)
{
    ALCdevice *device = alcOpenDevice(name);
    if(!device)
    {
        alcGetError(NULL);

        SetError("Device open failed");
        return AL_FALSE;
    }

    ALCcontext *context = alcCreateContext(device, attribs);
    if(alcGetError(device) != ALC_NO_ERROR || !context)
    {
        alcCloseDevice(device);

        SetError("Context creation failed");
        return AL_FALSE;
    }

    alcMakeContextCurrent(context);
    if(alcGetError(device) != AL_NO_ERROR)
    {
        alcDestroyContext(context);
        alcCloseDevice(device);

        SetError("Context setup failed");
        return AL_FALSE;
    }

    return AL_TRUE;
}

/* Function: alureShutdownDevice
 *
 * Destroys the current context and closes its associated device.
 *
 * Returns:
 * AL_FALSE on error.
 *
 * See Also:
 * <alureInitDevice>
 */
ALURE_API ALboolean ALURE_APIENTRY alureShutdownDevice(void)
{
    ALCcontext *context = alcGetCurrentContext();
    ALCdevice *device = alcGetContextsDevice(context);
    if(alcGetError(device) != ALC_NO_ERROR || !device)
    {
        SetError("Failed to get current device");
        return AL_FALSE;
    }

    if(alcMakeContextCurrent(NULL) == ALC_FALSE)
    {
        alcGetError(NULL);
        SetError("Failed to unset current context");
        return AL_FALSE;
    }

    alcDestroyContext(context);
    alcCloseDevice(device);
    alcGetError(NULL);

    return AL_TRUE;
}


/* Function: alureGetSampleFormat
 *
 * Retrieves an OpenAL format for the given sample format. If bits is non-0,
 * floatbits must be 0, and if floatbits is non-0, bits must be 0. The
 * application should not rely on any particular format enum being returned as
 * it is dependant on the available extensions. The returned format will be
 * valid for the current context. Requires an active context.
 *
 * Returns:
 * An OpenAL format enum for the given sample format, or AL_NONE if one can't
 * be found.
 */
ALURE_API ALenum ALURE_APIENTRY alureGetSampleFormat(ALuint channels, ALuint bits, ALuint floatbits)
{
    if(alGetError() != AL_NO_ERROR)
    {
        SetError("Existing OpenAL error");
        return AL_NONE;
    }

    if(bits && floatbits)
    {
        SetError("Both bit-types specified");
        return AL_NONE;
    }

    if(bits == 8)
    {
        if(channels == 1) return AL_FORMAT_MONO8;
        if(channels == 2) return AL_FORMAT_STEREO8;
        if(alIsExtensionPresent("AL_EXT_MCFORMATS"))
        {
            if(channels == 4) return AL_FORMAT_QUAD8;
            if(channels == 6) return AL_FORMAT_51CHN8;
            if(channels == 7) return AL_FORMAT_61CHN8;
            if(channels == 8) return AL_FORMAT_71CHN8;
        }
        SetError("Unhandled channel count\n");
        return AL_NONE;
    }
    if(bits == 16)
    {
        if(channels == 1) return AL_FORMAT_MONO16;
        if(channels == 2) return AL_FORMAT_STEREO16;
        if(alIsExtensionPresent("AL_EXT_MCFORMATS"))
        {
            if(channels == 4) return AL_FORMAT_QUAD16;
            if(channels == 6) return AL_FORMAT_51CHN16;
            if(channels == 7) return AL_FORMAT_61CHN16;
            if(channels == 8) return AL_FORMAT_71CHN16;
        }
        SetError("Unhandled channel count\n");
        return AL_NONE;
    }
    if(floatbits == 32)
    {
        if(alIsExtensionPresent("AL_EXT_FLOAT32"))
        {
            if(channels == 1) return AL_FORMAT_MONO_FLOAT32;
            if(channels == 2) return AL_FORMAT_STEREO_FLOAT32;
            if(alIsExtensionPresent("AL_EXT_MCFORMATS"))
            {
                if(channels == 4) return AL_FORMAT_QUAD32;
                if(channels == 6) return AL_FORMAT_51CHN32;
                if(channels == 7) return AL_FORMAT_61CHN32;
                if(channels == 8) return AL_FORMAT_71CHN32;
            }
        }
        SetError("Unhandled channel count\n");
        return AL_NONE;
    }

    SetError("Unhandled bit-depth\n");
    return AL_NONE;
}


/* Function: alureInstallDecodeCallbacks
 *
 * Installs callbacks to enable ALURE to handle more file types. The index is
 * the order that each given set of callbacks will be tried, starting at the
 * most negative number (INT_MIN) and going up. Negative indices will be tried
 * before the built-in decoders, and positive indices will be tried after.
 * Installing callbacks onto the same index multiple times will remove the
 * previous callbacks, and removing old callbacks won't affect any opened files
 * using them (they'll continue to use the old functions until properly closed,
 * although newly opened files will use the new ones). Passing NULL for all
 * callbacks is a valid way to remove an installed set, otherwise certain
 * callbacks must be specified. Callbacks that are not specified will assume
 * failure.
 *
 * Parameters:
 * open_file - This callback is expected to open the named file and prepare it
 *             for decoding. If the callbacks cannot decode the file, NULL
 *             should be returned to indicate failure. Upon success, a non-NULL
 *             handle must be returned, which will be used as a unique
 *             identifier for the decoder instance. This callback is required
 *             if open_memory is not specified.
 * open_memory - This callback behaves the same as open_file, except it takes a
 *               memory segment for input instead of a filename. The given
 *               memory will remain valid while the instance is open. This
 *               callback is required if open_file is not specified.
 * get_format - This callback is used to retrieve the format of the decoded
 *              data for the given instance. It is the responsibility of the
 *              function to make sure the returned format is valid for the
 *              current AL context (eg. don't return AL_FORMAT_QUAD16 if the
 *              AL_EXT_MCFORMATS extension isn't available). Returning 0 for
 *              samplerate or blocksize, or returning AL_NONE for format, will
 *              cause a failure. Returning AL_FALSE indicates failure. This
 *              callback is required.
 * decode - This callback is called to get more decoded data. Up to the
 *          specified amount of bytes should be written to the data pointer.
 *          The number of bytes written should be a multiple of the block size,
 *          otherwise an OpenAL error may occur during buffering. The function
 *          should return the number of bytes written. This callback is
 *          required.
 * rewind - This callback is for rewinding the instance so that the next decode
 *          calls for it will get audio data from the start of the sound file.
 *          If the stream fails to rewind, AL_FALSE should be returned.
 * close - This callback is called at the end of processing for a particular
 *         instance. The handle will not be used further and any associated
 *         data may be deleted.
 *
 * Returns:
 * AL_FALSE on error.
 */
ALURE_API ALboolean ALURE_APIENTRY alureInstallDecodeCallbacks(ALint index,
      void*     (*open_file)(const ALchar *filename),
      void*     (*open_memory)(const ALubyte *data, ALuint length),
      ALboolean (*get_format)(void *instance, ALenum *format, ALuint *samplerate, ALuint *blocksize),
      ALuint    (*decode)(void *instance, ALubyte *data, ALuint bytes),
      ALboolean (*rewind)(void *instance),
      void      (*close)(void *instance))
{
    if(!open_file && !open_memory && !get_format && !decode && !rewind && !close)
    {
        std::map<ALint,UserCallbacks>::iterator i = InstalledCallbacks.find(index);
        if(i != InstalledCallbacks.end())
            InstalledCallbacks.erase(i);
        return AL_TRUE;
    }

    if((!open_file && !open_memory) || !get_format || !decode)
    {
        SetError("Missing callback functions");
        return AL_FALSE;
    }

    UserCallbacks newcb;
    newcb.open_file = open_file;
    newcb.open_mem  = open_memory;
    newcb.get_fmt   = get_format;
    newcb.decode    = decode;
    newcb.rewind    = rewind;
    newcb.close     = close;

    InstalledCallbacks[index] = newcb;

    return AL_TRUE;
}


/* Function: alureSleep
 *
 * Rests the calling thread for the given number of seconds.
 *
 * Returns:
 * AL_FALSE on error.
 */
ALURE_API ALboolean ALURE_APIENTRY alureSleep(ALfloat duration)
{
    if(duration < 0.0f)
    {
        SetError("Invalid duration");
        return AL_FALSE;
    }

    ALuint seconds = (ALuint)duration;
    ALfloat rest = duration - (ALfloat)seconds;

#ifdef HAVE_NANOSLEEP

    struct timespec t, remainingTime;
    t.tv_sec = (time_t)seconds;
    t.tv_nsec = (long)(rest*1000000)*1000;

    while(nanosleep(&t, &remainingTime) < 0 && errno == EINTR)
        t = remainingTime;

#elif defined(HAVE_WINDOWS_H)

    while(seconds > 0)
    {
        Sleep(1000);
        seconds--;
    }
    Sleep((DWORD)(rest * 1000));

#endif

    return AL_TRUE;
}


/* Function: alureGetProcAddress
 *
 * Returns a pointer for the named ALURE function.
 *
 * Returns:
 * NULL on error.
 */
ALURE_API void* ALURE_APIENTRY alureGetProcAddress(const ALchar *funcname)
{
    std::map<std::string,void*>::iterator i = FunctionList.find(funcname);
    if(i != FunctionList.end())
        return i->second;

    SetError("Function not found");
    return NULL;
}

} // extern "C"
