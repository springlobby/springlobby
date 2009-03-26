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
#define ID_MENUITEM_SAVE  			50
#define ID_MENUITEM_RESET 			51
#define ID_MENUITEM_QUIT  			52
#define ID_MENUITEM_DISABLE_WARN 	53
#define ID_MENUITEM_MODE 			60
#define ID_MENUITEM_EXPERT 			62
#define ID_MENUITEM_SIMPLE 			61

#define ID_MENUITEM_HELP			70
#define ID_MENUITEM_ABOUT 			71
#define ID_MENUITEM_CONTACT			72
#define ID_MENUITEM_CREDITS 		73
#define ID_MENUITEM_BUGREPORT 		74

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
//#define ID_RO_SLI_8 108

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
#define ID_WINDOWP_UI_CBOX_15 374
#define ID_WINDOWP_UI_CBOX_16 375
#define ID_WINDOWP_UI_CBOX_17 376
#define ID_WINDOWP_UI_CBOX_18 377

//Zoom opt
#define ID_WINDOWP_UI_MW_SPD  385

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
#define ID_WINDOWP_QA_CBOX_10 310
#define ID_WINDOWP_QA_CBOX_11 311

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
#define ID_PATH_USYNC_BTN		701
#define ID_PATH_OK_BTN			702

//water 4 specific options
#define ID_W4_BumpWaterBlurReflection       720
#define ID_W4_BumpWaterUseDepthTexture      721
#define ID_W4_BumpWaterShoreWaves           722
#define ID_W4_BumpWaterReflection           723
#define ID_W4_BumpWaterTexSizeReflection    724
#define ID_W4_BumpWaterRefraction           725
#define ID_W4_BumpWaterAnisotropy           726

#define SLI_STYLE (wxSL_LABELS )
#define WX_DEF_V wxDefaultValidator
#define WX_DEF_P wxPoint(-1, -1)
#define WX_DEF_S wxSize(-1, -1)
#define WX_SLI_S wxSize(200, -1)

#define configHandler (susynclib())
#define TAB_SIZE wxSize(700,500)
#define UPDATE_W4_CONTROLS 	2003
#define UPDATE_VIDEO_MODE 	2002
#define UPDATE_QA_BOXES	  	2001
#define UPDATE_ALL			2000
#define UPDATE_EXPERTMODE_WARNING_ON_SIMPLETAB 2003

//#include <wx/string.h>
#include <wx/intl.h>
//#include <string>
#include <map>

struct Control {
    const wxString lbl;
    const wxString key;
    int id;
    const wxString def; //default value for confighandler, not control
    const wxString tTip [1];
};

const int Control_size = sizeof(Control);

typedef std::map<const wxString,const int> category_sizes_map ;
typedef category_sizes_map::value_type category_sizes_map_type;

const Control UI_ZOOM[] = {
	{_("Scrollwheel speed"),           _T("ScrollWheelSpeed"),      ID_WINDOWP_UI_MW_SPD,	_T("25"),
			{_("Higher values mean faster zoom with mouse wheel.\n"
				"Negative values will invert zoom direction.\n"
				"Results may vary depending on camera mode!")}	}

};

const category_sizes_map_type UI_ZOOM_entry ( _T("UI_ZOOM"), sizeof(UI_ZOOM) / Control_size );

//TODO is max_texture stages obsolte?
const Control RO_SLI[] = {
	{_("Shadow-map size"),              _T("ShadowMapSize"),       ID_RO_SLI_0,	_T("2048"), {_("higher value = better looking shadows\n"
																								"possible values: 1024, 2048, 4096, 8192")}	},
	{_("Tree view-distance"),           _T("TreeRadius"),          ID_RO_SLI_1,	_T("1000"), {_("sets the maximum distance at which trees will still be rendered")}	},
	{_("Terrain detail"),               _T("GroundDetail"),        ID_RO_SLI_2,	_T("80"), {_("higher value = more terrain details")}	},
	{_("Unit detail"),                  _T("UnitLodDist"),         ID_RO_SLI_3,	_T("300"), {_("higher value = more detailed units")}	},
	{_("Grass detail"),                 _T("GrassDetail"),         ID_RO_SLI_4,	_T("3"), {_("higher value = more detailed grass")}	},
	{_("Ground decals"),                _T("GroundDecals"),        ID_RO_SLI_5,	_T("0"), {_("only on/off available at this time")}	},
	{_("Unit icon distance"),           _T("UnitIconDist"),        ID_RO_SLI_6,	_T("350"), {_("determines at which range units are still fully rendered\n"
																								"higher value = greater range = more units rendered at the same time")}	},
	{_("Max simultaneous particles"),     _T("MaxParticles"),        ID_RO_SLI_7,	_T("5000")	, {_("limits how many particles are displayed at the same time")}}
	//{_("Max texture stages (SM3)"), _T("SM3MaxTextureStages"), ID_RO_SLI_8,	"6"	, {_("Decrease this if you are having bad perfomance on maps in sm3 format,\n"
	// not needed atm																				"increase if sm3 maps look ugly.")}}
};

