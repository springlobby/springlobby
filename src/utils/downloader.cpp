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
#include <wx/tokenzr.h>

#include "customdialogs.h"//shoudl be remove d fater initlai debug
#include "conversion.h"
#include "debug.h"
#include "globalevents.h"
#include "../socket.h"
#include "../curl/http.h"
#include "../globalsmanager.h"
#include "conversion.h"

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

PlasmaInterface& plasmaInterface()
{
    static LineInfo<PlasmaInterface> m( AT );
	static GlobalObjectHolder<PlasmaInterface, LineInfo<PlasmaInterface> > m_plasma( m );
	return m_plasma;
}

/** @brief PlasmaInterface
  *
  * @todo: document this function
  */
PlasmaInterface::PlasmaInterface()
    : m_host ( _T("planet-wars.eu") ),
    m_remote_path ( _T("/PlasmaServer/Service.asmx") )
{

}

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

struct MemoryStruct {
  char *memory;
  size_t size;
};

/** @brief GetResourceInfo
  *
  * @todo: document this function
  */
PlasmaResourceInfo PlasmaInterface::GetResourceInfo(const wxString& name)
{
    const int index = 1;


    wxString data = s_soap_querytemplate;
    data.Replace( _T("REALNAME") , name );

    //Set up header
    wxArrayString header;// = _T("");
    //Write content type
    header.Add( wxT("Content-Type: text/xml;charset=UTF-8") );
    header.Add( wxT("SOAPAction: \"http://planet-wars.eu/PlasmaServer/DownloadFile\"") );
    header.Add( wxT("Expect:") );

//    header.Add( _T("Host: ") + m_host );

    //Write POST content length
    header.Add( wxString::Format(_T("Content-Length: %d"), data.Len()) );

    wxStringInputStream req ( data );
    wxStringOutputStream response;
    wxStringOutputStream rheader;
    CURL *curl_handle;
    curl_handle = curl_easy_init();
    struct curl_slist* m_pHeaders = NULL;
    m_pHeaders = curl_slist_append(m_pHeaders, "Content-Type: text/xml;charset=UTF-8") ;
    m_pHeaders = curl_slist_append(m_pHeaders, "SOAPAction: \"http://planet-wars.eu/PlasmaServer/DownloadFile\"") ;
    m_pHeaders = curl_slist_append(m_pHeaders, "Expect:") ;

    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, m_pHeaders);
    curl_easy_setopt(curl_handle, CURLOPT_URL, "http://planet-wars.eu/PlasmaServer/Service.asmx" );
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, wxcurl_stream_write);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, (void *)&rheader);
    curl_easy_setopt(curl_handle, CURLOPT_POST, TRUE);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE_LARGE, data.Len() );
    curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION, wxcurl_stream_read);
    curl_easy_setopt(curl_handle, CURLOPT_READDATA, (void*)&req);

    CURLcode ret = curl_easy_perform(curl_handle);

//    wxMessageBox( response.GetString() );

  /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);

    m_buffers[index] = response.GetString();

    return ParseResourceInfoData( index );
}

PlasmaResourceInfo PlasmaInterface::ParseResourceInfoData( const int buffer_index )
{
    PlasmaResourceInfo info;
    wxString wxbuf = m_buffers[buffer_index];
//{
//     wxFileOutputStream outs( _T("/tmp/sl.txt") );
//    wxStringInputStream fe( wxbuf ) ;
//    outs <<  fe;
//    outs.Close();
//}
    wxString t_begin = _T("<soap:Envelope");
    wxString t_end = _T("</soap:Envelope>");
    wxString xml_section = wxbuf.Mid( wxbuf.Find( t_begin ) );//first char after t_begin to one before t_end

    wxStringInputStream str_input( xml_section );
//    wxXmlDocument xml( str_input );
//    assert( xml.GetRoot() );
//    wxXmlNode *node = xml.GetRoot()->GetChildren();
//    assert( node );


wxXmlDocument xml;
xml.Load( _T("/tmp/sl.txt") );
//xml.Save( _T("/tmp/sl.txt") );
wxXmlNode *node = xml.GetRoot()->GetChildren();
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

    assert( info.m_webseeds.Count() > 0 );
//    assert( info.ToFile( _T("/tmp/dl.info") ) );
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
    m_resource_list = PlasmaResourceInfo::VectorFromFile( _T("plasmaresourcelist.sl") );
}

