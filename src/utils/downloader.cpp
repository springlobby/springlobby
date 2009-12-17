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
#include <wx/convauto.h>
#include <wx/log.h>

#include "customdialogs.h"//shoudl be remove d fater initlai debug
#include "conversion.h"
#include "debug.h"


const wxString s_soap_service_url = _T("http://planet-wars.eu/PlasmaServer/Service.asmx?op=DownloadFile");

const wxString s_soap_querytemplate = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" \
"<soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\">\n"\
"   <soap12:Body>\n"\
"       <DownloadFile xmlns=\"http://planet-wars.eu/PlasmaServer/\">\n"\
"           <internalName>REALNAME</internalName>\n"\
"       </DownloadFile>\n"\
"   </soap12:Body>\n"\
"</soap12:Envelope>\0");

const wxString s_soap_querytemplate_resourcelist = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" \
"<soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\">\n"\
"   <soap12:Body>\n"\
"       <GetResourceList xmlns=\"http://planet-wars.eu/PlasmaServer/\" />\n"\
"   </soap12:Body>\n"\
"</soap12:Envelope>\0");


/** @brief PlasmaInterface
  *
  * @todo: document this function
  */
PlasmaInterface::PlasmaInterface()
    : m_host ( _T("planet-wars.eu") ),
    m_remote_path ( _T("/PlasmaServer/Service.asmx") )
{

}

/** @brief GetResourceInfo
  *
  * @todo: document this function
  */
PlasmaResourceInfo PlasmaInterface::GetResourceInfo(const wxString& name) const
{
    PlasmaResourceInfo info;
    wxSocketClient * socket = new wxSocketClient();
    wxString data = s_soap_querytemplate;
    data.Replace( _T("REALNAME") , name );

    //Set up header
    wxString header = _T("");

    //POST
    header += wxString::Format( _T("POST http://%s%s"), m_host.c_str(), m_remote_path.c_str() ) ;
    header += _T(" HTTP/1.1\n");

    //Write content type
    header += _T("Content-Type: text/xml;charset=UTF-8\n");
    header += _T("SOAPAction: \"http://planet-wars.eu/PlasmaServer/DownloadFile\"\n");

    header += _T("Host: ");
    header += m_host;
    header += _T("\n");

    //Write POST content length
    header += _T("Content-Length: ");
    header += wxString::Format(_T("%d"), data.Len());
    header += _T("\n\n");

    //Connect to host
    wxIPV4address * address = new wxIPV4address();
    address->Hostname(m_host);
    address->Service(80);

    socket->Connect(*address);

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

    wxString resourceType ( _T("unknown") );
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
                info.m_webseeds.Add( url->GetNodeContent() );
                url = url->GetNext();
            }
            wxXmlNode* next = links->GetNext();
            while ( next ) {
                wxString next_name = next->GetName();
                if ( next_name == _T("torrentFileName") ) {
                    info.m_torrent_filename = next->GetNodeContent();
                }
                else if ( next_name == _T("dependencies") ) {
                    wxXmlNode* deps = next->GetChildren();
                    while ( deps ) {
                        info.m_dependencies.Add( deps->GetNodeContent() );
                        deps = deps->GetNext();
                    }
                }
                else if ( next_name == _T("resourceType") ) {
                    resourceType = next->GetNodeContent();
                    if ( resourceType == _T("Mod") )
                        info.m_type = PlasmaResourceInfo::mod;
                    else if ( resourceType == _T("Map") )
                        info.m_type = PlasmaResourceInfo::map;
                    else
                        info.m_type = PlasmaResourceInfo::unknwon;
                }
                next = next->GetNext();
            }
            break;
        } // end section <DownloadFileResponse/>
        node = node->GetNext();
    }
    wxString seeds;
    for ( size_t i = 0; i < info.m_webseeds.Count(); ++i )
        seeds += info.m_webseeds[i] + _T("\n");
    wxMessageBox( seeds );


    return info;

}

