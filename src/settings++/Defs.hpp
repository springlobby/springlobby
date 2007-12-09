/**
    This file is part of Settings++,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (Ren� Milk)
    visit http://spring.clan-sy.com/phpbb/viewtopic.php?t=12104
    for more info/help

    Settings++ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Settings++ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Settings++.  If not, see <http://www.gnu.org/licenses/>.
**/

// Autohost
// LODScale=1.000
// LODScaleReflection=1.000
// LODScaleRefraction=1.000
// LODScaleShadow=1.000
// MapArchives=1									// multiple maps per archive?
// MetalMapPalette=0
//
// FIXME: VSync Off=-1?



#ifndef DEFS_HPP
#define DEFS_HPP

#define PWIN_SIZE_X  895
#define PWIN_SIZE_Y  830
#define CWIN_SIZE_X  580
#define CWIN_SIZE_Y  360

// ParentWin menu-item IDs
#define ID_MENUITEM_SAVE  50
#define ID_MENUITEM_RESET  51
#define ID_MENUITEM_QUIT  52
#define ID_MENUITEM_DISABLE_WARN 53
#define ID_MENUITE_MODE 60
#define ID_MENUITEM_EXPERT 62
#define ID_MENUITEM_SIMPLE 61

// ParentWin TextInput IDs
#define ID_RES_CHOICES_LBOX_X 1111
#define ID_RES_CHOICES_LBOX_Y 1112


// ParentWin Rendering Options slider IDs
#define ID_RO_SLI_0 100
#define ID_RO_SLI_1 101
#define ID_RO_SLI_2 102
#define ID_RO_SLI_3 103
#define ID_RO_SLI_4 104
#define ID_RO_SLI_5 105
#define ID_RO_SLI_6 106
#define ID_RO_SLI_7 107
#define ID_RO_SLI_8 108

// ParentWin Video Options checkbox IDs
#define ID_WINDOWP_VO_CBOX_0 250
#define ID_WINDOWP_VO_CBOX_1 251
#define ID_WINDOWP_VO_CBOX_2 252

// ParentWin Video Options radiobutton IDs
#define ID_WINDOWP_VO_RBUT_0 280
#define ID_WINDOWP_VO_RBUT_1 281

// ParentWin Video Options slider IDs
#define ID_VO_SLI_0 200

// tab_simple controls
#define ID_SIMPLE_QUAL_CBX		390
#define ID_SIMPLE_DETAIL_CBX	391
#define ID_SIMPLE_MODE_CBX		392


// UI Options checkbox IDs
#define ID_WINDOWP_UI_CBOX_1  360
#define ID_WINDOWP_UI_CBOX_2  361
#define ID_WINDOWP_UI_CBOX_3  362
#define ID_WINDOWP_UI_CBOX_4  363
#define ID_WINDOWP_UI_CBOX_5  364
#define ID_WINDOWP_UI_CBOX_6  365
#define ID_WINDOWP_UI_CBOX_7  366
#define ID_WINDOWP_UI_CBOX_8  367
#define ID_WINDOWP_UI_CBOX_9  368
#define ID_WINDOWP_UI_CBOX_10 369
#define ID_WINDOWP_UI_CBOX_11 370
#define ID_WINDOWP_UI_CBOX_12 371
#define ID_WINDOWP_UI_CBOX_13 372
#define ID_WINDOWP_UI_CBOX_14 373

//QUALITY OPTIONS checkbox IDs
#define ID_WINDOWP_QA_CBOX_0  300
#define ID_WINDOWP_QA_CBOX_1  301
#define ID_WINDOWP_QA_CBOX_2  302
#define ID_WINDOWP_QA_CBOX_3  303
#define ID_WINDOWP_QA_CBOX_4  304
#define ID_WINDOWP_QA_CBOX_5  305
#define ID_WINDOWP_QA_CBOX_6  306
#define ID_WINDOWP_QA_CBOX_7  307
#define ID_WINDOWP_QA_CBOX_8  308
#define ID_WINDOWP_QA_CBOX_9  309

#define ID_WINDOWP_WR_COMBOX  320

// ParentWin Audio Options slider IDs
#define ID_AO_SLI_0 400
#define ID_AO_SLI_1 401
#define ID_AO_SLI_2 402

// ParentWin Debug Options checkbox IDs
#define ID_WINDOWP_DO_CBOX_0 540
#define ID_WINDOWP_DO_CBOX_1 541