const category_sizes_map_type RO_SLI_entry ( _T("RO_SLI"), sizeof(RO_SLI) / Control_size );

const Control VO_CBOX[] = {
	{_("Run full-screen"),                _T("Fullscreen"),              ID_WINDOWP_VO_CBOX_0,	_T("1"), {_("run fullscreen or in a window?")}},
	{_("Dual-screen mode"),               _T("DualScreenMode"),          ID_WINDOWP_VO_CBOX_1,	_T("0"), {_("if you have two monitors you can use both")}},
	{_("Enable v-sync"),                  _T("VSync"),                   ID_WINDOWP_VO_CBOX_2,	_T("0"), {_("V-Sync on/off")}}
};

const category_sizes_map_type VO_CBOX_entry ( _T("VO_BOX"), sizeof(VO_CBOX) / Control_size );

const Control VO_RBUT[] = {
	{_("16-bit Z-buffer"), _T("DepthBufferBits"), ID_WINDOWP_VO_RBUT_0,	_T("24"), {_("placeholder")}},
	{_("24-bit Z-buffer"), _T("DepthBufferBits"), ID_WINDOWP_VO_RBUT_1,	_T("24"), {_("placeholder")}}
};

const category_sizes_map_type VO_RBUT_entry ( _T("VO_RBUT"), sizeof(VO_RBUT) / Control_size );


const Control VO_SLI[] = {
	{_("Full-scene anti-aliasing samples"), _T("FSAALevel"), ID_VO_SLI_0,	_T("0"), {_("how much anti-aliasing should be applied")}}
};

const category_sizes_map_type VO_SLI_entry ( _T("VO_SLI"), sizeof(VO_SLI) / Control_size );

const Control VO_SLI_EXT[] = {
	{_(""), _T("FSAA"), -1, _T("0"), {_("")}}
};

const category_sizes_map_type VO_SLI_EXT_entry ( _T("VO_SLI_EXT"), sizeof(VO_SLI_EXT) / Control_size );

const Control AO_SLI[] = {
	{_("Maximum simultaneous sounds"), _T("MaxSounds"),            ID_AO_SLI_0,	_T("32") , {_("maximum different sounds played at the same time\n"
																								"Set this to zero to disable sound completely.")}},
	{_("Global sound volume"),   _T("SoundVolume"),                ID_AO_SLI_1,	_T("100"), {_("overall sound volume")}},
	{_("Unit reply volume"),     _T("UnitReplyVolume"),       ID_AO_SLI_2,	_T("100") , {_("reply volume relative to global volume")}}

};

const category_sizes_map_type AO_SLI_entry ( _T("AO_SLI"), sizeof(AO_SLI) / Control_size );

const Control QA_CBOX[] = {
    {_("Shadows (slow)"),                                  _T("Shadows"),             ID_WINDOWP_QA_CBOX_0,	_T("0"), {_("enable shadows?")}},
	{_("3D trees"),                 _T("3DTrees"),             ID_WINDOWP_QA_CBOX_1,	_T("1"), {_("want better looking trees?\n"
														"needs Geforce 2/Radeon 8500/Intel 830 or later class graphic card")}},
	{_("High-resolution clouds"), _T("AdvSky"),              ID_WINDOWP_QA_CBOX_2,	_T("0"), {_("want better looking sky?\n"
														"needs Geforce 5/Radeon 9500/Intel 915 or later class graphic card")}},
	{_("Dynamic clouds (slow)"),                           _T("DynamicSky"),          ID_WINDOWP_QA_CBOX_3,	_T("0"), {_("want moving clouds in the sky?")}},
	{_("Reflective units"),       _T("AdvUnitShading"),      ID_WINDOWP_QA_CBOX_4,	_T("0"), {_("shiny units?\n"
														"needs Geforce 5/Radeon 9500/Intel 915 or later class graphic card")}},
	{_("Never use shaders when rendering SM3 maps"),       _T("SM3ForceFallbackTex"), ID_WINDOWP_QA_CBOX_5,	_T("0"), {_("problems with sm3 maps? enable this")}},
	{_("Enable LuaShaders support"),                       _T("LuaShaders"),          ID_WINDOWP_QA_CBOX_6,	_T("1"), {_("makes for some cool effects")}},
	{_("Use Pixelbuffer objects"),                       _T("UsePBO"),          ID_WINDOWP_QA_CBOX_7,	_T("0"), {_("If supported, it speeds up the dynamic loading of terrain textures -> smoother camera movement")}},
	{_("Compress textures"),                                _T("CompressTextures"),ID_WINDOWP_QA_CBOX_8,	_T("0"), {_("Runtime texture compression. (Ideal for graphic cards with small amount of vram)")}},
	{_("High-resolution LOS textures"),                    _T("HighResLos"),          ID_WINDOWP_QA_CBOX_9,	_T("1"), {_("smoother Line of Sight overlays")}},
    {_("Draw smooth points"),                              _T("SmoothPoints"),        ID_WINDOWP_QA_CBOX_10,	_T("0"), {_("should points be anti-aliased")}},
	{_("Draw smooth lines"),                               _T("SmoothLines"),         ID_WINDOWP_QA_CBOX_11,	_T("0"), {_("should lines be anti-aliased")}},
};

