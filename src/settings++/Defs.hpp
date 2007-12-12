/**
    This file is part of springsettings,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (Ren� Milk)
    visit http://spring.clan-sy.com/phpbb/viewtopic.php?t=12104
    for more info/help

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with springsettings.  If not, see <http://www.gnu.org/licenses/>.
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
#define ID_SIMPLE_GOEXPERT_BUT 	393


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

// panel_paths ids
#define ID_PATH_SPRINGDIR_BTN 	700
#define ID_PATH_USYNC_BTN		701
#define ID_PATH_OK_BTN			702

#define SLI_STYLE (wxSL_LABELS )
#define WX_DEF_V wxDefaultValidator
#define WX_DEF_P wxPoint(-1, -1)
#define WX_DEF_S wxSize(-1, -1)
#define WX_SLI_S wxSize(200, -1)

#define configHandler (susynclib())
#define TAB_SIZE wxSize(700,500)
#define UPDATE_VIDEO_MODE 	2002
#define UPDATE_QA_BOXES	  	2001
#define UPDATE_ALL			2000
#define UPDATE_EXPERTMODE_WARNING_ON_SIMPLETAB 2003

#include <wx/string.h>
#include <string>

struct Control {
    const wxString lbl;
    const wxString key;
    int id;
    const std::string def; //default value for confighandler, not control
    const wxString tTip [1];
};

//TODO is max_texture stages obsolte?
const Control RO_SLI[9] = {
	{_T("Shadow-map size"),              wxT("ShadowMapSize"),       ID_RO_SLI_0,	"2048", {_T("higher value = better looking shadows\n"
																								"possible values: 1024, 2048, 4096, 8192")}	},
	{_T("Tree view-distance"),           wxT("TreeRadius"),          ID_RO_SLI_1,	"1000", {_T("placeholder")}	},
	{_T("Terrain detail"),               wxT("GroundDetail"),        ID_RO_SLI_2,	"80", {_T("higher value = more terrain details")}	},
	{_T("Unit detail"),                  wxT("UnitLodDist"),         ID_RO_SLI_3,	"300", {_T("higher value = more detailed units")}	},
	{_T("Grass detail"),                 wxT("GrassDetail"),         ID_RO_SLI_4,	"3", {_T("higher value = more detailed graas")}	},
	{_T("Ground decals"),                wxT("GroundDecals"),        ID_RO_SLI_5,	"40", {_T("higher value = more ground decals")}	},
	{_T("Unit icon distance"),           wxT("UnitIconDist"),        ID_RO_SLI_6,	"350", {_T("determines at which range units are still fully rendered\n"
																								"higher value = greater range = more units rendered at the same time")}	},
	{_T("Max simultaneous particles"),     wxT("MaxParticles"),        ID_RO_SLI_7,	"5000"	, {_T("limits how many particles are displayed at the same time")}},
	{_T("Max texture stages (SM3)"), wxT("SM3MaxTextureStages"), ID_RO_SLI_8,	"20"	, {_T("placeholder")}}
};

const Control VO_CBOX[3] = {
	{_T("Run full-screen"),                wxT("Fullscreen"),              ID_WINDOWP_VO_CBOX_0,	"1", {_T("run fullscreen or in a window?")}},
	{_T("Dual-screen mode"),               wxT("DualScreenMode"),          ID_WINDOWP_VO_CBOX_1,	"0", {_T("if you have two monitors you can use both")}},
	{_T("Enable v-sync"),                  wxT("VSync"),                   ID_WINDOWP_VO_CBOX_2,	"1", {_T("V-Sync on/off")}},
};

const Control VO_RBUT[2] = {
	{_T("16-bit Z-buffer"), wxT("DepthBufferBits"), ID_WINDOWP_VO_RBUT_0,	"16", {_T("placeholder")}},
	{_T("24-bit Z-buffer"), wxT("DepthBufferBits"), ID_WINDOWP_VO_RBUT_1,	"16", {_T("placeholder")}}
};


const Control VO_SLI[1] = {
	{_T("Full-screen anti-aliasing samples"), wxT("FSAALevel"), ID_VO_SLI_0,	"0", {_T("how much anti-aliasing should be applied")}}
};

const Control VO_SLI_EXT[1] = {
	{_T(""), wxT("FSAA"), -1, "0"}
};


const Control AO_SLI[3] = {
	{_T("Maximum simultaneous sounds"), wxT("MaxSounds"),            ID_AO_SLI_0,	"16" , {_T("maximum different sounds played at the same time")}},
	{_T("Global sound volume"),   wxT("SoundVolume"),                ID_AO_SLI_1,	"100", {_T("overall sound volume")}},
	{_T("Unit reply volume"),     wxT("UnitReplySoundVolume"),       ID_AO_SLI_2,	"80" , {_T("reply volume relative to global volume")}}
	
};

const Control QA_CBOX[10] = {
    {_T("Shadows (slow)"),                                  wxT("Shadows"),             ID_WINDOWP_QA_CBOX_0,	"0", {_T("enable shadows?")}},
	{_T("3D trees"),                 wxT("3DTrees"),             ID_WINDOWP_QA_CBOX_1,	"1", {_T("want better looking trees?\n"
														"needs Geforce 2/Radeon 8500/Intel 830 or later class graphic card")}},
	{_T("High-resolution clouds"), wxT("AdvSky"),              ID_WINDOWP_QA_CBOX_2,	"0", {_T("want better looking sky?\n"
														"needs Geforce 5/Radeon 9500/Intel 915 or later class graphic card")}},
	{_T("Dynamic clouds (slow)"),                           wxT("DynamicSky"),          ID_WINDOWP_QA_CBOX_3,	"0", {_T("want moving clouds in the sky?")}},
	{_T("Reflective units"),       wxT("AdvUnitShading"),      ID_WINDOWP_QA_CBOX_4,	"0", {_T("shiny units?\n"
														"needs Geforce 5/Radeon 9500/Intel 915 or later class graphic card")}},
	{_T("Never use shaders when rendering SM3 maps"),       wxT("SM3ForceFallbackTex"), ID_WINDOWP_QA_CBOX_5,	"1", {_T("problems with sm3 maps? enable this")}},
	{_T("Enable LuaShaders support"),                       wxT("LuaShaders"),          ID_WINDOWP_QA_CBOX_6,	"1", {_T("makes for some cool effects")}},
	{_T("High-resolution LOS textures"),                    wxT("HighResLos"),          ID_WINDOWP_QA_CBOX_7,	"1", {_T("placeholder")}},
    {_T("Draw smooth points"),                              wxT("SmoothPoints"),        ID_WINDOWP_QA_CBOX_8,	"0", {_T("should points be anti-aliased")}},
	{_T("Draw smooth lines"),                               wxT("SmoothLines"),         ID_WINDOWP_QA_CBOX_9,	"0", {_T("should lines be anti-aliased")}},
};
	

const Control UI_CBOX[14] = {
	{_T("Enable LuaUI widgets"),                            wxT("LuaUI"),               ID_WINDOWP_UI_CBOX_1,	"1", {_T("mark to be able to use")}},
	
	{_T("Draw commands on mini-map"),                       wxT("MiniMapDrawCommands"), ID_WINDOWP_UI_CBOX_2,	"1", {_T("placeholder")}},
	{_T("Draw icons on mini-map"),                          wxT("MiniMapIcons"),        ID_WINDOWP_UI_CBOX_3,	"1", {_T("placeholder")}},
	{_T("Draw markers on mini-map"),                        wxT("MiniMapMarker"),       ID_WINDOWP_UI_CBOX_4,	"1", {_T("placeholder")}},
	{_T("Mini-map on left (single screen)"), 				wxT("MinimapOnLeft"),       ID_WINDOWP_UI_CBOX_5,	"1", {_T("placeholder")}},
	{_T("Mini-map on left (dual screen)"),   				wxT("DualScreenMiniMapOnLeft"), ID_WINDOWP_UI_CBOX_6,"1", {_T("placeholder")}},
	{_T("Simplified mini-map colors"),                      wxT("SimpleMiniMapColors"), ID_WINDOWP_UI_CBOX_7,	"0", {_T("placeholder")}},

	{_T("Team-colored nanospray"),                          wxT("TeamNanoSpray"),       ID_WINDOWP_UI_CBOX_8,	"1", {_T("placeholder")}},
	{_T("Colorized elevation map"),                         wxT("ColorElev"),           ID_WINDOWP_UI_CBOX_9,	"1", {_T("placeholder")}},
	
	{_T("Show in-game clock"),                              wxT("ShowClock"),           ID_WINDOWP_UI_CBOX_10,	"1", {_T("placeholder")}},
	{_T("Show in-game player information"),                 wxT("ShowPlayerInfo"),      ID_WINDOWP_UI_CBOX_11,	"1", {_T("placeholder")}},
	{_T("Show in-game framerate"),                          wxT("ShowFPS"),             ID_WINDOWP_UI_CBOX_12,	"0", {_T("placeholder")}},
//TODO is there even a reason that it should be disabled?
	{_T("Fix rendering on alt-tab"),                        wxT("FixAltTab"),           ID_WINDOWP_UI_CBOX_13,	"1", {_T("placeholder")}},
	{_T("Disallow helper AI's"),                            wxT("NoHelperAIs"),         ID_WINDOWP_UI_CBOX_14,	"0", {_T("placeholder")}}
};


const Control MO_SLI[5] = {
	{_T("Overhead camera"),    wxT("OverheadScrollSpeed"),    ID_MO_SLI_0,	"10", {_T("placeholder")}},
	{_T("Rotatable overhead camera"), wxT("RotOverheadScrollSpeed"), ID_MO_SLI_1,	"0" , {_T("placeholder")}},
	{_T("Total war camera"),          wxT("TWScrollSpeed"),          ID_MO_SLI_2,	"0" , {_T("placeholder")}},
	{_T("First person camera"),         wxT("FPSScrollSpeed"),         ID_MO_SLI_3,	"0" , {_T("placeholder")}},
	{_T("Free camera"),          wxT("CamFreeScrollSpeed"),     ID_MO_SLI_4,	"0" , {_T("placeholder")}}
};

const Control MO_SLI_EXT[5] = {
	{_T(""), wxT("OverheadEnabled"), -1,	"1", {_T("placeholder")}},
	{_T(""), wxT("RotOverheadEnabled"), -1,	"0", {_T("placeholder")}},
	{_T(""), wxT("TWEnabled"), -1,		"0", {_T("placeholder")}},
	{_T(""), wxT("FPSEnabled"), -1,		"0", {_T("placeholder")}},
	{_T(""), wxT("CamFreeEnabled"), -1,	"0", {_T("placeholder")}}
};


const Control DO_SLI[1] = {
	{_T("Console verbose level (0=min,10=max)"), wxT("VerboseLevel"), ID_DO_SLI_0,	"0", {_T("How much information should be outputted?")}}
};

const Control DO_CBOX[2] = {
	{_T("Catch AI exceptions"),        wxT("CatchAIExceptions"), ID_WINDOWP_DO_CBOX_0,	"0", {_T("placeholder")}},
	{_T("Send debug info to console"), wxT("StdoutDebug"),       ID_WINDOWP_DO_CBOX_1,	"0", {_T("placeholder")}}
};


const Control WR_COMBOX[1] = {
		{_T("Basic"),                  wxT("ReflectiveWater"), ID_WINDOWP_WR_COMBOX,	"1", {_T("placeholder")}}
	/*{_T("reflective",              "ReflectiveWater", ID_WINDOWP_WR_COMBOX_1,	"1"},
	{_T("reflective + refractive", "ReflectiveWater", ID_WINDOWP_WR_COMBOX_2,	"1"},
	{_T("dynamic",                 "ReflectiveWater", ID_WINDOWP_WR_COMBOX_3,	"1"}*/
};

