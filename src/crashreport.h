/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef CRASHREPORT_H_INCLUDED
#define CRASHREPORT_H_INCLUDED


#include <wx/setup.h>
#include <wx/debugrpt.h>
#include <wx/arrstr.h>

#include "utils/platform.h"
#include <sstream>

//! @brief uploads zipped stacktraces using curl
class NetDebugReport : public wxDebugReportCompress
{
  public:
	  NetDebugReport( const char* url );

	  //! the real workhorse
      bool Process();

  protected:
      //! @brief gets called after server answered to the upload attempt
      bool OnServerReply(const wxArrayString& reply);
	  const char* m_url;
};

class SpringDebugReport : public NetDebugReport {
	public:
		SpringDebugReport();
protected:
		void AddVFSFile( const wxString& fn, const wxString& id );

};

//! @brief dumps various infos to the zipped debug info package and calls NetDebugReport if network is present
class CrashReport
{
  public:

    static CrashReport& instance()
    {
      static CrashReport c;
      return c;
    }

    #if wxUSE_STACKWALKER && !__WXMSW__
        void GenerateReport();
    #else
        void GenerateReport(EXCEPTION_POINTERS* p);
    #endif

        //! @brief is the container for the stream logging target
        std::ostringstream crashlog;

    protected:
        CrashReport(){}

};

#endif // SPRINGLOBBY_HEADERGUARD_CRASHREPORT_H_INCLUDED
