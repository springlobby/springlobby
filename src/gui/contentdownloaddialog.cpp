/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "contentdownloaddialog.h"
#include "contentsearchresultslistctrl.h"
#include "utils/conversion.h"
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/msgdlg.h>
#include "json/wx/jsonreader.h"
#include "httpfile.h"
#include "ui.h"
#include <lslunitsync/unitsync.h>

DECLARE_EVENT_TYPE(SEARCH_FINISHED, wxID_ANY);
DEFINE_EVENT_TYPE(SEARCH_FINISHED);
BEGIN_EVENT_TABLE(ContentDownloadDialog, wxDialog)
EVT_BUTTON(SEARCH_BUTTON, ContentDownloadDialog::OnSearch)
EVT_BUTTON(CLOSE_BUTTON, ContentDownloadDialog::OnCloseButton)
EVT_BUTTON(DOWNLOAD_BUTTON, ContentDownloadDialog::OnDownloadButton)
EVT_COMMAND(ID_SEARCH_FINISHED, SEARCH_FINISHED, ContentDownloadDialog::OnSearchCompleted)
EVT_LIST_ITEM_ACTIVATED(LAUNCH_DOWNLOAD, ContentDownloadDialog::OnListDownload)
END_EVENT_TABLE()

ContentDownloadDialog::ContentDownloadDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long int style, const wxString& name)
    : wxDialog(parent, id, title, pos, size, style, name)
    , WindowAttributesPickle(_T("CONTENTDIALOG"), this, wxSize(670, 400))
    , wildcardsearch(false)
{
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	{
		m_search_res_w = new ContentSearchResultsListctrl(this, LAUNCH_DOWNLOAD);
		{
			m_main_sizer->Add(m_search_res_w, 2, wxALL | wxEXPAND, 5);
		}
		m_searchsizer = new wxBoxSizer(wxHORIZONTAL);
		{
			m_searchbox = new wxTextCtrl(this, -1);
			{
				m_searchbox->SetToolTip(_("Enter search query (wildcard * can be used)"));
				m_searchsizer->Add(m_searchbox, 1, wxALL, 5);
			}
			m_searchbutton = new wxButton(this, SEARCH_BUTTON, _("Search"));
			{
				m_searchsizer->Add(m_searchbutton, 0, wxALL, 5);
			}
			m_download_button = new wxButton(this, DOWNLOAD_BUTTON, _("Download"));
			{
				m_searchsizer->Add(m_download_button, 0, wxALL, 5);
			}
			m_close_button = new wxButton(this, CLOSE_BUTTON, _("Close"));
			{
				m_searchsizer->Add(m_close_button, 0, wxALL, 5);
			}
			m_main_sizer->Add(m_searchsizer, 0, wxEXPAND, 5);
		}
		SetSizer(m_main_sizer);
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
}


void ContentDownloadDialog::Search(const wxString& str)
{
	//FIXME: use pr-downloader and make async!
	const wxString param = ConvToIRI(str);
	const wxString query = wxString::Format(_("http://api.springfiles.com/json.php?nosensitive=on&logical=or&springname=%s&tag=%s"), param.c_str(), param.c_str());
	const wxString json = GetHttpFile(query);
	wxCommandEvent e;
	e.SetString(json);
	OnSearchCompleted(e);
}

void ContentDownloadDialog::OnSearch(wxCommandEvent& /*event*/)
{
	Search(m_searchbox->GetValue());
}

void ContentDownloadDialog::OnSearchCompleted(wxCommandEvent& event)
{
	assert(wxThread::IsMain());

	wxString json = event.GetString();
	//   std::cout << json.ToAscii().data() << std::endl;

	wxJSONReader reader;
	wxJSONValue root;
	int errors = reader.Parse(json, &root);
	m_searchbutton->Enable(true);
	if (errors) {
		wxMessageBox(wxString::Format(_T("Failed to parse search results:\n%s"), json.c_str()), _("Error"));
		return;
	}
	const wxJSONInternalArray* a = root.AsArray();
	if ((a->GetCount() == 0) && (!wildcardsearch)) { //no results returned, try wildcard search
		wildcardsearch = true;
		const wxString search_query = _T("*") + m_searchbox->GetValue() + _T("*"); //By default the user would expect that
		Search(search_query);
		return;
	}
	wildcardsearch = false;
	m_search_res_w->Clear();

	for (unsigned i = 0; i < a->GetCount(); i++) {
		wxJSONValue val = a->Item(i);
		wxString category = val[_("category")].AsString();

		long size = 0;
		if (val[_("size")].IsInt()) {
			size = val[_("size")].AsInt();
		} else if (val[_("size")].IsLong()) {
			size = val[_("size")].AsLong();
		}
		wxString name = val[_("springname")].AsString();
		//     std::cout << category.ToAscii().data() << "," << name.ToAscii().data() << "," << size << std::endl;
		ContentSearchResult* res = new ContentSearchResult();
		res->name = name;
		res->filesize = size;
		res->type = category;
		if (category == _("map"))
			res->is_downloaded = LSL::usync().MapExists(std::string(name.mb_str()));
		else if (category == _("game"))
			res->is_downloaded = LSL::usync().ModExists(std::string(name.mb_str()));
		else
			res->is_downloaded = 0;

		m_search_res_w->AddContent(res);
	}
}

void ContentDownloadDialog::OnCloseButton(wxCommandEvent& /*event*/)
{
	Close();
}

void ContentDownloadDialog::OnDownloadButton(wxCommandEvent& /*event*/)
{
	long item_index = m_search_res_w->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (item_index != -1) {
		const ContentSearchResult* res = m_search_res_w->GetDataFromIndex(item_index);
		ui().Download(STD_STRING(res->type), STD_STRING(res->name), "");
	}
}

void ContentDownloadDialog::OnListDownload(wxListEvent& event)
{
	const ContentSearchResult* res = m_search_res_w->GetDataFromIndex(event.GetIndex());
	ui().Download(STD_STRING(res->type), STD_STRING(res->name), "");
}