void PlasmaInterface::FetchResourceList()
{
    Socket* socket = new Socket( *this, true, true );
    const int index = 1 + m_buffers.size();
    m_socket_index[socket] = index;
    m_buffers[index] = wxEmptyString;

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
    socket->Connect(m_host,80);
    assert (  socket->State() == SS_Open );

    //Write header
    socket->Send(header+data);
    wxString g = socket->Receive();
    m_buffers[index] = g;
    ParseResourceListData( index );

    PlasmaResourceInfo::VectorToFile( m_resource_list, _T("plasmaresourcelist.sl") );

    GetGlobalEventSender( GlobalEvents::PlasmaResourceListParsed ).SendEvent( 0 );

}
void PlasmaInterface::ParseResourceListData( const int buffer_index )
{
//    buf = wxString( buf, wxConvISO8859_1 );
    wxString wxbuf = m_buffers[buffer_index];
    //msgbox also serves as wait thingy for socket read it seems here, remove and be prepared for less stuff read...

    wxString t_begin = _T("<soap:Envelope");
    wxString t_end = _T("</soap:Envelope>");
    wxString xml_section = wxbuf.Mid( wxbuf.Find( t_begin ) );//first char after t_begin to one before t_end

    wxStringInputStream str_input( xml_section );
    wxXmlDocument xml( str_input );
    xml.Save( _T("/tmp/sl.xml") );
    wxFileOutputStream outs( _T("/tmp/sl.txt") );
    wxStringInputStream fe( xml_section ) ;
    outs <<  fe;
    outs.Close();
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
            while ( resourceListResult ) {
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
                            resourceType = resourceDataContent->GetNodeContent();
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
                    resourceData = resourceData->GetNext();
                }
                resourceListResult = resourceListResult->GetNext();
            }
        } // end section <GetResourceListResponse/>
        node = node->GetNext();
    }
}

void PlasmaInterface::OnDataReceived( Socket* sock )
{
    if ( sock == 0 )
        return;

    wxString data = sock->Receive();
    int index = m_socket_index[sock];
    m_buffers[index] << data;

    if ( index < 0 ) //socket for infos
    {
    }
    else //socekt for list
    {
        ParseResourceListData( index );
    }
}


//wxString m_name;
//wxString m_torrent_filename;
//wxArrayString m_webseeds;
//wxArrayString m_dependencies;
//wxString m_local_torrent_filepath;
//unsigned int m_size_KB;
//ResourceType m_type;
wxString PlasmaResourceInfo::ToString() const
{
    //we save the number of elements in both arraytypes so we can use the same seperator for the whole line
    wxString num_deps = TowxString( m_dependencies.Count() );
    wxString num_seed = TowxString( m_webseeds.Count() );
    wxString sep(_T("\t") );
    //this takes care of the non array members
    wxString ret( num_seed + sep + num_deps + sep + m_name + sep
                + m_torrent_filename + sep + m_local_torrent_filepath
                + sep + TowxString( m_size_KB ) + sep + TowxString(  m_type )  );
    for ( size_t i = 0; i < m_dependencies.Count(); ++i )
        ret += sep + m_dependencies[i];
    for ( size_t i = 0; i < m_webseeds.Count(); ++i )
        ret += sep + m_webseeds[i];

    return ret;
}

void PlasmaResourceInfo::FromString( const wxString& str )
{
    wxString sep(_T("\t") );
    wxStringTokenizer tok( str, sep, wxTOKEN_RET_EMPTY_ALL );
    StringtokenizerVectorized vec( tok );
    size_t num_tokens = vec.size();
    if ( num_tokens < 6 )
    {
        wxLogError( _T("Loading PLasmaResourceInfo from string failed") ) ;
        return;
    }
    size_t num_deps = FromwxString<size_t>( vec[0] );
//    size_t num_seed = FromwxString<size_t>( vec[1] ); //not actually needed
    m_name = vec[2];
    m_torrent_filename = vec[3];
    m_local_torrent_filepath = vec[4];
    m_size_KB = FromwxString<unsigned int>( vec[5] );
    int rt = FromwxString<int>( vec[6] );
    switch ( rt ) {
        default: m_type = unknwon; break;
        case 1: m_type = map; break;
        case 0: m_type = mod; break;
    }
    m_dependencies.Clear();
    m_webseeds.Clear();
    size_t i = 7;
    for ( ; i < 7 + num_deps; ++i )
        m_dependencies.Add( vec[i] );
    for ( ; i < num_tokens; ++i )
        m_dependencies.Add( vec[i] );
}

PlasmaResourceInfo::PlasmaResourceInfo()
{}

void PlasmaInterface::OnCurlPerformEnd( wxCurlEndPerformEvent& evt )
{
    wxMessageBox( m_curl->GetResponseBody() );
}

BEGIN_EVENT_TABLE( PlasmaInterface, wxEvtHandler )
    EVT_CURL_END_PERFORM( -1, PlasmaInterface::OnCurlPerformEnd )
END_EVENT_TABLE()
