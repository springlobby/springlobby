/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
//

#include <wx/app.h>
#include <wx/log.h>

#include "replaythread.h"
#include "../settings++/custom_dialogs.h"
#include "../utils.h"
#include "../globalevents.h"


ReplayLoader::ReplayLoader( ReplayList& list )
    : m_thread_loader( new ReplayLoaderThread ( list ) )
{}

ReplayLoader::~ReplayLoader()
{
}

ReplayLoader::ReplayLoaderThread::ReplayLoaderThread( ReplayList& list )
    :   m_destroy(false),
        m_replays( list )


{
    Init();
}

ReplayLoader::ReplayLoaderThread::~ReplayLoaderThread()
{
}

void ReplayLoader::ReplayLoaderThread::Init()
{
    Create();
    Run();
}


void* ReplayLoader::ReplayLoaderThread::Entry()
{

    //download failed
//
//        wxCommandEvent notice(ExeDownloadEvt,GetId());
//        notice.SetInt( FileDownloading.GetError() );
//        wxPostEvent( &SL_GlobalEvtHandler::GetSL_GlobalEvtHandler(), notice );

    return NULL;
}

bool ReplayLoader::ReplayLoaderThread::TestDestroy()
{
    return m_destroy;
}


void ReplayLoader::ReplayLoaderThread::CloseThread()
{
    m_destroy = true;
}
