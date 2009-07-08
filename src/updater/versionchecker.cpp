#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/sstream.h>
#include <wx/protocol/http.h>
#include <wx/log.h>

#include "../utils/debug.h"
#include "../settings++/custom_dialogs.h"


//! @brief gets latest version from version.springlobby.info via HTTP
wxString GetLatestVersion()
{
  wxHTTP versionRequest;

  versionRequest.SetHeader(_T("Content-type"), _T("text/html; charset=utf-8"));
  // normal timeout is 10 minutes.. set to 10 secs.
  versionRequest.SetTimeout(10);
  versionRequest.Connect( _T("version.springlobby.info"), 80);
  wxInputStream *stream = versionRequest.GetInputStream( _T("/latest.txt") );
  wxString result;

  if (versionRequest.GetError() == wxPROTO_NOERR)
  {

    wxStringOutputStream output(&result);
    stream->Read(output);
  }
  else
  {
    wxString err;
    switch (versionRequest.GetError())
    {

      case wxPROTO_NETERR:
        err = _T("Network Error");
        break;
      case wxPROTO_PROTERR:
        err = _T("Negotiation error");
        break;
      case wxPROTO_CONNERR:
        err = _T("Failed to connect to server");
        break;
      case wxPROTO_INVVAL:
        err = _T("Invalid Value");
        break;
      case wxPROTO_NOHNDLR:
        err = _T("No Handler");
        break;
      case wxPROTO_NOFILE:
        err = _T("File doesn't exit");
        break;
      case wxPROTO_ABRT:
        err = _T("Action Aborted");
        break;
      case wxPROTO_RCNCT:
        err = _T("Reconnection Error");
        break;
      default:
        err = _T("Unknown Error");
        break;
    }

    wxLogDebugFunc(_T("Error connecting! Error is: ") + err);
    customMessageBoxNoModal(SL_MAIN_ICON, _T("Error connecting! Error is: ") + err, _T(""));

    return _T("-1");
  }

  wxDELETE(stream);
  versionRequest.Close();

    // Need to replace crap chars or versions will always be inequal
  result.Replace(_T(" "), _T(""), true);
  result.Replace(_T("\n"), _T(""), true);
  result.Replace(_T("\t"), _T(""), true);

  return result;
}