// ParentWin Debug Options slider IDs
#define ID_DO_SLI_0 500

// ParentWin Water Rendering radiobutton IDs
#define ID_WINDOWP_WR_RBUT_0 550
#define ID_WINDOWP_WR_RBUT_1 551
#define ID_WINDOWP_WR_RBUT_2 552
#define ID_WINDOWP_WR_RBUT_3 553

// ChildWin Mouse Options checkbox IDs
#define ID_WINDOWC_MO_CBOX_0 650
#define ID_WINDOWC_MO_CBOX_1 651

// ChildWin Mouse Options radiobutton IDs
#define ID_WINDOWC_MO_RBUT_0 680
#define ID_WINDOWC_MO_RBUT_1 681
#define ID_WINDOWC_MO_RBUT_2 682
#define ID_WINDOWC_MO_RBUT_3 683
#define ID_WINDOWC_MO_RBUT_4 684

// ChildWin Mouse Options slider IDs
#define ID_MO_SLI_0 660
#define ID_MO_SLI_1 661
#define ID_MO_SLI_2 662
#define ID_MO_SLI_3 663
#define ID_MO_SLI_4 664



#include <wx/wx.h>
#include <string>
#include <iostream>
//#include <fstream>
#include <sstream>

struct Control {
    //const char lbl[64];
    const std::string lbl;
    //const char key[64];
    const wxString key;
    int id;
    const std::string def; //default value for confighandler, not control
    const wxString tTip [1];
};

#define _S(v) wxString(v.c_str(),wxConvUTF8)

template<typename Type> void toString(std::string& s, Type t) {
	//using namespace std;
			std::stringstream ss;
			ss << t;
			ss >> s;
}

template<typename Type> Type fromString(const std::string& s) {
        std::stringstream ss;
        ss << s;
        Type r;
        ss >> r;
        return r;
}

//TODO guard properly
#ifndef JUFGDJ
	#define STD_STRING(v) std::string((const char*)v.mb_str(wxConvUTF8))
	#ifdef WIN32
	  #define CHOOSE_DLL _("Library (*.dll)|*.dll|Any File (*.*)|*.*")
	  #define USYNC_CALL_CONV __stdcall
	  #define SPRING_BIN _T("spring.exe")
	  #define UNITSYNC_BIN _T("unitsync.dll")
	  #define DOS_TXT true
	#else
	  #define CHOOSE_DLL _("Library (*.so)|*.so|Any File (*.*)|*.*")
	  #define USYNC_CALL_CONV
	  #define SPRING_BIN _T("spring")
	  #define UNITSYNC_BIN _T("unitsync.so")
	  #define DOS_TXT false
	#endif
#endif

//const wxWindowID _1 = wxWindowID(-1);
const wxPoint zeroPoint = wxPoint(0,0);

//TODO find reasonable default values
//TODO is max_texture stages obsolte?
const Control RO_SLI[9] = {
	{"shadow-map size",              _T("ShadowMapSize"),       ID_RO_SLI_0,	"2048", {wxT("placeholder")}	},
	{"tree view-distance",           _T("TreeRadius"),          ID_RO_SLI_1,	"3000", {wxT("placeholder")}	},
	{"terrain detail",               _T("GroundDetail"),        ID_RO_SLI_2,	"120", {wxT("placeholder")}	},
	{"unit detail",                  _T("UnitLodDist"),         ID_RO_SLI_3,	"600", {wxT("placeholder")}	},
	{"grass detail",                 _T("GrassDetail"),         ID_RO_SLI_4,	"10", {wxT("placeholder")}	},
	{"ground decals",                _T("GroundDecals"),        ID_RO_SLI_5,	"120", {wxT("placeholder")}	},
	{"unit icon distance",           _T("UnitIconDist"),        ID_RO_SLI_6,	"1000", {wxT("placeholder")}	},
	{"max simultaneous particles",     _T("MaxParticles"),        ID_RO_SLI_7,	"20000"	, {wxT("placeholder")}},
	{"max texture stages (SM3)", _T("SM3MaxTextureStages"), ID_RO_SLI_8,	"20"	, {wxT("placeholder")}}
};

