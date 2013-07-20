#include <string>
#include <wx/gauge.h>
#include "battleroomdownloads.h"
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
    wxBoxSizer* m_main_sizer = new wxBoxSizer( wxHORIZONTAL );//wxVERTICAL
    {
        wxBoxSizer* m_map_sizer= new wxBoxSizer( wxHORIZONTAL ); //wxVERTICAL
        {
            wxBoxSizer* m_map_data_size=new wxBoxSizer( wxHORIZONTAL );//wxHORIZONTAL
            {
                m_map_text=new wxStaticText(this,-1,_("Map "));
                {
                    m_map_data_size->Add( m_map_text, 0, wxALL, 2 );
                }

                m_map_info=new wxStaticText(this,-1,_T(""),wxDefaultPosition,wxSize(-1,-1),wxALIGN_CENTRE );
                {
                    m_map_data_size->Add( m_map_info, 0, wxALL, 2 );
                }
                m_map_sizer->Add( m_map_data_size, 0, wxALL, 2 );
            }

            m_map_progress=new wxGauge(this,-1,100,wxDefaultPosition, wxDefaultSize, wxGA_SMOOTH|wxGA_HORIZONTAL );
            {
                m_map_sizer->Add((wxWindow*) m_map_progress, 1, wxALL|wxEXPAND, 2 );
            }

            m_main_sizer->Add( m_map_sizer, 1, wxALL|wxEXPAND, 2 );
        }

        wxBoxSizer* m_game_sizer= new wxBoxSizer( wxHORIZONTAL );
        {
            wxBoxSizer* m_game_data_size=new wxBoxSizer( wxHORIZONTAL );
            {
                m_game_text=new wxStaticText(this,-1,_("Game "));
                {
                    m_game_data_size->Add( m_game_text, 0, wxALL, 2 );
                }

                m_game_info=new wxStaticText(this,-1,_T(""),wxDefaultPosition,wxSize(-1,-1),wxALIGN_CENTRE );
                {
                    m_game_data_size->Add( m_game_info, 0, wxALL, 2 );
                }
                m_game_sizer->Add( m_game_data_size, 0, wxALL, 2 );
            }

            m_game_progress=new wxGauge(this,-1,100,wxDefaultPosition, wxDefaultSize, wxGA_SMOOTH|wxGA_HORIZONTAL );
            {
                m_game_sizer->Add( m_game_progress, 1, wxALL|wxEXPAND, 2 );
            }

            m_main_sizer->Add( m_game_sizer, 1, wxALL|wxEXPAND, 2 );
        }
        SetSizer( m_main_sizer );
    }
	Layout();
}

BattleRoomDownloads::~BattleRoomDownloads()
{

}

void BattleRoomDownloads::SetProgress(ObserverDownloadInfo obi,wxStaticText* txt,wxGauge* g)
{
    g->SetRange(obi.size);
    g->SetValue(obi.progress);

    txt->SetLabel(wxString::Format(wxT("%i%%"),(int)((double)100.0*obi.progress/(double)obi.size)));
}

void BattleRoomDownloads::NoDownload(wxStaticText* txt,wxGauge* g)
{
    g->SetRange(100);
    g->SetValue(0);
    txt->SetLabel(_T("NoDownload"));
}

void BattleRoomDownloads::SetBattle( IBattle* battle )
{
    m_battle=battle;
}

void BattleRoomDownloads::OnUpdate()
{
    if(!m_battle)
    {
        NoDownload(m_map_info,m_map_progress);
        NoDownload(m_game_info,m_game_progress);
        return;
    }

    DownloadsObserver& observ=downloadsObserver();
    std::map<wxString,ObserverDownloadInfo> dlmap;
    observ.GetMap(dlmap);

    try
    {
        wxString map=m_battle->GetHostMapName();
        ObserverDownloadInfo obi=dlmap.at(map);
        SetProgress(obi,m_map_info,m_map_progress);
    }
    catch(...)
    {
        NoDownload(m_map_info,m_map_progress);
    }

    try
    {
        wxString game=m_battle->GetHostModName();
        ObserverDownloadInfo obi=dlmap.at(game);
        SetProgress(obi,m_game_info,m_game_progress);
    }
    catch(...)
    {
        NoDownload(m_game_info,m_game_progress);
    }
}
















