#ifndef CRASHREPORT_H_INCLUDED
#define CRASHREPORT_H_INCLUDED

#ifdef ENABLE_DEBUG_REPORT

#include <wx/debugrpt.h>
#include "utils.h"
#if wxUSE_DEBUGREPORT

#include <sstream>
#include <wx/arrstr.h>

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

    CrashReport(){}
    void GenerateReport(wxDebugReport::Context ctx);
    //! @brief is the container for the stream logging target
    std::ostringstream crashlog;
};

CrashReport& crashreport();

#endif // wxUSE_DEBUGREPORT

#endif //ENABLE_DEBUG_REPORT

#endif // SPRINGLOBBY_HEADERGUARD_CRASHREPORT_H_INCLUDED

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

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