const Control VO_CBOX[3] = {
	{"run full-screen",                _T("Fullscreen"),              ID_WINDOWP_VO_CBOX_0,	"1", {wxT("placeholder")}},
	{"dual-screen mode",               _T("DualScreenMode"),          ID_WINDOWP_VO_CBOX_1,	"0", {wxT("placeholder")}},
	{"enable v-sync",                  _T("VSync"),                   ID_WINDOWP_VO_CBOX_2,	"1", {wxT("placeholder")}},
};

const Control VO_RBUT[2] = {
	{"16-bit Z-buffer", _T("DepthBufferBits"), ID_WINDOWP_VO_RBUT_0,	"16", {wxT("placeholder")}},
	{"24-bit Z-buffer", _T("DepthBufferBits"), ID_WINDOWP_VO_RBUT_1,	"16", {wxT("placeholder")}}
};


const Control VO_SLI[1] = {
	{"full-screen anti-aliasing samples", _T("FSAALevel"), ID_VO_SLI_0,	"0", {wxT("placeholder")}}
};

const Control VO_SLI_EXT[1] = {
	{"", _T("FSAA"), -1, "0"}
};


const Control AO_SLI[3] = {
	{"maximum simultaneous sounds", _T("MaxSounds"),            ID_AO_SLI_0,	"16" , {wxT("placeholder")}},
	{"global sound volume",   _T("SoundVolume"),                ID_AO_SLI_1,	"100", {wxT("placeholder")}},
	{"unit reply volume",     _T("UnitReplySoundVolume"),       ID_AO_SLI_2,	"80" , {wxT("placeholder")}}
	
};

const Control QA_CBOX[10] = {
    {"shadows (slow)",                                  _T("Shadows"),             ID_WINDOWP_QA_CBOX_0,	"0", {wxT("placeholder")}},
	{"3D trees (needs Vertex_Program)",                 _T("3DTrees"),             ID_WINDOWP_QA_CBOX_1,	"1", {wxT("placeholder")}},
	{"high-resolution clouds (needs Fragment_Program)", _T("AdvSky"),              ID_WINDOWP_QA_CBOX_2,	"0", {wxT("placeholder")}},
	{"dynamic clouds (slow)",                           _T("DynamicSky"),          ID_WINDOWP_QA_CBOX_3,	"0", {wxT("placeholder")}},
	{"reflective units (needs Fragment_Program)",       _T("AdvUnitShading"),      ID_WINDOWP_QA_CBOX_4,	"0", {wxT("placeholder")}},
	{"never use shaders when rendering SM3 maps",       _T("SM3ForceFallbackTex"), ID_WINDOWP_QA_CBOX_5,	"1", {wxT("placeholder")}},
	{"enable LuaShaders support",                       _T("LuaShaders"),          ID_WINDOWP_QA_CBOX_6,	"1", {wxT("placeholder")}},
	{"high-resolution LOS textures",                    _T("HighResLos"),          ID_WINDOWP_QA_CBOX_7,	"1", {wxT("placeholder")}},
    	{"draw smooth points",                              _T("SmoothPoints"),        ID_WINDOWP_QA_CBOX_8,	"0", {wxT("placeholder")}},
	{"draw smooth lines",                               _T("SmoothLines"),         ID_WINDOWP_QA_CBOX_9,	"0", {wxT("placeholder")}},
};
	

const Control UI_CBOX[14] = {
	{"enable LuaUI widgets",                            _T("LuaUI"),               ID_WINDOWP_UI_CBOX_1,	"1", {wxT("placeholder")}},
	
	{"draw commands on mini-map",                       _T("MiniMapDrawCommands"), ID_WINDOWP_UI_CBOX_2,	"1", {wxT("placeholder")}},
	{"draw icons on mini-map",                          _T("MiniMapIcons"),        ID_WINDOWP_UI_CBOX_3,	"1", {wxT("placeholder")}},
	{"draw markers on mini-map",                        _T("MiniMapMarker"),       ID_WINDOWP_UI_CBOX_4,	"1", {wxT("placeholder")}},
	{"mini-map on left (single screen)", 				_T("MinimapOnLeft"),       ID_WINDOWP_UI_CBOX_5,	"1", {wxT("placeholder")}},
	{"mini-map on left (dual screen)",   				_T("DualScreenMiniMapOnLeft"), ID_WINDOWP_UI_CBOX_6,"1", {wxT("placeholder")}},
	{"simplified mini-map colors",                      _T("SimpleMiniMapColors"), ID_WINDOWP_UI_CBOX_7,	"0", {wxT("placeholder")}},

	{"team-colored nanospray",                          _T("TeamNanoSpray"),       ID_WINDOWP_UI_CBOX_8,	"1", {wxT("placeholder")}},
	{"colorized elevation map",                         _T("ColorElev"),           ID_WINDOWP_UI_CBOX_9,	"1", {wxT("placeholder")}},
	
	{"show in-game clock",                              _T("ShowClock"),           ID_WINDOWP_UI_CBOX_10,	"1", {wxT("placeholder")}},
	{"show in-game player information",                 _T("ShowPlayerInfo"),      ID_WINDOWP_UI_CBOX_11,	"1", {wxT("placeholder")}},
	{"show in-game framerate",                          _T("ShowFPS"),             ID_WINDOWP_UI_CBOX_12,	"0", {wxT("placeholder")}},
//TODO is there even a reason that it should be disabled?
	{"fix rendering on alt-tab",                        _T("FixAltTab"),           ID_WINDOWP_UI_CBOX_13,	"1", {wxT("placeholder")}},
	{"disallow helper AI's",                            _T("NoHelperAIs"),         ID_WINDOWP_UI_CBOX_14,	"0", {wxT("placeholder")}}
};