const category_sizes_map_type QA_CBOX_entry ( _T("QA_CBOX"), sizeof(QA_CBOX) / Control_size );

const Control UI_CBOX[] = {

    {_("Issue commands on mini-map"),                       _T("MiniMapFullProxy"), ID_WINDOWP_UI_CBOX_2, _T("1"), {_("Issue orders on the mini-map like you would ")}},
	{_("Show commands on mini-map"),                       _T("MiniMapDrawCommands"), ID_WINDOWP_UI_CBOX_3,	_T("1"), {_("default value is \"on\"")}},
	{_("Draw icons on mini-map"),                          _T("MiniMapIcons"),        ID_WINDOWP_UI_CBOX_4,	_T("1"), {_("default value is \"on\"")}},
	{_("Draw markers on mini-map"),                        _T("MiniMapMarker"),       ID_WINDOWP_UI_CBOX_5,	_T("1"), {_("default value is \"on\"")}},
	{_("Mini-map on left (single screen)"), 				_T("MinimapOnLeft"),       ID_WINDOWP_UI_CBOX_6,	_T("1"), {_("left is the default")}},
	{_("Mini-map on left (dual screen)"),   				_T("DualScreenMiniMapOnLeft"), ID_WINDOWP_UI_CBOX_7,_T("1"), {_("left is the default")}},
	{_("Simplified mini-map colors"),                      _T("SimpleMiniMapColors"), ID_WINDOWP_UI_CBOX_8,	_T("0"), {_("Use less colors")}},

	{_("Team-colored nanospray"),                          _T("TeamNanoSpray"),       ID_WINDOWP_UI_CBOX_9,	_T("0"),
																				{_("Should nano particels be the color of your team?")}},
	{_("Colorized elevation map"),                         _T("ColorElev"),           ID_WINDOWP_UI_CBOX_10,	_T("1"), {_("makes differences in height clearer")}},

	{_("Show in-game clock"),                              _T("ShowClock"),           ID_WINDOWP_UI_CBOX_11,	_T("0"),
										{_("requires \"Enable LuaWidgets\" to be set.\nWill be displayed in the bottom right corner")}},
	{_("Show in-game player information"),                 _T("ShowPlayerInfo"),      ID_WINDOWP_UI_CBOX_12,	_T("0"),
										{_("requires \"Enable LuaWidgets\" to be set.\nWill be displayed in the bottom right corner")}},
	{_("Show in-game framerate"),                          _T("ShowFPS"),             ID_WINDOWP_UI_CBOX_13,	_T("0"),
										{_("requires \"Enable LuaWidgets\" to be set.\nWill be displayed in the bottom right corner")}},
//TODO is there even a reason that it should be disabled?
	{_("Fix rendering on alt-tab"),                        _T("FixAltTab"),           ID_WINDOWP_UI_CBOX_14,	_T("1"), {_("Do not change if not needed")}},
	{_("Disallow helper AI's"),                            _T("NoHelperAIs"),         ID_WINDOWP_UI_CBOX_15,	_T("0"), {_("Disables Economy AI, etc.\n"
																														"If enabled might screw with LuaUi.")}},
	{_("Enable scroll on window edge"),					_T("WindowedEdgeMove"),	ID_WINDOWP_UI_CBOX_16,	_T("1"), {_("Scroll the screen when mouse reaches the screen's edge.")}},
	{_("Invert Mouse"),									_T("InvertMouse"),			ID_WINDOWP_UI_CBOX_17,	_T("0"), {_("Inverts the Mouse Y-axis in FPS mode")}},
	{_("Use Hardware Cursor"),									_T("HardwareCursor"),			ID_WINDOWP_UI_CBOX_18,	_T("0"), {_("Use native OS mouse cursor (hardware accelerated)")}},


};

