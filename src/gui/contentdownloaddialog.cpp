/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "contentdownloaddialog.h"
#include "contentsearchresultslistctrl.h"
#include "utils/conversion.h"
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/protocol/http.h>
#include <wx/sstream.h>
#include <wx/thread.h>
#include "../json/wx/jsonreader.h"
#include "ui.h"
#include <lslunitsync/unitsync.h>

#include <iostream>
DECLARE_EVENT_TYPE(SEARCH_FINISHED, wxID_ANY);
DEFINE_EVENT_TYPE(SEARCH_FINISHED);
BEGIN_EVENT_TABLE( ContentDownloadDialog, wxDialog )
	EVT_BUTTON(		SEARCH_BUTTON, ContentDownloadDialog::OnSearch )
	EVT_BUTTON(		CLOSE_BUTTON, ContentDownloadDialog::OnCloseButton )
	EVT_BUTTON(		DOWNLOAD_BUTTON, ContentDownloadDialog::OnDownloadButton )
	EVT_COMMAND(ID_SEARCH_FINISHED,SEARCH_FINISHED,ContentDownloadDialog::OnSearchCompleted )
	EVT_LIST_ITEM_ACTIVATED ( LAUNCH_DOWNLOAD ,              ContentDownloadDialog::OnListDownload      )
END_EVENT_TABLE()

class SearchThread : public wxThread
{
public:
	virtual void* Entry();
	SearchThread(ContentDownloadDialog * content_dialog,wxString searchquery);
	virtual ~SearchThread();

private:

	ContentDownloadDialog* m_content_dialog;
	wxString m_search_query;

};

SearchThread::SearchThread(ContentDownloadDialog * content_dialog,wxString searchquery):
	wxThread(wxTHREAD_DETACHED),
	m_content_dialog(content_dialog),
	m_search_query(searchquery)
{
}

// convert a string to IRI: https://en.wikipedia.org/wiki/Internationalized_resource_identifier
static wxString ConvToIRI(const wxString& str)
{
	std::string utf8(str.mb_str(wxMBConvUTF8()));
	wxString escaped;
	for (unsigned i=0; i<utf8.length(); i++) {
		const unsigned char c = utf8[i];
		if ( (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ) {
			escaped.append(wxChar(c));
		} else { /* if (i+1<utf8.length())*/
			escaped.append(wxString::Format(_T("%%%02x"),wxChar(c)));
		}
		//FIXME: this function is incomplete! tested only with german umlauts
	}
	return escaped;
}

void* SearchThread::Entry()
{
	const wxString searchescaped = ConvToIRI(m_search_query);


//   std::cout << "Escaped search query: " << m_search_query.ToAscii().data() << std::endl;
	wxHTTP get;
	get.SetTimeout(10);
	get.Connect(_("api.springfiles.com"));
	const wxString query = wxFormat(_("/json.php?nosensitive=on&logical=or&springname=%s&tag=%s"))  % searchescaped % searchescaped;
	wxInputStream * httpStream = get.GetInputStream(query);
	wxString res;
	if ( get.GetError() == wxPROTO_NOERR ) {

		wxStringOutputStream out_stream(&res);
		httpStream->Read(out_stream);


	}
	wxDELETE(httpStream);
	wxCommandEvent notify(SEARCH_FINISHED,ContentDownloadDialog::ID_SEARCH_FINISHED);
	notify.SetInt(0);
	notify.SetString(res);
	wxPostEvent(m_content_dialog,notify);
//   std::cout << "Search finished" << std::endl;
	return NULL;
}

SearchThread::~SearchThread()
{

}


ContentDownloadDialog::ContentDownloadDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long int style, const wxString& name):
	wxDialog(parent, id, title, pos, size, style, name),
	WindowAttributesPickle( _T("CONTENTDIALOG"), this, wxSize( 670, 400 )),
	wildcardsearch(false)
{
	m_search_thread = NULL;
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	{
		m_search_res_w = new ContentSearchResultsListctrl(this,LAUNCH_DOWNLOAD);
		{
			m_main_sizer->Add(m_search_res_w,2,wxALL | wxEXPAND, 5);
		}
		m_searchsizer = new wxBoxSizer( wxHORIZONTAL );
		{
			m_searchbox = new wxTextCtrl(this,-1);
			{
				m_searchbox->SetToolTip(_("Enter search query (wildcard * can be used)"));
				m_searchsizer->Add(m_searchbox,1,wxALL,5);
			}
			m_searchbutton = new wxButton(this,SEARCH_BUTTON,_("Search"));
			{
				m_searchsizer->Add(m_searchbutton,0,wxALL,5);

			}
			m_download_button = new wxButton(this,DOWNLOAD_BUTTON,_("Download"));
			{
				m_searchsizer->Add(m_download_button,0,wxALL,5);

			}
			m_close_button = new wxButton(this,CLOSE_BUTTON,_("Close"));
			{
				m_searchsizer->Add(m_close_button,0,wxALL,5);
			}
			m_main_sizer->Add(m_searchsizer,0,wxEXPAND,5);
		}
		SetSizer( m_main_sizer );
	}
	Layout();

	m_searchbutton->SetDefault();
	m_searchbox->SetFocus();
}