const wxString WR_COMBOX_CHOICES[4] = {
	_T("Basic"), _T("Reflective"), _T("Reflective + refractive"), _T("Dynamic")
};

const Control MO_CBOX[2] = {
	{_T("Invert mouse y-axis"),             wxT("InvertMouse"),      ID_WINDOWC_MO_CBOX_0,	"1", {_T("placeholder")}},
	{_T("Mini-map 3-button mouse support"), wxT("MiniMapFullProxy"), ID_WINDOWC_MO_CBOX_1,	"1", {_T("placeholder")}}
};

const Control MO_RBUT[5] = {
	{_T("Overhead"),            wxT("CamMode"), ID_WINDOWC_MO_RBUT_0,	"1", {_T("placeholder")}},
	{_T("Rotatable overhead"), wxT("CamMode"), ID_WINDOWC_MO_RBUT_1,	"1", {_T("placeholder")}},
	{_T("Total war)"),           wxT("CamMode"), ID_WINDOWC_MO_RBUT_2,	"1", {_T("placeholder")}},
	{_T("First person"),        wxT("CamMode"), ID_WINDOWC_MO_RBUT_3,	"1", {_T("placeholder")}},
	{_T("Free camera"),         wxT("CamMode"), ID_WINDOWC_MO_RBUT_4,	"1", {_T("placeholder")}}
};

const Control RC_TEXT[2] = {
	{_T(""), wxT("XResolution"), -1,	"1024", {_T("placeholder")}},
	{_T(""), wxT("YResolution"), -1,	"768" , {_T("placeholder")}}
};

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
