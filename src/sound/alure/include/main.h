#ifndef MAIN_H
#define MAIN_H

#include "AL/alure.h"
#include "alext.h"

#ifdef HAS_SNDFILE
#include <sndfile.h>
#endif
#ifdef HAS_VORBISFILE
#include <vorbis/vorbisfile.h>
#endif
#ifdef HAS_FLAC
#include <FLAC/all.h>
#endif
#ifdef HAS_MPG123
#include <mpg123.h>
#endif
#ifdef HAS_GSTREAMER
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/app/gstappbuffer.h>
#endif


#ifdef HAVE_WINDOWS_H

#include <windows.h>

#else

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#include <assert.h>
#include <pthread.h>
#ifdef HAVE_PTHREAD_NP_H
#include <pthread_np.h>
#endif
#include <errno.h>

typedef pthread_mutex_t CRITICAL_SECTION;
static inline void EnterCriticalSection(CRITICAL_SECTION *cs)
{
    int ret;
    ret = pthread_mutex_lock(cs);
    assert(ret == 0);
}
static inline void LeaveCriticalSection(CRITICAL_SECTION *cs)
{
    int ret;
    ret = pthread_mutex_unlock(cs);
    assert(ret == 0);
}
static inline void InitializeCriticalSection(CRITICAL_SECTION *cs)
{
    pthread_mutexattr_t attrib;
    int ret;

    ret = pthread_mutexattr_init(&attrib);
    assert(ret == 0);

    ret = pthread_mutexattr_settype(&attrib, PTHREAD_MUTEX_RECURSIVE);
#ifdef HAVE_PTHREAD_NP_H
    if(ret != 0)
        ret = pthread_mutexattr_setkind_np(&attrib, PTHREAD_MUTEX_RECURSIVE);
#endif
    assert(ret == 0);
    ret = pthread_mutex_init(cs, &attrib);
    assert(ret == 0);

    pthread_mutexattr_destroy(&attrib);
}
static inline void DeleteCriticalSection(CRITICAL_SECTION *cs)
{
    int ret;
    ret = pthread_mutex_destroy(cs);
    assert(ret == 0);
}

#endif

#include <map>
#include <streambuf>
#include <istream>
#include <list>
#include <algorithm>

void SetError(const char *err);

struct UserCallbacks {
    void*     (*open_file)(const ALchar*);
    void*     (*open_mem)(const ALubyte*,ALuint);
    ALboolean (*get_fmt)(void*,ALenum*,ALuint*,ALuint*);
    ALuint    (*decode)(void*,ALubyte*,ALuint);
    ALboolean (*rewind)(void*);
    void      (*close)(void*);
};
extern std::map<ALint,UserCallbacks> InstalledCallbacks;


struct alureStream {
    ALubyte *data;

    ALubyte *dataChunk;
    ALsizei chunkLen;

    std::istream *fstream;

    virtual bool IsValid() = 0;
    virtual bool GetFormat(ALenum*,ALuint*,ALuint*) = 0;
    virtual ALuint GetData(ALubyte*,ALuint) = 0;
    virtual bool Rewind() = 0;

    alureStream(std::istream *_stream=NULL)
      : data(NULL), dataChunk(NULL), fstream(_stream)
    { StreamList.push_front(this); }
    virtual ~alureStream()
    {
        delete[] data; delete[] dataChunk;
        StreamList.erase(std::find(StreamList.begin(), StreamList.end(), this));
    }

    static void Clear(void)
    {
        while(StreamList.size() > 0)
            alureDestroyStream(*(StreamList.begin()), 0, NULL);
    }

    static bool Verify(alureStream *stream)
    {
        ListType::iterator i = std::find(StreamList.begin(), StreamList.end(), stream);
        return (i != StreamList.end());
    }

private:
    typedef std::list<alureStream*> ListType;
    static ListType StreamList;
};

struct MemDataInfo {
    const ALubyte *Data;
    size_t Length;
    size_t Pos;

    MemDataInfo() : Data(NULL), Length(0), Pos(0)
    { }
    MemDataInfo(const MemDataInfo &inf) : Data(inf.Data), Length(inf.Length),
                                          Pos(inf.Pos)
    { }
};

class MemStreamBuf : public std::streambuf {
    MemDataInfo memInfo;

    virtual int_type underflow();
    virtual pos_type seekoff(off_type offset, std::ios_base::seekdir whence, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);
    virtual pos_type seekpos(pos_type pos, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);

public:
    MemStreamBuf(const MemDataInfo &data)
      : memInfo(data)
    {
        memInfo.Pos /= sizeof(char_type);
        memInfo.Length /= sizeof(char_type);
    }
    virtual ~MemStreamBuf() { }
};

struct UserFuncs {
    void* (*open)(const char *filename, ALuint mode);
    void (*close)(void *f);
    ALsizei (*read)(void *f, ALubyte *buf, ALuint count);
    ALsizei (*write)(void *f, const ALubyte *buf, ALuint count);
    alureInt64 (*seek)(void *f, alureInt64 offset, int whence);
};
extern UserFuncs Funcs;

class FileStreamBuf : public std::streambuf {
    void *usrFile;
    UserFuncs fio;

    char buffer[1024];

    virtual int_type underflow();
    virtual pos_type seekoff(off_type offset, std::ios_base::seekdir whence, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);
    virtual pos_type seekpos(pos_type pos, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);

public:
    bool IsOpen()
    { return usrFile != NULL; }

    FileStreamBuf(const char *filename, ALint mode)
      : usrFile(NULL), fio(Funcs)
    { usrFile = fio.open(filename, mode); }
    virtual ~FileStreamBuf()
    { if(usrFile) fio.close(usrFile); }
};

class InStream : public std::istream {
public:
    bool IsOpen()
    {
        FileStreamBuf *fbuf = dynamic_cast<FileStreamBuf*>(rdbuf());
        if(fbuf) return fbuf->IsOpen();
        return true;
    }

    InStream(const char *filename)
      : std::istream(new FileStreamBuf(filename, 0))
    { }
    InStream(const MemDataInfo &memInfo)
      : std::istream(new MemStreamBuf(memInfo))
    { }
    virtual ~InStream()
    { delete rdbuf(); }
};


template <typename T>
alureStream *create_stream(const T &fdata);

extern CRITICAL_SECTION cs_StreamPlay;

#endif // MAIN_H
