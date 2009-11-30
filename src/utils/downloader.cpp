#include "downloader.h"


#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER


#include <wx/protocol/http.h>
#include <wx/xml/xml.h>
#include <wx/wfstream.h>
#include <wx/sstream.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include "customdialogs.h"
#include "conversion.h"
#include "../base64.h"
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
    data.Replace( _T("REALNAME") , _T("DeltaSiegeDry.smf") );
    wxString host = _T("planet-wars.eu");
    wxString path = _T("/PlasmaServer/Service.asmx");//can be php or any other file


    //Set up header
    wxString header = _T("");

    //POST
    header += _T("POST http://planet-wars.eu/PlasmaServer/Service.asmx");
//    header += path;
    header += _T(" HTTP/1.1\n");
//    header += _T("Accept-Encoding: *\n");

    //Write host website name

//    //Write user agent
//    header += _T("User-Agent: HTTPTool/1.0\n");

    //Write content type
    header += _T("Content-Type: text/xml;charset=UTF-8\n");
    header += _T("SOAPAction: \"http://planet-wars.eu/PlasmaServer/DownloadFile\"\n");
//    header += _T("User-Agent: Jakarta Commons-HttpClient/3.1\n");

    header += _T("Host: ");
    header += host;
    header += _T("\n");

    //Write POST content length
    header += _T("Content-Length: ");
    header += wxString::Format(_T("%d"), data.Len());
    header += _T("\n\n");

    //Print on screen to make sure it looks right
//    wxMessageBox(header);
//    wxMessageBox(data);

    //Connect to host
    wxIPV4address * address = new wxIPV4address();
    address->Hostname(host);
    address->Service(80);

    socket->Connect(*address);
//    if (socket->Connect(*address))
//            wxMessageBox(_T("connected"));

    //Write header
    socket->Write(header.mb_str(),header.Len());
//    wxMessageBox(wxString::Format(_T("Wrote %d out of %d bytes"),socket->LastCount(),header.Len()));

    //Write data
    socket->Write(data.mb_str(),data.Len());
//    wxMessageBox(wxString::Format(_T("Wrote %d out of %d bytes"),socket->LastCount(),data.Len()));


    //Get Response
    socket->WaitForRead(10);
    char peek_buf[1025];
    socket->Peek(peek_buf,1025);
    peek_buf[socket->LastCount()] = '\0';

//    buf = wxString( buf, wxConvISO8859_1 );
    wxString wxbuf = wxString::  FromAscii( peek_buf );
    //msgbox also serves as wait thingy for socket read it seems here, remove and be prepared for less stuff read...
    wxMessageBox(wxString::Format(_T("Read %d bytes: %s"),socket->LastCount(),wxbuf.c_str()));

    long content_length = 0;
    wxStringTokenizer toks ( wxbuf, _T("\n") );
    while( toks.HasMoreTokens() ) {
        wxString line = toks.GetNextToken();
        if ( line.StartsWith( _T("Content-Length") ) ) {
            line = line.Mid( line.Last( wxChar(':') ) + 1, line.Last( wxChar('\n') ) ).Trim( false ).Trim( true );
            line.ToLong( &content_length );
            assert( content_length >= 0 );
            break;
        }
    }
    char buf[1025+content_length];
    socket->Read(buf,1025+content_length);

    buf[socket->LastCount()] = '\0';

//    buf = wxString( buf, wxConvISO8859_1 );
    wxbuf = wxString::  FromAscii( buf );
    wxMessageBox(wxString::Format(_T("Content size %d | Read %d bytes: %s"),content_length,socket->LastCount(),wxbuf.c_str()));

    wxString t_begin = _T("<soap:Envelope");
    wxString t_end = _T("</soap:Envelope>");
    wxString xml_section = wxbuf.Mid( wxbuf.Find( t_begin ) );//first char after t_begin to one before t_end

    wxMessageBox(xml_section);
    wxStringInputStream str_input( xml_section );
    wxXmlDocument xml( str_input );
    assert( xml.GetRoot() );
    wxXmlNode *node = xml.GetRoot()->GetChildren();
    assert( node );
    wxArrayString webseeds;
    node = node->GetChildren();
    assert( node );
    while ( node ) {
        wxString node_name = node->GetName();
        if ( node_name == _T("DownloadFileResponse") ) {
            wxXmlNode* downloadFileResult = node->GetChildren();
            assert( downloadFileResult );
            wxString result = downloadFileResult->GetNodeContent();
            //check result
            wxXmlNode* links = downloadFileResult->GetNext();
            assert( links );
            wxXmlNode* url = links->GetChildren();
            while ( url ) {
                webseeds.Add( url->GetNodeContent() );
                url = url->GetNext();
            }
            wxXmlNode* next = links->GetNext();
            while ( next ) {
                wxString next_name = next->GetName();
                if ( next_name == _T("torrentFileName") ) {
                    wxString tor_name = next->GetNodeContent();
                    wxString dl_target = wxString::Format( _T("/tmp/%s"), tor_name.c_str() );
                    downloadFile( host, _T("PlasmaServer/Resources/") + tor_name, dl_target );
                    break;
                }
                next = next->GetNext();
            }
            break;
        }
        node = node->GetNext();
    }
    wxString seeds;
    for ( size_t i = 0; i < webseeds.Count(); ++i )
        seeds += webseeds[i] + _T("\n");
    wxMessageBox( seeds );
    return webseeds;
}

void downloadFile( const wxString& host, const wxString& remote_path, const wxString& local_dest )
{
    wxHTTP FileDownloading;
    /// normal timeout is 10 minutes.. set to 10 secs.
    FileDownloading.SetTimeout(10);
    FileDownloading.Connect( host, 80);

    wxInputStream* httpstream = FileDownloading.GetInputStream( _T("/") + remote_path );

    if ( httpstream )
    {
          wxFileOutputStream outs( local_dest );
          httpstream->Read(outs);
          outs.Close();
          delete httpstream;
          httpstream = 0;
          //download success
    }
    else {
        //throw something
    }
}
