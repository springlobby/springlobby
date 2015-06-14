/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <stdexcept>
#include <wx/log.h>

#include "singleplayerbattle.h"
#include "gui/hosting/mainsingleplayertab.h"
#include "iserver.h"
#include "utils/slconfig.h"
#include "settings.h"
#include "spring.h"
#include <lslutils/conversion.h>
#include "utils/conversion.h"
#include "utils/lslconversion.h"

SLCONFIG("/Spring/DefaultName", "Player", "playername for singleplayer battles");

SinglePlayerBattle::SinglePlayerBattle(MainSinglePlayerTab& msptab)
    : m_sptab(msptab)
{
	m_me = STD_STRING(cfg().ReadString("/Spring/DefaultName"));
	OnUserAdded(m_me);
	m_me.BattleStatus().side = sett().GetBattleLastSideSel(TowxString(GetHostModName()));
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
		Update(stdprintf("%d_%s", LSL::Enum::PrivateOptions, "mapname"));
	}
	if ((update & HI_Mod_Changed) != 0) {
		RemoveUnfittingBots();
		LoadMod();
		wxString presetname = sett().GetModDefaultPresetName(TowxString(GetHostModName()));
		if (!presetname.IsEmpty()) {
			LoadOptionsPreset(STD_STRING(presetname));
			SendHostInfo(HI_Send_All_opts);
		}
		m_sptab.ReloadModOptContrls();
		Update(stdprintf("%d_%s", LSL::Enum::PrivateOptions, "modname"));
	}
	if ((update & HI_Send_All_opts) != 0) {
		for (int i = 0; i < (int)LSL::Enum::LastOption; i++) {
			for (const auto pair : CustomBattleOptions().getOptionsMap((LSL::Enum::GameOption)i)) {
				Update(stdprintf("%d_%s", i, pair.first.c_str()));
			}
		}
	}
}


void SinglePlayerBattle::StartSpring()
{
	spring().Run(*this);
}