const Control MO_SLI[5] = {
	{"Overhead Camera scroll speed",    _T("OverheadScrollSpeed"),    ID_MO_SLI_0,	"10", {wxT("placeholder")}},
	{"Rotatable overhead Camera scroll speed", _T("RotOverheadScrollSpeed"), ID_MO_SLI_1,	"0" , {wxT("placeholder")}},
	{"Total War Camera scroll speed",          _T("TWScrollSpeed"),          ID_MO_SLI_2,	"0" , {wxT("placeholder")}},
	{"First Person Camera scroll speed",         _T("FPSScrollSpeed"),         ID_MO_SLI_3,	"0" , {wxT("placeholder")}},
	{"Free Camera scroll speed",          _T("CamFreeScrollSpeed"),     ID_MO_SLI_4,	"0" , {wxT("placeholder")}}
};

const Control MO_SLI_EXT[5] = {
	{"", _T("OverheadEnabled"), -1,	"1", {wxT("placeholder")}},
	{"", _T("RotOverheadEnabled"), -1,	"0", {wxT("placeholder")}},
	{"", _T("TWEnabled"), -1,		"0", {wxT("placeholder")}},
	{"", _T("FPSEnabled"), -1,		"0", {wxT("placeholder")}},
	{"", _T("CamFreeEnabled"), -1,	"0", {wxT("placeholder")}}
};


const Control DO_SLI[1] = {
	{"console verbose level (0=min,10=max)", _T("VerboseLevel"), ID_DO_SLI_0,	"0", {wxT("placeholder")}}
};

const Control DO_CBOX[2] = {
	{"catch AI exceptions",        _T("CatchAIExceptions"), ID_WINDOWP_DO_CBOX_0,	"0", {wxT("placeholder")}},
	{"send debug info to console", _T("StdoutDebug"),       ID_WINDOWP_DO_CBOX_1,	"0", {wxT("placeholder")}}
};


const Control WR_COMBOX[1] = {
		{"basic",                  _T("ReflectiveWater"), ID_WINDOWP_WR_COMBOX,	"1", {wxT("placeholder")}}
	/*{"reflective",              "ReflectiveWater", ID_WINDOWP_WR_COMBOX_1,	"1"},
	{"reflective + refractive", "ReflectiveWater", ID_WINDOWP_WR_COMBOX_2,	"1"},
	{"dynamic",                 "ReflectiveWater", ID_WINDOWP_WR_COMBOX_3,	"1"}*/
};

const wxString WR_COMBOX_CHOICES[4] = {
	wxT("basic"), wxT("reflective"), wxT("reflective + refractive"), wxT("dynamic")
};

const Control MO_CBOX[2] = {
	{"invert mouse y-axis",             _T("InvertMouse"),      ID_WINDOWC_MO_CBOX_0,	"1", {wxT("placeholder")}},
	{"mini-map 3-button mouse support", _T("MiniMapFullProxy"), ID_WINDOWC_MO_CBOX_1,	"1", {wxT("placeholder")}}
};

