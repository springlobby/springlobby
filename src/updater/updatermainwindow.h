/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_UPDATERMAINWINDOW_H
#define SPRINGLOBBY_HEADERGUARD_UPDATERMAINWINDOW_H

#include <wx/frame.h>

#include "../utils/globalevents.h"

class wxCloseEvent;

class UpdaterMainwindow : public wxFrame, public GlobalEvent {

    public:
        UpdaterMainwindow( const wxString& rev_string );
        virtual ~UpdaterMainwindow();

        void OnClose( wxCloseEvent& evt );

        void OnUpdateFinished( wxCommandEvent& /*data*/ );

    protected:
        DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_UPDATERMAINWINDOW_H
