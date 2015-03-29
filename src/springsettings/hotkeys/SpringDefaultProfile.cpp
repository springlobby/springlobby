/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "SpringDefaultProfile.h"
#include "KeynameConverter.h"

key_binding SpringDefaultProfile::m_bindings;

void SpringDefaultProfile::initialize()
{
	if (SpringDefaultProfile::m_bindings.isEmpty() == false) {
		//is already initialized
		return;
	}

	SpringDefaultProfile::addBinding(_T("LastMsgPos"), _T("Any+f3"));
	SpringDefaultProfile::addBinding(_T("NoSound"), _T("Any+f6"));
	SpringDefaultProfile::addBinding(_T("ShowMetalMap"), _T("Any+f4"));
	SpringDefaultProfile::addBinding(_T("ShowPathMap"), _T("Any+f2"));
	SpringDefaultProfile::addBinding(_T("aiselect"), _T("Ctrl+q"));
	SpringDefaultProfile::addBinding(_T("areaattack"), _T("Alt+Shift+a"));
	SpringDefaultProfile::addBinding(_T("areaattack"), _T("Alt+a"));
	SpringDefaultProfile::addBinding(_T("attack"), _T("Shift+a"));
	SpringDefaultProfile::addBinding(_T("attack"), _T("a"));
	SpringDefaultProfile::addBinding(_T("buildfacing dec"), _T("Shift+]"));
	SpringDefaultProfile::addBinding(_T("buildfacing dec"), _T("]"));
	SpringDefaultProfile::addBinding(_T("buildfacing inc"), _T("Shift+["));
	SpringDefaultProfile::addBinding(_T("buildfacing inc"), _T("["));
	SpringDefaultProfile::addBinding(_T("buildspacing dec"), _T("Any+x"));
	SpringDefaultProfile::addBinding(_T("buildspacing inc"), _T("Any+z"));
	SpringDefaultProfile::addBinding(_T("chat"), _T("Any+enter"));
	SpringDefaultProfile::addBinding(_T("chatall"), _T("Ctrl+enter"));
	SpringDefaultProfile::addBinding(_T("chatally"), _T("Alt+enter"));
	SpringDefaultProfile::addBinding(_T("chatspec"), _T("Shift+enter"));
	SpringDefaultProfile::addBinding(_T("chatswitchall"), _T("Ctrl+enter"));
	SpringDefaultProfile::addBinding(_T("chatswitchally"), _T("Alt+enter"));
	SpringDefaultProfile::addBinding(_T("chatswitchspec"), _T("Shift+enter"));
	SpringDefaultProfile::addBinding(_T("cloak"), _T("Shift+k"));
	SpringDefaultProfile::addBinding(_T("cloak"), _T("k"));
	SpringDefaultProfile::addBinding(_T("controlunit"), _T("Any+c"));
	SpringDefaultProfile::addBinding(_T("createvideo"), _T("Ctrl+Shift+f10"));

	SpringDefaultProfile::addBinding(_T("debug"), _T("Alt+b"));
	SpringDefaultProfile::addBinding(_T("decguiopacity"), _T("Shift+,"));
	SpringDefaultProfile::addBinding(_T("decreaseViewRadius"), _T("Any+end"));
	SpringDefaultProfile::addBinding(_T("dgun"), _T("Shift+d"));
	SpringDefaultProfile::addBinding(_T("dgun"), _T("d"));
	SpringDefaultProfile::addBinding(_T("drawinmap"), _T("Any+\\"));
	SpringDefaultProfile::addBinding(_T("drawinmap"), _T("Any+`"));
	//SpringDefaultProfile::addBinding( _T("drawinmap"), _T("Any+0xa7") ); what key is this??
	SpringDefaultProfile::addBinding(_T("dynamicSky"), _T("Any+f7"));
	SpringDefaultProfile::addBinding(_T("edit_backspace"), _T("Any+backspace"));
	SpringDefaultProfile::addBinding(_T("edit_complete"), _T("Any+tab"));
	SpringDefaultProfile::addBinding(_T("edit_delete"), _T("Any+delete"));
	SpringDefaultProfile::addBinding(_T("edit_end"), _T("Alt+right"));
	SpringDefaultProfile::addBinding(_T("edit_end"), _T("Any+end"));
	SpringDefaultProfile::addBinding(_T("edit_escape"), _T("Any+escape"));
	SpringDefaultProfile::addBinding(_T("edit_home"), _T("Alt+left"));
	SpringDefaultProfile::addBinding(_T("edit_home"), _T("Any+home"));
	SpringDefaultProfile::addBinding(_T("edit_next_char"), _T("Any+right"));
	SpringDefaultProfile::addBinding(_T("edit_next_line"), _T("Any+down"));
	SpringDefaultProfile::addBinding(_T("edit_next_word"), _T("Ctrl+right"));
	SpringDefaultProfile::addBinding(_T("edit_prev_char"), _T("Any+left"));
	SpringDefaultProfile::addBinding(_T("edit_prev_line"), _T("Any+up"));
	SpringDefaultProfile::addBinding(_T("edit_prev_word"), _T("Ctrl+left"));
	SpringDefaultProfile::addBinding(_T("edit_return"), _T("Any+enter"));
	SpringDefaultProfile::addBinding(_T("fakemeta"), _T("space"));
	SpringDefaultProfile::addBinding(_T("fight"), _T("Shift+f"));
	SpringDefaultProfile::addBinding(_T("fight"), _T("f"));
	SpringDefaultProfile::addBinding(_T("forcestart"), _T("Alt+f"));
	SpringDefaultProfile::addBinding(_T("gameinfo"), _T("Any+i"));
	SpringDefaultProfile::addBinding(_T("group0"), _T("Any+0"));
	SpringDefaultProfile::addBinding(_T("group1"), _T("Any+1"));
	SpringDefaultProfile::addBinding(_T("group2"), _T("Any+2"));
	SpringDefaultProfile::addBinding(_T("group3"), _T("Any+3"));
	SpringDefaultProfile::addBinding(_T("group4"), _T("Any+4"));
	SpringDefaultProfile::addBinding(_T("group5"), _T("Any+5"));
	SpringDefaultProfile::addBinding(_T("group6"), _T("Any+6"));
	SpringDefaultProfile::addBinding(_T("group7"), _T("Any+7"));
	SpringDefaultProfile::addBinding(_T("group8"), _T("Any+8"));
	SpringDefaultProfile::addBinding(_T("group9"), _T("Any+9"));
	SpringDefaultProfile::addBinding(_T("groupadd"), _T("q"));
	SpringDefaultProfile::addBinding(_T("groupclear"), _T("Shift+q"));
	SpringDefaultProfile::addBinding(_T("groupselect"), _T("q"));
	SpringDefaultProfile::addBinding(_T("guard"), _T("Shift+g"));
	SpringDefaultProfile::addBinding(_T("guard"), _T("g"));
	SpringDefaultProfile::addBinding(_T("hideinterface"), _T("Any+f5"));
	SpringDefaultProfile::addBinding(_T("hotbind"), _T("Ctrl+insert"));
	SpringDefaultProfile::addBinding(_T("hotunbind"), _T("Ctrl+delete"));
	SpringDefaultProfile::addBinding(_T("incguiopacity"), _T("Shift+."));
	SpringDefaultProfile::addBinding(_T("increaseViewRadius"), _T("Any+home"));
	SpringDefaultProfile::addBinding(_T("loadunits"), _T("Shift+l"));
	SpringDefaultProfile::addBinding(_T("loadunits"), _T("l"));
	SpringDefaultProfile::addBinding(_T("mouse2"), _T("Any+j"));
	SpringDefaultProfile::addBinding(_T("mousestate"), _T("backspace"));
	SpringDefaultProfile::addBinding(_T("move"), _T("Shift+m"));
	SpringDefaultProfile::addBinding(_T("move"), _T("m"));
	SpringDefaultProfile::addBinding(_T("moveback"), _T("Any+down"));
	SpringDefaultProfile::addBinding(_T("movedown"), _T("Any+pagedown"));
	SpringDefaultProfile::addBinding(_T("movefast"), _T("Any+shift"));
	SpringDefaultProfile::addBinding(_T("moveforward"), _T("Any+up"));
	SpringDefaultProfile::addBinding(_T("moveleft"), _T("Any+left"));
	SpringDefaultProfile::addBinding(_T("moveright"), _T("Any+right"));
	SpringDefaultProfile::addBinding(_T("moveslow"), _T("Any+ctrl"));
	SpringDefaultProfile::addBinding(_T("moveup"), _T("Any+pageup"));
	SpringDefaultProfile::addBinding(_T("nextmenu"), _T("."));
	SpringDefaultProfile::addBinding(_T("onoff"), _T("Shift+x"));
	SpringDefaultProfile::addBinding(_T("onoff"), _T("x"));
	SpringDefaultProfile::addBinding(_T("pastetext"), _T("Ctrl+v"));
	SpringDefaultProfile::addBinding(_T("patrol"), _T("Shift+p"));
	SpringDefaultProfile::addBinding(_T("patrol"), _T("p"));
	SpringDefaultProfile::addBinding(_T("pause"), _T("Any+pause"));
	SpringDefaultProfile::addBinding(_T("prevmenu"), _T(","));
	SpringDefaultProfile::addBinding(_T("quitforce"), _T("Ctrl+Shift+esc"));
	SpringDefaultProfile::addBinding(_T("quitmenu"), _T("esc"));
	SpringDefaultProfile::addBinding(_T("reclaim"), _T("Shift+e"));
	SpringDefaultProfile::addBinding(_T("reclaim"), _T("e"));
	SpringDefaultProfile::addBinding(_T("repair"), _T("Shift+r"));
	SpringDefaultProfile::addBinding(_T("repair"), _T("r"));
	SpringDefaultProfile::addBinding(_T("savegame"), _T("Ctrl+Shift+f8"));
	SpringDefaultProfile::addBinding(_T("screenshot"), _T("Any+f11"));
	SpringDefaultProfile::addBinding(_T("screenshot"), _T("Any+f12"));
	SpringDefaultProfile::addBinding(_T("selfd"), _T("Ctrl+d"));
	SpringDefaultProfile::addBinding(_T("selfd queued"), _T("Ctrl+Shift+d"));
	SpringDefaultProfile::addBinding(_T("sharedialog"), _T("Any+h"));
	SpringDefaultProfile::addBinding(_T("showElevation"), _T("Any+f1"));
	SpringDefaultProfile::addBinding(_T("showhealthbars"), _T("Any+f9"));
	SpringDefaultProfile::addBinding(_T("singlestep"), _T("Alt+o"));
	SpringDefaultProfile::addBinding(_T("slowdown"), _T("Any+-"));
	SpringDefaultProfile::addBinding(_T("slowdown"), _T("Any+delete"));
	SpringDefaultProfile::addBinding(_T("slowdown"), _T("Any+numpad-"));
	SpringDefaultProfile::addBinding(_T("specteam 0"), _T("1"));
	SpringDefaultProfile::addBinding(_T("specteam 1"), _T("2"));
	SpringDefaultProfile::addBinding(_T("specteam 10"), _T("Ctrl+1"));
	SpringDefaultProfile::addBinding(_T("specteam 11"), _T("Ctrl+2"));
	SpringDefaultProfile::addBinding(_T("specteam 12"), _T("Ctrl+3"));
	SpringDefaultProfile::addBinding(_T("specteam 13"), _T("Ctrl+4"));
	SpringDefaultProfile::addBinding(_T("specteam 14"), _T("Ctrl+5"));
	SpringDefaultProfile::addBinding(_T("specteam 15"), _T("Ctrl+6"));
	SpringDefaultProfile::addBinding(_T("specteam 16"), _T("Ctrl+7"));
	SpringDefaultProfile::addBinding(_T("specteam 17"), _T("Ctrl+8"));
	SpringDefaultProfile::addBinding(_T("specteam 18"), _T("Ctrl+9"));
	SpringDefaultProfile::addBinding(_T("specteam 19"), _T("Ctrl+0"));
	SpringDefaultProfile::addBinding(_T("specteam 2"), _T("3"));
	SpringDefaultProfile::addBinding(_T("specteam 3"), _T("4"));
	SpringDefaultProfile::addBinding(_T("specteam 4"), _T("5"));
	SpringDefaultProfile::addBinding(_T("specteam 5"), _T("6"));
	SpringDefaultProfile::addBinding(_T("specteam 6"), _T("7"));
	SpringDefaultProfile::addBinding(_T("specteam 7"), _T("8"));
	SpringDefaultProfile::addBinding(_T("specteam 8"), _T("9"));
	SpringDefaultProfile::addBinding(_T("specteam 9"), _T("0"));
	//SpringDefaultProfile::addBinding( _T("speedup"), _T("Any++") ); makes no sense for me. its the same as Any+= effectively
	SpringDefaultProfile::addBinding(_T("speedup"), _T("Any+="));
	SpringDefaultProfile::addBinding(_T("speedup"), _T("Any+insert"));
	SpringDefaultProfile::addBinding(_T("speedup"), _T("Any+numpad+"));
	SpringDefaultProfile::addBinding(_T("stop"), _T("Shift+s"));
	SpringDefaultProfile::addBinding(_T("stop"), _T("s"));
	SpringDefaultProfile::addBinding(_T("togglecammode"), _T("Ctrl+backspace"));
	SpringDefaultProfile::addBinding(_T("togglecammode"), _T("Shift+backspace"));
	SpringDefaultProfile::addBinding(_T("togglelos"), _T("Any+l"));
	SpringDefaultProfile::addBinding(_T("toggleoverview"), _T("Any+tab"));
	SpringDefaultProfile::addBinding(_T("toggleradarandjammer"), _T("Any+;"));
	SpringDefaultProfile::addBinding(_T("track"), _T("Any+t"));
	SpringDefaultProfile::addBinding(_T("trackmode"), _T("Ctrl+t"));
	SpringDefaultProfile::addBinding(_T("unloadunits"), _T("Shift+u"));
	SpringDefaultProfile::addBinding(_T("unloadunits"), _T("u"));
	SpringDefaultProfile::addBinding(_T("viewfps"), _T("Ctrl+f1"));
	SpringDefaultProfile::addBinding(_T("viewrot"), _T("Ctrl+f4"));
	SpringDefaultProfile::addBinding(_T("viewta"), _T("Ctrl+f2"));
	SpringDefaultProfile::addBinding(_T("viewtw"), _T("Ctrl+f3"));
	SpringDefaultProfile::addBinding(_T("wait"), _T("w"));
	SpringDefaultProfile::addBinding(_T("wait queued"), _T("Shift+w"));

	SpringDefaultProfile::m_bindings.setMetaKey(_T("space"));
}

void SpringDefaultProfile::addBinding(const wxString& command, const wxString& key)
{
	SpringDefaultProfile::m_bindings.bind(command, key);
}

const key_binding& SpringDefaultProfile::getBindings()
{
	SpringDefaultProfile::initialize();

	return SpringDefaultProfile::m_bindings;
}
