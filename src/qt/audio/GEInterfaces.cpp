/**
 *
 * GE::GA General interfaces
 * tuomo.hirvonen@digia.com
 *
 */

#include <memory.h>
#include "GEInterfaces.h"


using namespace GE;

/**
 * CAudioSource
 * common functionality
 *
 */
IAudioSource::IAudioSource() {
    m_next = 0;
};

IAudioSource::~IAudioSource() {


};

/**
 * CAudioMixer
 *
 */
CAudioMixer::CAudioMixer() {
    m_sourceList = 0;
    m_mixingBuffer = 0;
    m_mixingBufferLength = 0;
    m_fixedGeneralVolume = 4096;
};


CAudioMixer::~CAudioMixer() {
    destroyList();
    if (m_mixingBuffer) {
        delete [] m_mixingBuffer;
        m_mixingBuffer = 0;
    };
};

void CAudioMixer::destroyList() {
    m_mutex.lock();
    IAudioSource *l = m_sourceList;
    while (l) {
        IAudioSource *n = l->m_next;
        delete l;
        l = n;
    };
    m_sourceList = 0;
    m_mutex.unlock();
};


IAudioSource* CAudioMixer::addAudioSource( IAudioSource *source ) {
    m_mutex.lock();
    source->m_next = 0;
    if (m_sourceList) {
        IAudioSource *l = m_sourceList;
        while (l->m_next) l = l->m_next;
        l->m_next = source;
    } else m_sourceList = source;
    m_mutex.unlock();
    return source;

};


bool CAudioMixer::removeAudioSource( IAudioSource *source ) {
    return true;
};

void CAudioMixer::setGeneralVolume( float vol ) {
    m_fixedGeneralVolume = (4096.0f*vol);
};

int CAudioMixer::pullAudio( AUDIO_SAMPLE_TYPE *target, int bufferLength ) {

    if (!m_sourceList) return 0;

    m_mutex.lock();


    if (m_mixingBufferLength<bufferLength) {
        if (m_mixingBuffer) delete [] m_mixingBuffer;
        m_mixingBufferLength = bufferLength;
        m_mixingBuffer = new AUDIO_SAMPLE_TYPE[ m_mixingBufferLength ];
    };

    memset( target, 0, sizeof( AUDIO_SAMPLE_TYPE ) * bufferLength );

    AUDIO_SAMPLE_TYPE *t;
    AUDIO_SAMPLE_TYPE *t_target;
    AUDIO_SAMPLE_TYPE *s;

    IAudioSource *prev = 0;
    IAudioSource *l = m_sourceList;
    while (l) {
        IAudioSource *next = l->m_next;

            // process l
        int mixed = l->pullAudio( m_mixingBuffer, bufferLength );
        if (mixed>0) {
            // mix to main..
            t = target;
            t_target = t+mixed;
            s = m_mixingBuffer;
            while (t!=t_target) {
                *t +=(((*s)*m_fixedGeneralVolume)>>12);
                t++;
                s++;
            };
        };



        // autodestroy
        if (l->canBeDestroyed() == true) {          // NOTE, IS UNDER TESTING,... MIGHT CAUSE UNPREDICTABLE CRASHING WITH SOME USE CASES!!!
            if (!prev)
                m_sourceList = next;
            else prev->m_next = next;
            delete l;
            l = 0;
        };




        prev = l;
        l = next;
    };
    m_mutex.unlock();
    return bufferLength;
};




