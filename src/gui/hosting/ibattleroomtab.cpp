/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "ibattleroomtab.h"

#include "battleroomtab.h"

IBattleRoomTab::IBattleRoomTab(wxWindow* parent, int id)
	: wxPanel(parent, id) {
}

IBattleRoomTab::~IBattleRoomTab() {
}

IBattleRoomTab* IBattleRoomTab::CreateInstance(wxWindow* parent,
		IBattle* battle) {
	return new BattleRoomTab(parent, battle);
}
