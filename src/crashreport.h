#ifndef CRASHREPORT_H_INCLUDED
#define CRASHREPORT_H_INCLUDED

#if wxUSE_DEBUGREPORT

class DebugReport : public wxDebugReportUpload
{
public:
    DebugReport() : wxDebugReportUpload
                       (
                        _T("http://"),
                        _T("report:file"),
                        _T("action")
                       )
    {
    }

protected:
    virtual bool OnServerReply(const wxArrayString& reply){}
};

class CrashReport
{
  public:

    CrashReport(){}

    void GenerateReport(wxDebugReport::Context ctx);

  protected:

    bool m_uploadReport;
};

#else // wxUSE_DEBUGREPORT

class CrashReport
{
  public:

    CrashReport() {}

    void GenerateReport(wxDebugReport::Context ctx);
};

#endif // !wxUSE_DEBUGREPORT

CrashReport& m_crash_report();

#endif // SPRINGLOBBY_HEADERGUARD_CRASHREPORT_H_INCLUDED