const category_sizes_map_type UI_CBOX_entry ( _T("UI_CBOX"), sizeof(UI_CBOX) / Control_size );

const Control MO_SLI[] = {
	{_("Overhead camera"),    _T("OverheadScrollSpeed"),    ID_MO_SLI_0,	_T("10"), {_("set the scroll speed (mouse + keyboard) for this mode")}},
	{_("Rotatable overhead camera"), _T("RotOverheadScrollSpeed"), ID_MO_SLI_1,	_T("10") , {_("set the scroll speed (mouse + keyboard) for this mode")}},
	{_("Total war camera"),          _T("TWScrollSpeed"),          ID_MO_SLI_2,	_T("10") , {_("set the scroll speed (mouse + keyboard) for this mode")}},
	{_("First person camera"),         _T("FPSScrollSpeed"),         ID_MO_SLI_3,	_T("10") , {_("set the scroll speed (mouse + keyboard) for this mode")}},
	{_("Free camera"),          _T("CamFreeScrollSpeed"),     ID_MO_SLI_4,	_T("100") , {_("set the scroll speed (mouse + keyboard) for this mode")}}
};

const category_sizes_map_type MO_SLI_entry ( _T("MO_SLI"), sizeof(MO_SLI) / Control_size );

const Control MO_SLI_EXT[] = {
	{_(""), _T("OverheadEnabled"), -1,	_T("1"), {_("Make this the default view when startins Spring.\n"
													"Can be changed ingame.")}},
	{_(""), _T("RotOverheadEnabled"), -1,	_T("1"), {_("Make this the default view when startins Spring.\n"
													"Can be changed ingame.")}},
	{_(""), _T("TWEnabled"), -1,		_T("1"), {_("Make this the default view when startins Spring.\n"
												"Can be changed ingame.")}},
	{_(""), _T("FPSEnabled"), -1,		_T("1"), {_("Make this the default view when startins Spring.\n"
												"Can be changed ingame.")}},
	{_(""), _T("CamFreeEnabled"), -1,	_T("1"), {_("Make this the default view when startins Spring.\n"
												"Can be changed ingame.")}},
};

const category_sizes_map_type MO_SLI_EXT_entry ( _T("MO_SLI_EXT"), sizeof(MO_SLI_EXT) / Control_size );

const Control DO_SLI[] = {
	{_("Console verbose level (0=min,10=max)"), _T("VerboseLevel"), ID_DO_SLI_0,	_T("10"), {_("How much information should be outputted?")}}
};

const category_sizes_map_type DO_SLI_entry ( _T("DO_SLI"), sizeof(DO_SLI) / Control_size );

const Control DO_CBOX[] = {
	{_("Catch AI exceptions"),        _T("CatchAIExceptions"), ID_WINDOWP_DO_CBOX_0,	_T("1"), {_("disable for AI debugging")}},
	{_("Send debug info to console"), _T("StdoutDebug"),       ID_WINDOWP_DO_CBOX_1,	_T("0"), {_("if disabled these will only be logged")}}
};

const category_sizes_map_type DO_CBOX_entry ( _T("DO_CBOX"), sizeof(DO_CBOX) / Control_size );

const Control WR_COMBOX[] = {
		{_("Basic"),                  _T("ReflectiveWater"), ID_WINDOWP_WR_COMBOX,	_T("1"), {_("Depending on the power of your graphics card,\n"
																								"selecting higher quality than basic can have a\n"
																								"major impact on Spring's performance.\n")}}
	/*{_("reflective",              "ReflectiveWater", ID_WINDOWP_WR_COMBOX_1,	"1"},
	{_("reflective + refractive", "ReflectiveWater", ID_WINDOWP_WR_COMBOX_2,	"1"},
	{_("dynamic",                 "ReflectiveWater", ID_WINDOWP_WR_COMBOX_3,	"1"}*/
};

const category_sizes_map_type WR_COMBOX_entry ( _T("WR_COMBOX"), sizeof(WR_COMBOX) / Control_size );

const wxString WR_COMBOX_CHOICES[] = {
	_("Basic"), _("Reflective"), _("Reflective + refractive"), _("Dynamic"), _("Bump-mapped")
};

