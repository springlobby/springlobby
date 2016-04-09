/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "ibattlemaptab.h"

#include "battlemaptab.h"

IBattleMapTab::IBattleMapTab(wxWindow* parent, int id) :
		wxPanel(parent, id) {
}

IBattleMapTab::~IBattleMapTab() {
}

IBattleMapTab* IBattleMapTab::CreateInstance(wxWindow* parent,
		IBattle* battle) {
	return new BattleMapTab(parent, battle);
}
