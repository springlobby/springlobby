/**
 *
 * GE::GA Qt Audio out
 * tuomo.hirvonen@digia.com
 *
 */

#ifndef __GE_QTAUDIOOUT__
#define __GE_QTAUDIOOUT__

#include <QtCore/qobject.h>
#include <QtCore/qfile.h>
#include <QtMultimedia>
#include <QtCore/qtimer.h>
#include <QtCore/qstring.h>
#include <QThread>

#include "GEInterfaces.h"


class QAudioOutput;

namespace GE {



    class AudioOut : public QThread {
        Q_OBJECT

    public:
        AudioOut(QObject *parent, GE::IAudioSource *source);
        virtual ~AudioOut();



    private slots:
        void audioNotify();         // for internal notify "solution"


    protected:
         void tick();
        virtual void run();         // this is for the threaded mode only


        qint64 m_samplesMixed;

        QAudioOutput *m_audioOutput;
        QIODevice *m_outTarget;
        GE::IAudioSource *m_source;
        int m_runstate;
        AUDIO_SAMPLE_TYPE *m_sendBuffer;
        int m_sendBufferSize;
    };
}

#endif
