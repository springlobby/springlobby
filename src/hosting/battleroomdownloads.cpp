#include "battleroomdownloads.h"
#include <string>
#include <wx/gauge.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include "../downloader/downloadsobserver.h"
#include <map>
#include  "../ibattle.h"
#include <lslunitsync/unitsync.h>

BattleRoomDownloads::BattleRoomDownloads(wxWindow* parent, IBattle* battle):
    wxPanel( parent, -1, wxDefaultPosition, wxSize(-1, 80), wxNO_BORDER ),
    m_battle(battle)
{
    wxBoxSizer* m_main_sizer = new wxBoxSizer(wxVERTICAL);
    {
        wxBoxSizer* m_map_sizer = new wxBoxSizer(wxHORIZONTAL);
        {
            m_map_text=new wxStaticText(this,-1,_("Map: "));
            {
                m_map_sizer->Add( m_map_text, 0, wxALL, 2 );
            }
            m_map_progress=new wxGauge(this,-1,100);
            {
                m_map_sizer->Add( m_map_progress, 1, wxALL, 2 );
            }
            m_main_sizer->Add( m_map_sizer, 0, wxALL | wxEXPAND, 2 );
        }

        wxBoxSizer* m_mod_sizer = new wxBoxSizer( wxHORIZONTAL );
        {
            m_mod_text=new wxStaticText(this,-1,_("Mod: "));
            {
                m_mod_sizer->Add( m_mod_text, 0, wxALL, 2 );
            }
            m_mod_progress=new wxGauge(this,-1,100);
            {
                m_mod_sizer->Add( m_mod_progress, 1, wxALL, 2 );
            }
            m_main_sizer->Add( m_mod_sizer, 0, wxALL | wxEXPAND, 2 );
        }
        SetSizer( m_main_sizer );
    }

	Layout();

}

BattleRoomDownloads::~BattleRoomDownloads()
{

}

void BattleRoomDownloads::SetProgress(ObserverDownloadInfo obi,wxGauge* g)
{
    g->SetRange(obi.size);
    g->SetValue(obi.progress);
    g->SetLabel(wxString::Format(wxT("%i%%"),(int)((double)100.0*obi.progress/(double)obi.size)));
}

void BattleRoomDownloads::NoDownload(wxGauge* g)
{
    g->SetRange(100);
    g->SetValue(0);
    g->SetLabel(_T(""));
}

void BattleRoomDownloads::SetBattle( IBattle* battle )
{
    m_battle=battle;
}

void BattleRoomDownloads::OnUpdate()
{
    if(!m_battle)
    {
        NoDownload(m_map_progress);
        NoDownload(m_mod_progress);
        return;
    }

    DownloadsObserver& observ=downloadsObserver();
    std::map<wxString,ObserverDownloadInfo> dlmap;
    observ.GetMap(dlmap);

    try
    {
        wxString map=m_battle->GetHostMapName();
        ObserverDownloadInfo obi=dlmap.at(map);
        SetProgress(obi,m_map_progress);
    }
    catch(...)
    {
        NoDownload(m_map_progress);
    }

    try
    {
        wxString mod=m_battle->GetHostModName();
        ObserverDownloadInfo obi=dlmap.at(mod);
        SetProgress(obi,m_mod_progress);
    }
    catch(...)
    {
        NoDownload(m_mod_progress);
    }
}
















