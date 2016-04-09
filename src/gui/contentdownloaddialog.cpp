/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "contentdownloaddialog.h"
#include "contentsearchresultview.h"
#include "utils/conversion.h"

#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/msgdlg.h>

#include "downloader/lib/src/Downloader/Http/HttpDownloader.h" //FIXME: remove this
#include "downloader/prdownloader.h"

#include "httpfile.h"
#include "ui.h"
#include "servermanager.h"
#include "utils/slpaths.h"

#include <lslunitsync/unitsync.h>

DECLARE_EVENT_TYPE(SEARCH_FINISHED, wxID_ANY);
DEFINE_EVENT_TYPE(SEARCH_FINISHED);
BEGIN_EVENT_TABLE(ContentDownloadDialog, wxDialog)
EVT_BUTTON(SEARCH_BUTTON, ContentDownloadDialog::OnSearch)
EVT_BUTTON(CLOSE_BUTTON, ContentDownloadDialog::OnCloseButton)
EVT_BUTTON(DOWNLOAD_BUTTON, ContentDownloadDialog::OnDownloadButton)
EVT_COMMAND(ID_SEARCH_FINISHED, SEARCH_FINISHED, ContentDownloadDialog::OnSearchCompleted)
EVT_DATAVIEW_ITEM_ACTIVATED(LAUNCH_DOWNLOAD, ContentDownloadDialog::OnListDownload)
END_EVENT_TABLE()

ContentDownloadDialog::ContentDownloadDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long int style, const wxString& name)
    : wxDialog(parent, id, title, pos, size, style, name)
    , WindowAttributesPickle(_T("CONTENTDIALOG"), this, wxSize(670, 400))
    , wildcardsearch(false)
{
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	{
		m_search_res_w = new ContentSearchResultView(this, LAUNCH_DOWNLOAD, _T("ContentDownloadDialog_ContentSearchResultView"));
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

static std::list<IDownload*> dls;

void ContentDownloadDialog::Search(const wxString& str)
{
	//FIXME: make async!
	const std::string param = ConvToIRI(STD_STRING(str));
	const std::string query = stdprintf("http://api.springfiles.com/json.php?nosensitive=on&logical=or&springname=%s&tag=%s", param.c_str(), param.c_str());
	std::string res;
	CHttpDownloader::DownloadUrl(query, res);
	CHttpDownloader::ParseResult(STD_STRING(str), res, dls);
	wxCommandEvent e;
	OnSearchCompleted(e);
}

void ContentDownloadDialog::OnSearch(wxCommandEvent& /*event*/)
{
	Search(m_searchbox->GetValue());
}

void ContentDownloadDialog::OnSearchCompleted(wxCommandEvent& /*event*/)
{
	assert(wxThread::IsMain());

	if (dls.empty() && (!wildcardsearch)) { //no results returned, try wildcard search
		wildcardsearch = true;
		const wxString search_query = _T("*") + m_searchbox->GetValue() + _T("*"); //By default the user would expect that
		Search(search_query);
		return;
	}
	wildcardsearch = false;
	m_search_res_w->Clear();

	for (const IDownload* dl : dls) {
		ContentSearchResult* res = new ContentSearchResult();
		res->name = dl->origin_name;
		res->filesize = dl->size;
		res->type = DownloadEnum::getCat(dl->cat);
		res->category = dl->cat;
		switch (dl->cat) {
			case DownloadEnum::CAT_MAP:
				res->is_downloaded = LSL::usync().MapExists(dl->origin_name);
				break;
			case DownloadEnum::CAT_GAME:
				res->is_downloaded = LSL::usync().GameExists(dl->origin_name);
				break;
			case DownloadEnum::CAT_ENGINE: //FIXME: check if platform matches / filter out different platform (?)
			case DownloadEnum::CAT_ENGINE_LINUX:
			case DownloadEnum::CAT_ENGINE_LINUX64:
			case DownloadEnum::CAT_ENGINE_MACOSX:
			case DownloadEnum::CAT_ENGINE_WINDOWS:
				res->is_downloaded = !SlPaths::GetCompatibleVersion(dl->version).empty();
				break;
			default:
				res->is_downloaded = false;
		}
		m_search_res_w->AddContent(*res);
	}
	IDownloader::freeResult(dls);
}

void ContentDownloadDialog::OnCloseButton(wxCommandEvent& /*event*/)
{
	Close();
}

void ContentDownloadDialog::OnDownloadButton(wxCommandEvent& /*event*/)
{
	wxDataViewEvent dummy;
	OnListDownload(dummy);
}

void ContentDownloadDialog::OnListDownload(wxDataViewEvent& /*event*/)
{
	const ContentSearchResult* res = static_cast<ContentSearchResult*>(m_search_res_w->GetSelection().GetID());

	if (res == nullptr) {
		return;
	}
	prDownloader().Download(res->category, STD_STRING(res->name));
}