/** @brief downloadFile
  *
  * @todo: document this function
  */
void PlasmaInterface::downloadFile(const wxString& host, const wxString& remote_path, const wxString& local_dest) const
{
    wxHTTP FileDownloading;
    /// normal timeout is 10 minutes.. set to 10 secs.
    FileDownloading.SetTimeout(10);
    FileDownloading.Connect( host, 80);

    wxInputStream* httpstream = FileDownloading.GetInputStream( _T("/") + remote_path );
    wxLogDebug( _T("downloading file ") + host + _T("/") + remote_path );

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
        throw std::exception();
    }
}

/** @brief DownloadTorrentFile
  *
  * @todo: document this function
  */
bool PlasmaInterface::DownloadTorrentFile( PlasmaResourceInfo& info, const wxString& destination_directory) const
{
    wxString dl_target = destination_directory + wxFileName::GetPathSeparator() + info.m_torrent_filename;
    try {
        downloadFile( m_host, _T("PlasmaServer/Resources/") + info.m_torrent_filename, dl_target );
        info.m_local_torrent_filepath = dl_target;
    }
    catch ( std::exception& e ) {
        wxLogError( _T("downloading file failed: :") + dl_target );
        return false;
    }
    return true;
}

void PlasmaInterface::InitResourceList()
{
    wxSocketClient * socket = new wxSocketClient();
    wxString data = s_soap_querytemplate_resourcelist;

    //Set up header
    wxString header = _T("");

    //POST
    header += wxString::Format( _T("POST http://%s%s"), m_host.c_str(), m_remote_path.c_str() ) ;
//    header += path;
    header += _T(" HTTP/1.1\n");
//    header += _T("Accept-Encoding: *\n");

    //Write host website name

//    //Write user agent
//    header += _T("User-Agent: HTTPTool/1.0\n");

    //Write content type
    header += _T("Content-Type: text/xml;charset=UTF-8\n");
    header += _T("SOAPAction: \"http://planet-wars.eu/PlasmaServer/GetResourceList\"\n");

    header += _T("Host: ");
    header += m_host;
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
    address->Hostname(m_host);
    address->Service(80);

    socket->Connect(*address);

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

    m_resource_list.clear();

    wxString resourceType ( _T("unknown") );
    node = node->GetChildren();
    assert( node );
    while ( node ) {
        wxString node_name = node->GetName();
        if ( node_name == _T("GetResourceListResponse") ) {
            wxXmlNode* resourceListResult = node->GetChildren();
            assert( resourceListResult );
            wxXmlNode* resourceData = resourceListResult->GetChildren();

            while ( resourceData ) {
                wxXmlNode* resourceDataContent = resourceData->GetChildren();
                PlasmaResourceInfo info;
                while ( resourceDataContent ) {
                    wxString rc_node_name = resourceDataContent->GetName();
                    if ( rc_node_name == _T("Dependencies") ){
                        //! TODO
                    }
                    else if ( rc_node_name == _T("InternalName") ){
                        info.m_name = resourceDataContent->GetNodeContent();
                    }
                    else if ( rc_node_name == _T("ResourceType") ){
                        wxString resourceType = resourceDataContent->GetNodeContent();
                        if ( resourceType == _T("Mod") )
                            info.m_type = PlasmaResourceInfo::mod;
                        else if ( resourceType == _T("Map") )
                            info.m_type = PlasmaResourceInfo::map;
                        else
                            info.m_type = PlasmaResourceInfo::unknwon;
                    }
                    else if ( rc_node_name == _T("SpringHashes") ){
                        //! TODO
                    }
                    resourceDataContent = resourceDataContent->GetNext();
                }
                m_resource_list.push_back( info );
                resourceData = resourceListResult->GetNext();
            }
            break;
        } // end section <GetResourceListResponse/>
        node = node->GetNext();
    }
}