const Control MO_CBOX[] = {
	{_("Invert mouse y-axis"),             _T("InvertMouse"),      ID_WINDOWC_MO_CBOX_0,	_T("1"), {_("swap up/down with down/up")}},
	{_("Mini-map 3-button mouse support"), _T("MiniMapFullProxy"), ID_WINDOWC_MO_CBOX_1,	_T("1"), {_("if you don't want to able to use that button, disable it here")}}
};

const category_sizes_map_type MO_CBOX_entry ( _T("MO_CBOX"), sizeof(MO_CBOX) / Control_size );

const Control MO_RBUT[] = {
	{_("Overhead"),            _T("CamMode"), ID_WINDOWC_MO_RBUT_0,	_T("1"), {_("Static bird's eye view")}},
	{_("Rotatable overhead"), _T("CamMode"), ID_WINDOWC_MO_RBUT_1,	_T("1"), {_("Same as overhead, but you can rotate around the z-axis")}},
	{_("Total war"),           _T("CamMode"), ID_WINDOWC_MO_RBUT_2,	_T("1"), {_("top-view camera, which can be tilted on the X axis")}},
	{_("First person"),        _T("CamMode"), ID_WINDOWC_MO_RBUT_3,	_T("1"), {_("Camera from unit's point of view")}},
	{_("Free camera"),         _T("CamMode"), ID_WINDOWC_MO_RBUT_4,	_T("1"), {_("Modify the view anyway you want")}}
};

const category_sizes_map_type MO_RBUT_entry ( _T("MO_RBUT"), sizeof(MO_RBUT) / Control_size );

const Control RC_TEXT[] = {
	{_(""), _T("XResolution"), -1,	_T("1024"), {_("screen width")}},
	{_(""), _T("YResolution"), -1,	_T("768") , {_("screen height")}}
};

const category_sizes_map_type RC_TEXT_entry ( _T("RC_TEXT"), sizeof(RC_TEXT) / Control_size );


const Control W4_CONTROLS[] = {
    //booleans = checkboxes
    {_("Blur reflection"), _T("BumpWaterBlurReflection"), ID_W4_BumpWaterBlurReflection , _T("1"), {_("")}},
    {_("Use depth texture"), _T("BumpWaterUseDepthTexture"), ID_W4_BumpWaterUseDepthTexture , _T("1"), {_("enables smoother blending on coastlines")}},
    {_("Shore waves"), _T("BumpWaterShoreWaves"), ID_W4_BumpWaterShoreWaves , _T("0"), {_("Enables shorewaves")}},
    {_("Reflection"), _T("BumpWaterReflection"), ID_W4_BumpWaterReflection , _T("1"), {_("Turn on water reflections")}},
    // select boxes
    {_("Reflection texture size"), _T("BumpWaterTexSizeReflection"), ID_W4_BumpWaterTexSizeReflection , _T("128"), {_("")}},
    {_("Refraction"), _T("BumpWaterRefraction"), ID_W4_BumpWaterRefraction , _T("1"), {_("Turn on water refractions.\n(0:=off, 1:=screencopy(fast), 2:=own rendering pass(slow)).")}},
    // spin control
    {_("Anisotropy"), _T("BumpWaterAnisotropy"), ID_W4_BumpWaterAnisotropy , _T("0"), {_("")}},
    //     {_(""), _T(""), ID_W4_ , _T(""), {_("")}},
    //     {_(""), _T(""), ID_W4_ , _T(""), {_("")}},
    //     {_(""), _T(""), ID_W4_ , _T(""), {_("")}},
    //     {_(""), _T(""), ID_W4_ , _T(""), {_("")}},

};

const wxString W4_REFRACTION_CHOICES[] = { _("off"), _("screencopy(fast)"), _("own rendering pass(slow)") };
const wxString W4_TEXSIZE_CHOICES[] = { _("128"), _("256"), _("512"), _T("1024") };

const category_sizes_map_type W4_CONTROLS_entry ( _T("W4_CONTROLS"), sizeof(W4_CONTROLS) / Control_size );

const category_sizes_map_type entries_[] = { RC_TEXT_entry, MO_RBUT_entry, MO_CBOX_entry, WR_COMBOX_entry, DO_CBOX_entry, DO_SLI_entry, MO_SLI_EXT_entry,
                                            MO_SLI_entry, UI_CBOX_entry, QA_CBOX_entry, AO_SLI_entry, AO_SLI_entry, VO_SLI_EXT_entry, VO_SLI_entry,
                                            VO_RBUT_entry, VO_CBOX_entry, RO_SLI_entry, UI_ZOOM_entry, W4_CONTROLS_entry};

static category_sizes_map s_category_sizes ( entries_ , entries_ + sizeof(entries_) / sizeof(entries_[0]) );


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
