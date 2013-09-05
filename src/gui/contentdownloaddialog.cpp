/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2013  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
#include <wx/jsonreader.h>
#include "ui.h"

#include <iostream>
DECLARE_EVENT_TYPE(SEARCH_FINISHED, wxID_ANY);
DEFINE_EVENT_TYPE(SEARCH_FINISHED);
BEGIN_EVENT_TABLE( ContentDownloadDialog, wxDialog )
	EVT_BUTTON(		SEARCH_BUTTON, ContentDownloadDialog::OnSearch )
	EVT_BUTTON(		CLOSE_BUTTON, ContentDownloadDialog::OnCloseButton )
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

SearchThread::SearchThread(ContentDownloadDialog * content_dialog,wxString searchquery): wxThread(wxTHREAD_DETACHED)
{
  m_content_dialog = content_dialog;
  m_search_query = searchquery;
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
		} else/* if (i+1<utf8.length())*/ {
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
  if ( get.GetError() == wxPROTO_NOERR )
  {

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


ContentDownloadDialog::ContentDownloadDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long int style, const wxString& name):  wxDialog(parent, id, title, pos, size, style, name), WindowAttributesPickle( _T("CONTENTDIALOG"), this, wxSize( 670, 400 ))
{
  m_search_thread = NULL;
  m_main_sizer = new wxBoxSizer(wxVERTICAL);
  m_search_res_w = new ContentSearchResultsListctrl(this,LAUNCH_DOWNLOAD);
  m_main_sizer->Add(m_search_res_w,2,wxALL | wxEXPAND, 3);
  m_searchsizer = new wxBoxSizer( wxHORIZONTAL );
  m_searchbox = new wxTextCtrl(this,-1);
  m_searchbutton = new wxButton(this,SEARCH_BUTTON,_("Search"));
  m_searchlabel = new wxStaticText(this,-1,_("Enter search query(wildcard * can be used):"));
  m_searchsizer->Add(m_searchlabel);
  m_searchsizer->Add(m_searchbox,1,wxEXPAND | wxRIGHT ,0);
  m_searchsizer->Add(m_searchbutton);
  m_close_button = new wxButton(this,CLOSE_BUTTON,_("Close"));
  m_searchsizer->Add(m_close_button);
  m_main_sizer->Add(m_searchsizer,0,wxEXPAND | wxTOP,1);
  SetSizer( m_main_sizer );
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
  if ( m_search_thread )
  {
      m_search_thread->Wait();
  }
}
void ContentDownloadDialog::OnSearch(wxCommandEvent& /*event*/)
{
  wxString search_query = _T("*")+m_searchbox->GetValue()+_T("*");//By default the user would expect that
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
  if ( errors )
  {
    wxMessageBox(_("Failed to parse search results"),_("Error"));
    return;
  }
  const wxJSONInternalArray * a = root.AsArray();
  m_search_res_w->Clear();
  for ( unsigned i = 0; i < a->GetCount(); i++ )
  {
    wxJSONValue val = a->Item(i);
    wxString category = val[_("category")].AsString();
    int size = val[_("size")].AsInt();
    wxString name = val[_("springname")].AsString();
//     std::cout << category.ToAscii().data() << "," << name.ToAscii().data() << "," << size << std::endl;
    ContentSearchResult* res = new ContentSearchResult();
    res->name = name;
    res->filesize = size;
    res->type = category;
    m_search_res_w->AddContent(res);
  }
}

void ContentDownloadDialog::OnCloseButton(wxCommandEvent& /*event*/)
{
    Close();
}

void ContentDownloadDialog::OnListDownload(wxListEvent& event)
{
	const ContentSearchResult * res = m_search_res_w->GetDataFromIndex(event.GetIndex());
	ui().Download(res->type, res->name, wxEmptyString);
}