const Control MO_RBUT[5] = {
	{"OH (overhead)",            _T("CamMode"), ID_WINDOWC_MO_RBUT_0,	"1", {wxT("placeholder")}},
	{"ROH (rotatable overhead)", _T("CamMode"), ID_WINDOWC_MO_RBUT_1,	"1", {wxT("placeholder")}},
	{"TW (total war)",           _T("CamMode"), ID_WINDOWC_MO_RBUT_2,	"1", {wxT("placeholder")}},
	{"FP (first-person)",        _T("CamMode"), ID_WINDOWC_MO_RBUT_3,	"1", {wxT("placeholder")}},
	{"FC (free camera)",         _T("CamMode"), ID_WINDOWC_MO_RBUT_4,	"1", {wxT("placeholder")}}
};

const Control RC_TEXT[2] = {
	{"", _T("XResolution"), -1,	"1024", {wxT("placeholder")}},
	{"", _T("YResolution"), -1,	"768" , {wxT("placeholder")}}
};



#define SLI_STYLE (wxSL_LABELS | wxSL_AUTOTICKS)
#define WX_DEF_V wxDefaultValidator
#define WX_DEF_P wxPoint(-1, -1)
#define WX_DEF_S wxSize(-1, -1)
#define WX_SLI_S wxSize(200, -1)

/** not used
#define NUM_DEFAULTS 88
const char DEFAULTS[NUM_DEFAULTS][64] = {
	"3DTrees=1",
	"AdvSky=0",
	"AdvUnitShading=0",
	"FSAA=0",
	"FSAALevel=0",
	"ReflectiveWater=1",
	"MaxParticles=20000",
	"DynamicSky=0",
	"DepthBufferBits=16",
	"StencilBufferBits=1",
	"GrassDetail=10",
	"GroundDecals=100",
	"GroundDetail=120",
	"Shadows=0",
	"ShadowMapSize=2048",
	"SM3MaxTextureStages=20",
	"SM3ForceFallbackTex=1",
	"TreeRadius=3000",
	"UnitIconDist=1000",
	"UnitLodDist=600",
	"VSync=1",
	"XResolution=1024",
	"YResolution=768",

	"BuildIconsFirst=0",
	"ColorElev=1",
	"DualScreenMode=0",
	"FixAltTab=1",
	"Fullscreen=0",
	"FixAltTab=1",
	"HighResLos=1",
	"NoHelperAIs=0",
	"TeamNanoSpray=1",
	"SmoothLines=0",
	"SmoothPoints=0",
	"ShowClock=1",
	"ShowPlayerInfo=1",
	"ShowFPS=0",

	"StdoutDebug=0",
	"CatchAIExceptions=0",
	"VerboseLevel=0",

	"CamMode=1",
	"CamTimeExponent=4.0",
	"CamTimeFactor=1.0",
	"DoubleClickTime=200",

	"FontCharFirst=32",
	"FontCharLast=223",
	"FontFile=Luxi.ttf",

	"InfoConsoleGeometry=0.25 0.95 0.40 0.20",
	"TooltipGeometry=0.00 0.00 0.40 0.1",
	"TooltipOutlineFont=0",
	"InfoMessageTime=400",
	"InvertMouse=1",
	"InvertQueueKey=0",
	"WindowedEdgeMove=1",

	"LuaGaia=1",
	"LuaRules=1",
	"LuaShaders=1",
	"LuaUI=1",

	"MapArchives=1",
	"MetalMapPalette=0",

	"MiniMapButtonSize=16",
	"MiniMapCursorScale=-0.5",
	"MiniMapDrawCommands=1",
	"MiniMapFullProxy=1",
	"MiniMapGeometry=2 2 200 200",
	"MiniMapIcons=1",
	"MiniMapMarker=1",
	"MiniMapUnitExp=0.25",
	"MiniMapUnitSize=2.5",
	"MinimapOnLeft=1",
	"DualScreenMiniMapOnLeft=1",
	"SimpleMiniMapColors=0",

	"OverheadEnabled=1",
	"OverheadScrollSpeed=10",
	"RotOverheadEnabled=0",
	"RotOverheadScrollSpeed=0",
	"RotOverheadMouseScale=0.002",
	"TWEnabled=0",
	"TWScrollSpeed=0",
	"FPSEnabled=0",
	"FPSScrollSpeed=0",
	"FPSMouseScale=0.002",
	"CamFreeEnabled=0",
	"CamFreeScrollSpeed=0",
	"ScrollWheelSpeed=25",

	"MaxSounds=16",
	"SoundVolume=100",
	"UnitReplySoundVolume=80"
};
**/

#endif
