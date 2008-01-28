#ifndef CRASHREPORT_H_INCLUDED
#define CRASHREPORT_H_INCLUDED



#if wxUSE_DEBUGREPORT && !defined(HAVE_WX26)
#include <wx/debugrpt.h>
#include <sstream>
#include <wx/arrstr.h>
//! @brief uploads zipped stacktraces using curl
class NetDebugReport : public wxDebugReportUpload
{
  public:
      //! @brief class contructor that sets curl arguments
      NetDebugReport();

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

#endif // SPRINGLOBBY_HEADERGUARD_CRASHREPORT_H_INCLUDED
