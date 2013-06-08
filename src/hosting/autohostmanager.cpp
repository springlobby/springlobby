#include "autohostmanager.h"

#include "../battle.h"

AutohostHandler::AutohostHandler():m_battle(0)
{
}

AutohostHandler::~AutohostHandler()
{

}

void AutohostHandler::SetBattle(Battle* battle)
{
    m_battle=battle;
}

//========================
//-------- Springie ------
//========================

SpringieHandler::SpringieHandler():AutohostHandler()
{

}

SpringieHandler::~SpringieHandler()
{

}

void SpringieHandler::Balance()
{
    m_battle->Say(_T("!balance"));
}

void SpringieHandler::SetRandomMap()
{
    m_battle->Say(_T("!map"));
}

void SpringieHandler::SetMap(wxString& map)
{
    m_battle->Say(_T("!map")+map);
}

void SpringieHandler::ClearStartBoxes()
{
    m_battle->Say(_T("!clear")); /// will check
}

void SpringieHandler::AddStartBox(int posx,int posy,int w,int h)
{
    m_battle->Say(wxString::Format(wxT("!addbox %i %i %i %i"),posx,posy,w,h));
}

void SpringieHandler::Notify()
{
    m_battle->Say(_T("!notify"));
}
//------------------------------

//========================
//-------- Spads ---------
//========================
SpadsHandler::SpadsHandler():AutohostHandler()
{

}

SpadsHandler::~SpadsHandler()
{

}

void SpadsHandler::Balance()
{
    m_battle->Say(_T("!balance"));
}

void SpadsHandler::SetRandomMap()
{
    m_battle->Say(_T("!map 1")); //not so random
}

void SpadsHandler::SetMap(wxString& map)
{
    m_battle->Say(_T("!map")+map);
}

void SpadsHandler::ClearStartBoxes()
{

}

void SpadsHandler::AddStartBox(int posx,int posy,int w,int h)
{

}

void SpadsHandler::Notify()
{
    m_battle->Say(_T("!notify"));
}

//-------------


AutohostManager::AutohostManager():m_type(AUTOHOSTTYPE_UNKNOWN), m_battle(0)
{

}

AutohostManager::~AutohostManager()
{

}

void AutohostManager::SetBattle(Battle* battle)
{
    m_battle=battle;
    m_springie.SetBattle(battle);
    m_spads.SetBattle(battle);
}

AutohostHandler& AutohostManager::GetAutohostHandler()
{
    switch(m_type)
    {
        case AUTOHOSTTYPE_SPRINGIE:
            return GetSpringie();
        case AUTOHOSTTYPE_SPADS:
            return GetSpads();
    }
    throw "Unknown autohost";
}

SpringieHandler& AutohostManager::GetSpringie()
{
    return m_springie;
}

SpadsHandler& AutohostManager::GetSpads()
{
    return m_spads;
}

void AutohostManager::RecnognizeAutohost()
{
}

AutohostManager::AutohostType AutohostManager::GetAutohostType()
{
    return m_type;
}
