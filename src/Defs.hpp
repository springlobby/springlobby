/**
    This file is part of Settings++,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (René Milk)
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
//#define ID_MENUITEM_MOUSE 60

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

struct Control {
    //const char lbl[64];
    const std::string lbl;
    //const char key[64];
    const std::string key;
    int id;
    const std::string def; //default value for confighandler, not control
};

inline wxString _S (const std::string str)
{
	return wxString(str.c_str(),*wxConvCurrent);
}

template<typename Type> void toString(std::string& s, Type t) {
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

//const wxWindowID _1 = wxWindowID(-1);
const wxPoint zeroPoint = wxPoint(0,0);

const Control RO_SLI[9] = {
	{"shadow-map size",              "ShadowMapSize",       ID_RO_SLI_0,	"2048"	},
	{"tree view-distance",           "TreeRadius",          ID_RO_SLI_1,	"3000"	},
	{"terrain detail",               "GroundDetail",        ID_RO_SLI_2,	"120"	},
	{"unit detail",                  "UnitLodDist",         ID_RO_SLI_3,	"600"	},
	{"grass detail",                 "GrassDetail",         ID_RO_SLI_4,	"10"	},
	{"ground decals",                "GroundDecals",        ID_RO_SLI_5,	"120"	},
	{"unit icon distance",           "UnitIconDist",        ID_RO_SLI_6,	"1000"	},
	{"max simultaneous particles",     "MaxParticles",        ID_RO_SLI_7,	"20000"	},
	{"max texture stages (SM3)", "SM3MaxTextureStages", ID_RO_SLI_8,	"20"	}
};

const Control VO_CBOX[3] = {
	{"run full-screen",                "Fullscreen",              ID_WINDOWP_VO_CBOX_0,	"1"},
	{"dual-screen mode",               "DualScreenMode",          ID_WINDOWP_VO_CBOX_1,	"0"},
	{"enable v-sync",                  "VSync",                   ID_WINDOWP_VO_CBOX_2,	"1"},
};

const Control VO_RBUT[2] = {
	{"16-bit Z-buffer", "DepthBufferBits", ID_WINDOWP_VO_RBUT_0,	"16"},
	{"24-bit Z-buffer", "DepthBufferBits", ID_WINDOWP_VO_RBUT_1,	"16"}
};


const Control VO_SLI[1] = {
	{"full-screen anti-aliasing samples", "FSAALevel", ID_VO_SLI_0,	"0"}
};

const Control VO_SLI_EXT[1] = {
	{"", "FSAA", -1, "0"}
};


const Control AO_SLI[3] = {
	{"maximum simultaneous sounds", "MaxSounds",            ID_AO_SLI_0,	"16" },
	{"global sound volume",   "SoundVolume",                ID_AO_SLI_1,	"100"},
	{"unit reply volume",     "UnitReplySoundVolume",       ID_AO_SLI_2,	"80" }
	
};

const Control QA_CBOX[10] = {
    {"shadows (slow)",                                  "Shadows",             ID_WINDOWP_QA_CBOX_0,	"0"},
	{"3D trees (needs Vertex_Program)",                 "3DTrees",             ID_WINDOWP_QA_CBOX_1,	"1"},
	{"high-resolution clouds (needs Fragment_Program)", "AdvSky",              ID_WINDOWP_QA_CBOX_2,	"0"},
	{"dynamic clouds (slow)",                           "DynamicSky",          ID_WINDOWP_QA_CBOX_3,	"0"},
	{"reflective units (needs Fragment_Program)",       "AdvUnitShading",      ID_WINDOWP_QA_CBOX_4,	"0"},
	{"never use shaders when rendering SM3 maps",       "SM3ForceFallbackTex", ID_WINDOWP_QA_CBOX_5,	"1"},
	{"enable LuaShaders support",                       "LuaShaders",          ID_WINDOWP_QA_CBOX_6,	"1"},
	{"high-resolution LOS textures",                    "HighResLos",          ID_WINDOWP_QA_CBOX_7,	"1"},
    	{"draw smooth points",                              "SmoothPoints",        ID_WINDOWP_QA_CBOX_8,	"0"},
	{"draw smooth lines",                               "SmoothLines",         ID_WINDOWP_QA_CBOX_9,	"0"},
};
	

const Control UI_CBOX[14] = {
	{"enable LuaUI widgets",                            "LuaUI",               ID_WINDOWP_UI_CBOX_1,	"1"},
	
	{"draw commands on mini-map",                       "MiniMapDrawCommands", ID_WINDOWP_UI_CBOX_2,	"1"},
	{"draw icons on mini-map",                          "MiniMapIcons",        ID_WINDOWP_UI_CBOX_3,	"1"},
	{"draw markers on mini-map",                        "MiniMapMarker",       ID_WINDOWP_UI_CBOX_4,	"1"},
	{"mini-map on left (single screen)", 				"MinimapOnLeft",       ID_WINDOWP_UI_CBOX_5,	"1"},
	{"mini-map on left (dual screen)",   				"DualScreenMiniMapOnLeft", ID_WINDOWP_UI_CBOX_6,"1"},
	{"simplified mini-map colors",                      "SimpleMiniMapColors", ID_WINDOWP_UI_CBOX_7,	"0"},

	{"team-colored nanospray",                          "TeamNanoSpray",       ID_WINDOWP_UI_CBOX_8,	"1"},
	{"colorized elevation map",                         "ColorElev",           ID_WINDOWP_UI_CBOX_9,	"1"},
	
	{"show in-game clock",                              "ShowClock",           ID_WINDOWP_UI_CBOX_10,	"1"},
	{"show in-game player information",                 "ShowPlayerInfo",      ID_WINDOWP_UI_CBOX_11,	"1"},
	{"show in-game framerate",                          "ShowFPS",             ID_WINDOWP_UI_CBOX_12,	"0"},

	{"fix rendering on alt-tab",                        "FixAltTab",           ID_WINDOWP_UI_CBOX_13,	"1"},
	{"disallow helper AI's",                            "NoHelperAIs",         ID_WINDOWP_UI_CBOX_14,	"0"}
};


const Control MO_SLI[5] = {
	{"overhead scroll speed (0 to disable)",    "OverheadScrollSpeed",    ID_MO_SLI_0,	"10"},
	{"rotatable overhead scroll speed (0 to disable)", "RotOverheadScrollSpeed", ID_MO_SLI_1,	"0" },
	{"TW scroll speed (0 to disable)",          "TWScrollSpeed",          ID_MO_SLI_2,	"0" },
	{"FPS scroll speed (0 to disable)",         "FPSScrollSpeed",         ID_MO_SLI_3,	"0" },
	{"FC scroll speed (0 to disable)",          "CamFreeScrollSpeed",     ID_MO_SLI_4,	"0" }
};

const Control MO_SLI_EXT[5] = {
	{"", "OverheadEnabled", -1,	"1"},
	{"", "RotOverheadEnabled", -1,	"0"},
	{"", "TWEnabled", -1,		"0"},
	{"", "FPSEnabled", -1,		"0"},
	{"", "CamFreeEnabled", -1,	"0"}
};


const Control DO_SLI[1] = {
	{"console verbose level (0=min,10=max)", "VerboseLevel", ID_DO_SLI_0,	"0"}
};

const Control DO_CBOX[2] = {
	{"catch AI exceptions",        "CatchAIExceptions", ID_WINDOWP_DO_CBOX_0,	"0"},
	{"send debug info to console", "StdoutDebug",       ID_WINDOWP_DO_CBOX_1,	"0"}
};

const Control WR_COMBOX[4] = {
	"basic",                   "ReflectiveWater", ID_WINDOWP_WR_COMBOX,	"1"
	/*{"reflective",              "ReflectiveWater", ID_WINDOWP_WR_COMBOX_1,	"1"},
	{"reflective + refractive", "ReflectiveWater", ID_WINDOWP_WR_COMBOX_2,	"1"},
	{"dynamic",                 "ReflectiveWater", ID_WINDOWP_WR_COMBOX_3,	"1"}*/
};

