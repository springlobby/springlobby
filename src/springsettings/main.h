/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGSETTINGS_MAIN_h
#define SPRINGSETTINGS_MAIN_h

#include <wx/app.h>

class wxLocale;
class wxTranslationHelper;

class Springsettings : public wxApp
{
	public:
        Springsettings();
		~Springsettings();
		bool OnInit();
		int OnExit();
		virtual void OnFatalException();

        virtual void OnInitCmdLine(wxCmdLineParser& parser);
        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

    protected:

		wxTranslationHelper* m_translationhelper;

        long m_log_verbosity;
        bool m_log_console;
		bool m_log_file;
		wxString m_log_file_path;
        bool m_log_window_show;
        bool m_crash_handle_disable;

};

#endif
