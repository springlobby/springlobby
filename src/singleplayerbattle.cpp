/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "singleplayerbattle.h"

#include <lslutils/conversion.h>
#include <wx/log.h>
#include <stdexcept>

#include "gui/hosting/mainsingleplayertab.h"
#include "iserver.h"
#include "settings.h"
#include "spring.h"
#include "utils/conversion.h"
#include "utils/lslconversion.h"
#include "utils/slconfig.h"

SLCONFIG("/Spring/DefaultName", "Player", "playername for singleplayer battles");

SinglePlayerBattle::SinglePlayerBattle(MainSinglePlayerTab& msptab)
    : m_sptab(msptab)
    , m_me(STD_STRING(cfg().ReadString("/Spring/DefaultName")))
{
	OnUserAdded(m_me);
	m_me.BattleStatus().side = sett().GetBattleLastSideSel(TowxString(GetHostGameNameAndVersion()));
	m_me.BattleStatus().colour = wxColourTolsl(sett().GetBattleLastColour());
	CustomBattleOptions().setSingleOption("startpostype", LSL::Util::ToIntString(ST_Pick), LSL::Enum::EngineOption);
}


SinglePlayerBattle::~SinglePlayerBattle()
{
}


void SinglePlayerBattle::SendHostInfo(HostInfo update)
{
	if ((update & HI_StartType) != 0)
		m_sptab.UpdateMinimap();
	if ((update & HI_Restrictions) != 0)
		m_sptab.ReloadRestrictions();
	if ((update & HI_Map_Changed) != 0) {
		LoadMap();
		m_sptab.ReloadMapOptContrls();
	}
	if ((update & HI_Game_Changed) != 0) {
		RemoveUnfittingBots();
		LoadGame();
		wxString presetname = sett().GetModDefaultPresetName(TowxString(GetHostGameNameAndVersion()));
		if (!presetname.IsEmpty()) {
			LoadOptionsPreset(STD_STRING(presetname));
			SendHostInfo(HI_Send_All_opts);
		}
		m_sptab.ReloadModOptContrls();
	}
}


void SinglePlayerBattle::StartSpring()
{
	spring().Run(*this);
}
