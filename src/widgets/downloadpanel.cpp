#include "downloadpanel.h"

#include <wx/protocol/http.h>
#include <wx/xml/xml.h>


WidgetDownloadPanel::WidgetDownloadPanel(wxWindow* parent, wxWindowID id, const wxString& title,
    const wxPoint& pos , const wxSize& size , long style )
    : wxScrolledWindow (parent,  id, pos, size, style)
{
    PopulateList();
}

WidgetDownloadPanel::~WidgetDownloadPanel()
{
    //dtor
}

bool WidgetDownloadPanel::PopulateList()
{
    bool success = true;
	wxHTTP http;

	http.SetTimeout(6);
	http.Connect(_T("spring.vsync.de"));
        // PHP file sending XML content
	wxInputStream *httpStream = http.GetInputStream(_T("/luaManager/lua_manager.php?m=0"));

	if (http.GetError() == wxPROTO_NOERR)
	{
                // will crash here, if xml content is not formatted PERFECTLY
		wxXmlDocument xml(*httpStream);

		wxXmlNode *node = xml.GetRoot()->GetChildren();
		while (node)
		{
//			if (node->GetName() == _T("tagname1"))
//				staticText1->SetLabel(node->GetNodeContent());
//			else if(node->GetName() == _T("tagname2"))
//				staticText2->SetLabel(node->GetNodeContent());

                        // [...]

			node = node->GetNext();
		}
	}
	else
		success = false;

	http.Close();
	wxDELETE(httpStream);

	return success;
}
