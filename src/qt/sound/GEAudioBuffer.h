
/**
 *
 * GE::GA AudioBuffer functionality
 * tuomo.hirvonen@digia.com
 *
 */

#ifndef __GE_IGA_AUDIOBUFFER__
#define __GE_IGA_AUDIOBUFFER__

#include <QFile>
#include "GEInterfaces.h"


namespace GE {

    class CAudioBufferPlayInstance;
    class CAudioBuffer;         // forward declaration
    typedef AUDIO_SAMPLE_TYPE(*SAMPLE_FUNCTION_TYPE)(CAudioBuffer *abuffer, int pos, int channel);

    class CAudioBuffer {			// container for a sound
    public:
        CAudioBuffer();
        virtual ~CAudioBuffer();

        static CAudioBuffer* loadWav( QString fileName );
        static CAudioBuffer* loadWav( FILE *wavFile );          // support for stdio
        void reallocate( int length );


        inline void* getRawData() { return m_data; }
        inline int getDataLength() { return m_dataLength; }

        inline int getBytesPerSample() { return (m_bitsPerSample>>3); }
        inline int getBitsPerSample() { return m_bitsPerSample; }
        inline int getSamplesPerSec() { return m_samplesPerSec; }
        inline short getNofChannels() { return m_nofChannels; }
        inline SAMPLE_FUNCTION_TYPE getSampleFunction() { return m_sampleFunction; }


        // static implementations of sample functions
        static AUDIO_SAMPLE_TYPE sampleFunction8bitMono( CAudioBuffer *abuffer, int pos, int channel );
        static AUDIO_SAMPLE_TYPE sampleFunction16bitMono( CAudioBuffer *abuffer, int pos, int channel );
        static AUDIO_SAMPLE_TYPE sampleFunction8bitStereo( CAudioBuffer *abuffer, int pos, int channel );
        static AUDIO_SAMPLE_TYPE sampleFunction16bitStereo( CAudioBuffer *abuffer, int pos, int channel );

        CAudioBufferPlayInstance *playWithMixer( GE::CAudioMixer &mixer );

    protected:
        SAMPLE_FUNCTION_TYPE m_sampleFunction;
        short m_nofChannels;
        void *m_data;
        int m_dataLength;				// in bytes
        short m_bitsPerSample;
        bool m_signedData;
        int m_samplesPerSec;
    };



    class CAudioBufferPlayInstance : public IAudioSource {
    public:
        CAudioBufferPlayInstance();
        CAudioBufferPlayInstance( CAudioBuffer *start_playing );
        virtual ~CAudioBufferPlayInstance();
        void playBuffer( CAudioBuffer *startPlaying, float volume, float fixedSpeed, int loopTimes = 0 );			// looptimes -1 = loop forever

        void setSpeed( float speed );
        void setLeftVolume( float lvol );
        void setRightVolume( float rvol );


        inline void setLoopTimes( int ltimes ) { m_loopTimes = ltimes; }
        void stop();



        int pullAudio( AUDIO_SAMPLE_TYPE *target, int bufferLength );
        bool canBeDestroyed();

        bool isPlaying() { if (m_buffer) return true; else return false; }
        inline bool isFinished() { return m_finished; }
        inline bool destroyWhenFinished() { return m_destroyWhenFinished; }
        inline void setDestroyWhenFinished( bool set ) { m_destroyWhenFinished = set; }

    protected:
        int mixBlock( AUDIO_SAMPLE_TYPE *target, int bufferLength );
        bool m_finished;
        bool m_destroyWhenFinished;
        int m_fixedPos;
        int m_fixedInc;

        int m_fixedLeftVolume;
        int m_fixedRightVolume;
        int m_fixedCenter;
        int m_loopTimes;
        CAudioBuffer *m_buffer;
    };

};



#endif
