#ifndef CRASHREPORT_H_INCLUDED
#define CRASHREPORT_H_INCLUDED


#include <wx/setup.h>
#if wxUSE_DEBUGREPORT && defined(ENABLE_DEBUG_REPORT)

#include <wx/debugrpt.h>
#include "utils/platform.h"

#include <sstream>
#include <wx/arrstr.h>

#if !defined(wxUSE_STACKWALKER) || !wxUSE_STACKWALKER
    #include <windows.h>
    #include <process.h>
    #include <imagehlp.h>
    #include <signal.h>
#endif

//! @brief uploads zipped stacktraces using curl
class NetDebugReport : public wxDebugReportCompress
{
  public:
      //! @brief class contructor that sets curl arguments
      NetDebugReport();
      bool Process();

  protected:
      //! @brief gets called after server answered to the upload attempt
      bool OnServerReply(const wxArrayString& reply);
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

    #if wxUSE_STACKWALKER
        void GenerateReport();
    #else
        void GenerateReport(EXCEPTION_POINTERS* p);
    #endif

        //! @brief is the container for the stream logging target
        std::ostringstream crashlog;

    protected:
        CrashReport(){}

};


#endif //wxUSE_DEBUGREPORT && defined(ENABLE_DEBUG_REPORT)

#endif // SPRINGLOBBY_HEADERGUARD_CRASHREPORT_H_INCLUDED

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

