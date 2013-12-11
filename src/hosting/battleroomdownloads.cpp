#include <string>
#include <wx/gauge.h>
#include "battleroomdownloads.h"
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include "../downloader/downloadsobserver.h"
#include <map>
#include  "../ibattle.h"

class BattleRoomDownloadProgres: public wxPanel
{
private:
        wxGauge* m_progres;
        wxStaticText* m_info;
        wxStaticText* m_text;
        wxBoxSizer* m_main_sizer;
public:
    BattleRoomDownloadProgres(wxWindow* parent,const wxString& str_text):
        wxPanel( parent, -1, wxDefaultPosition, wxSize(-1, 80), wxNO_BORDER )
    {
        m_main_sizer=new wxBoxSizer( wxHORIZONTAL );
        {
            wxBoxSizer* m_data_sizer=new wxBoxSizer( wxHORIZONTAL );//wxHORIZONTAL
            {
                m_text=new wxStaticText(this,-1,str_text);
                {
                    m_data_sizer->Add( m_text, 0, wxALL, 2 );
                }

                m_info=new wxStaticText(this,-1,_(" "),wxDefaultPosition,wxSize(-1,-1),wxALIGN_CENTRE );
                {
                    m_data_sizer->Add( m_info, 0, wxALL, 2 );
                }
                m_main_sizer->Add( m_data_sizer, 0, wxALL, 2 );
            }

            m_progres=new wxGauge(this,-1,100,wxDefaultPosition, wxDefaultSize, wxGA_SMOOTH|wxGA_HORIZONTAL );
            {
                m_main_sizer->Add((wxWindow*) m_progres, 1, wxALL|wxEXPAND, 2 );
            }
            SetSizer( m_main_sizer );
        }
        Layout();
    }

    void Update(ObserverDownloadInfo dl_info)
    {
        m_progres->SetRange(dl_info.size);
        m_progres->SetValue(dl_info.progress);
        m_info->SetLabel(wxString::Format(wxT("%i%%"),(int)((double)100.0*dl_info.progress/(double)dl_info.size)));
    }
};


BattleRoomDownloads::BattleRoomDownloads(wxWindow* parent, IBattle* battle):
    wxPanel( parent, -1, wxDefaultPosition, wxSize(-1, 80), wxNO_BORDER ),
    m_battle(battle)
{
    wxBoxSizer* m_main_sizer = new wxBoxSizer( wxHORIZONTAL );//wxVERTICAL
    {
        m_mod=new BattleRoomDownloadProgres(this,_T("Mod "));
        {
            m_main_sizer->Add((wxWindow*) m_mod, 1, wxALL|wxEXPAND, 2 );
        }
        m_map=new BattleRoomDownloadProgres(this,_T("Map "));
        {
            m_main_sizer->Add((wxWindow*) m_map, 1, wxALL|wxEXPAND, 2 );
        }
        SetSizer( m_main_sizer );
    }
	Layout();
}

BattleRoomDownloads::~BattleRoomDownloads()
{

}

void BattleRoomDownloads::SetBattle( IBattle* battle )
{
    m_battle=battle;
}

void BattleRoomDownloads::OnUpdate()
{
    if(!m_battle)
    {
        m_mod->Hide();
        m_map->Hide();
        return;
    }

    DownloadsObserver& observ=downloadsObserver();
    std::map<wxString,ObserverDownloadInfo> dlmap;
    observ.GetMap(dlmap);

    try
    {
        wxString game=m_battle->GetHostModName();
        ObserverDownloadInfo obi=dlmap.at(game);
        m_mod->Show();
        m_mod->Update(obi);
    }
    catch(...)
    {
        m_mod->Hide();
    }

    try
    {
        wxString map=m_battle->GetHostMapName();
        ObserverDownloadInfo obi=dlmap.at(map);
        m_map->Show();
        m_map->Update(obi);
    }
    catch(...)
    {
        m_map->Hide();
    }
}
