bool ContentDownloadDialog::Show(bool show)
{
	return wxDialog::Show(show);
}

ContentDownloadDialog::~ContentDownloadDialog()
{
	if ( m_search_thread ) {
		m_search_thread->Wait();
	}
}
void ContentDownloadDialog::OnSearch(wxCommandEvent& /*event*/)
{
	wxString search_query = m_searchbox->GetValue();
	m_searchbutton->Enable(false);
	m_search_thread = new SearchThread(this,search_query);
	m_search_thread->Create();
	m_search_thread->Run();
}
void ContentDownloadDialog::OnSearchCompleted(wxCommandEvent& event)
{
	wxString json = event.GetString();
//   std::cout << json.ToAscii().data() << std::endl;

	wxJSONReader reader;
	wxJSONValue root;
	int errors = reader.Parse(json,&root);
	m_search_thread = NULL;
	m_searchbutton->Enable(true);
	if ( errors ) {
		wxMessageBox(_("Failed to parse search results"),_("Error"));
		return;
	}
	const wxJSONInternalArray * a = root.AsArray();
	if ((a->GetCount() == 0) && (!wildcardsearch)) { //no results returned, try wildcard search
		wildcardsearch = true;
		wxString search_query = _T("*")+m_searchbox->GetValue()+_T("*");//By default the user would expect that
		m_searchbutton->Enable(false);
		m_search_thread = new SearchThread(this,search_query);
		m_search_thread->Create();
		m_search_thread->Run();
		return;
	}
	wildcardsearch = false;
	m_search_res_w->Clear();

	for ( unsigned i = 0; i < a->GetCount(); i++ ) {
		wxJSONValue val = a->Item(i);
		wxString category = val[_("category")].AsString();
		int size = val[_("size")].AsInt();
		wxString name = val[_("springname")].AsString();
//     std::cout << category.ToAscii().data() << "," << name.ToAscii().data() << "," << size << std::endl;
		ContentSearchResult* res = new ContentSearchResult();
		res->name = name;
		res->filesize = size;
		res->type = category;
		if(category==_("map"))
			res->is_downloaded=LSL::usync().MapExists(std::string(name.mb_str()));
		else if(category==_("game"))
			res->is_downloaded=LSL::usync().ModExists(std::string(name.mb_str()));
		else
			res->is_downloaded=0;

		m_search_res_w->AddContent(res);
	}
}

void ContentDownloadDialog::OnCloseButton(wxCommandEvent& /*event*/)
{
	Close();
}

void ContentDownloadDialog::OnDownloadButton( wxCommandEvent& event)
{
	long item_index=m_search_res_w->GetNextItem(-1,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);
	if(item_index!=-1) {
		const ContentSearchResult * res = m_search_res_w->GetDataFromIndex(item_index);
		ui().Download(res->type, res->name, wxEmptyString);
	}
}

void ContentDownloadDialog::OnListDownload(wxListEvent& event)
{
	const ContentSearchResult * res = m_search_res_w->GetDataFromIndex(event.GetIndex());
	ui().Download(res->type, res->name, wxEmptyString);
}

