#include "downloader.h"


#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER


#include <wx/protocol/http.h>
#include <wx/xml/xml.h>
#include "customdialogs.h"
#include "utils/conversion.h"
#include <wx/convauto.h>

const wxString s_soap_service_url = _T("http://planet-wars.eu/PlasmaServer/Service.asmx?op=DownloadFile");

const wxString s_soap_querytemplate = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" \
"<soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\">\n"\
"   <soap12:Body>\n"\
"       <DownloadFile xmlns=\"http://planet-wars.eu/PlasmaServer/\">\n"\
"           <internalName>REALNAME</internalName>\n"\
"       </DownloadFile>\n"\
"   </soap12:Body>\n"\
"</soap12:Envelope>\0");

wxArrayString getDownloadLinks( const wxString& name ) {
    wxSocketClient * socket = new wxSocketClient();
    wxString data = s_soap_querytemplate;
    data.Replace( _T("REALNAME") , name );
    wxString host = _T("planet-wars.eu");
    wxString path = _T("/PlasmaServer/Service.asmx");//can be php or any other file


    //Set up header
    wxString header = _T("");

    //POST
    header += _T("POST ");
    header += path;
    header += _T(" HTTP/1.1\n");

    //Write host website name
    header += _T("Host: ");
    header += host;
    header += _T("\n");

//    //Write user agent
//    header += _T("User-Agent: HTTPTool/1.0\n");

    //Write content type
    header += _T("Content-Type: application/soap+xml; charset=utf-8\n");

    //Write POST content length
    header += _T("Content-Length: ");
    header += wxString::Format(_T("%d"), data.Len());
    header += _T("\n\n\n");

    //Print on screen to make sure it looks right
    wxMessageBox(header);
    wxMessageBox(data);

    //Connect to host
    wxIPV4address * address = new wxIPV4address();
    address->Hostname(host);
    address->Service(80);

    socket->Connect(*address);
//    if (socket->Connect(*address))
//            wxMessageBox(_T("connected"));

    //Write header
    socket->Write(header.c_str(),header.Len());
    wxMessageBox(wxString::Format(_T("Wrote %d out of %d bytes"),socket->LastCount(),header.Len()));

    //Write data
    socket->Write(data.c_str(),data.Len());
    wxMessageBox(wxString::Format(_T("Wrote %d out of %d bytes"),socket->LastCount(),data.Len()));


    //Get Response
    char buf[2049];
    socket->Read(buf,2049);


    buf[socket->LastCount()] = '\0';

//    buf = wxString( buf, wxConvISO8859_1 );
    wxString wxbuf = wxString::  FromAscii( buf );
    wxMessageBox(wxString::Format(_T("Read %d bytes: %s"),socket->LastCount(),wxbuf.c_str()));
    return wxArrayString();
}