const wxString WR_COMBOX_CHOICES[4] = {
	wxT("basic"), wxT("reflective"), wxT("reflective + refractive"), wxT("dynamic")
};

const Control MO_CBOX[2] = {
	{"invert mouse y-axis",             "InvertMouse",      ID_WINDOWC_MO_CBOX_0,	"1"},
	{"mini-map 3-button mouse support", "MiniMapFullProxy", ID_WINDOWC_MO_CBOX_1,	"1"}
};

const Control MO_RBUT[5] = {
	{"OH (overhead)",            "CamMode", ID_WINDOWC_MO_RBUT_0,	"1"},
	{"ROH (rotatable overhead)", "CamMode", ID_WINDOWC_MO_RBUT_1,	"1"},
	{"TW (total war)",           "CamMode", ID_WINDOWC_MO_RBUT_2,	"1"},
	{"FP (first-person)",        "CamMode", ID_WINDOWC_MO_RBUT_3,	"1"},
	{"FC (free camera)",         "CamMode", ID_WINDOWC_MO_RBUT_4,	"1"}
};

const Control RC_TEXT[2] = {
	{"", "XResolution", -1,	"1024"},
	{"", "YResolution", -1,	"768" }
};



#define SLI_STYLE (wxSL_LABELS | wxSL_AUTOTICKS)
#define WX_DEF_V wxDefaultValidator
#define WX_DEF_P wxPoint(-1, -1)
#define WX_DEF_S wxSize(-1, -1)
#define WX_SLI_S wxSize(200, -1)


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


#endif
