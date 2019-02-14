/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

// Autohost
// LODScale=1.000
// LODScaleReflection=1.000
// LODScaleRefraction=1.000
// LODScaleShadow=1.000
// MapArchives=1									// multiple maps per archive?
// MetalMapPalette=0
//
// FIXME: VSync Off=-1?

#include <wx/gdicmn.h>
#ifndef DEFS_HPP
#define DEFS_HPP

const unsigned int PWIN_SIZE_X = 895;
const unsigned int PWIN_SIZE_Y = 830;
const unsigned int CWIN_SIZE_X = 580;
const unsigned int CWIN_SIZE_Y = 360;

// ParentWin menu-item IDs
const int ID_MENUITEM_SAVE = 50;
const int ID_MENUITEM_RESET = 51;
const int ID_MENUITEM_QUIT = 52;
const int ID_MENUITEM_DISABLE_WARN = 53;
const int ID_MENUITEM_MODE = 60;
const int ID_MENUITEM_EXPERT = 62;
const int ID_MENUITEM_SIMPLE = 61;

const int ID_MENUITEM_HELP = 70;
const int ID_MENUITEM_ABOUT = 71;
const int ID_MENUITEM_CONTACT = 72;
const int ID_MENUITEM_CREDITS = 73;
const int ID_MENUITEM_BUGREPORT = 74;

// ParentWin TextInput IDs
const int ID_RES_CHOICES_LBOX_X = 1111;
const int ID_RES_CHOICES_LBOX_Y = 1112;

// ParentWin Rendering Options slider IDs
const int ID_RO_SLI_0 = 100;
const int ID_RO_SLI_1 = 101;
const int ID_RO_SLI_2 = 102;
const int ID_RO_SLI_3 = 103;
const int ID_RO_SLI_4 = 104;
const int ID_RO_SLI_5 = 105;
const int ID_RO_SLI_6 = 106;
const int ID_RO_SLI_7 = 107;
const int ID_RO_SLI_8 = 108;

// ParentWin Video Options checkbox IDs
const int ID_WINDOWP_VO_CBOX_0 = 250;
const int ID_WINDOWP_VO_CBOX_1 = 251;
const int ID_WINDOWP_VO_CBOX_2 = 252;

// ParentWin Video Options radiobutton IDs
const int ID_WINDOWP_VO_RBUT_0 = 280;
const int ID_WINDOWP_VO_RBUT_1 = 281;

// ParentWin Video Options slider IDs
const int ID_VO_SLI_0 = 200;

// tab_simple controls
const int ID_SIMPLE_QUAL_CBX = 390;
const int ID_SIMPLE_DETAIL_CBX = 391;
const int ID_SIMPLE_MODE_CBX = 392;
const int ID_SIMPLE_GOEXPERT_BUT = 393;


// UI Options checkbox IDs
const int ID_WINDOWP_UI_CBOX_1 = 360;
const int ID_WINDOWP_UI_CBOX_2 = 361;
const int ID_WINDOWP_UI_CBOX_3 = 362;
const int ID_WINDOWP_UI_CBOX_4 = 363;
const int ID_WINDOWP_UI_CBOX_5 = 364;
const int ID_WINDOWP_UI_CBOX_6 = 365;
const int ID_WINDOWP_UI_CBOX_7 = 366;
const int ID_WINDOWP_UI_CBOX_8 = 367;
const int ID_WINDOWP_UI_CBOX_9 = 368;
const int ID_WINDOWP_UI_CBOX_10 = 369;
const int ID_WINDOWP_UI_CBOX_11 = 370;
const int ID_WINDOWP_UI_CBOX_12 = 371;
const int ID_WINDOWP_UI_CBOX_13 = 372;
const int ID_WINDOWP_UI_CBOX_14 = 373;
const int ID_WINDOWP_UI_CBOX_15 = 374;
const int ID_WINDOWP_UI_CBOX_16 = 375;
const int ID_WINDOWP_UI_CBOX_17 = 376;
const int ID_WINDOWP_UI_CBOX_18 = 377;

//Zoom opt
const int ID_WINDOWP_UI_MW_SPD = 385;

//QUALITY OPTIONS checkbox IDs
const int ID_WINDOWP_QA_CBOX_0 = 300;
const int ID_WINDOWP_QA_CBOX_1 = 301;
const int ID_WINDOWP_QA_CBOX_2 = 302;
const int ID_WINDOWP_QA_CBOX_3 = 303;
const int ID_WINDOWP_QA_CBOX_4 = 304;
const int ID_WINDOWP_QA_CBOX_5 = 305;
const int ID_WINDOWP_QA_CBOX_6 = 306;
const int ID_WINDOWP_QA_CBOX_7 = 307;
const int ID_WINDOWP_QA_CBOX_8 = 308;
const int ID_WINDOWP_QA_CBOX_9 = 309;
const int ID_WINDOWP_QA_CBOX_10 = 310;
const int ID_WINDOWP_QA_CBOX_11 = 311;

const int ID_WINDOWP_WR_COMBOX = 320;

// ParentWin Audio Options slider IDs
const int ID_AO_SLI_0 = 400;
const int ID_AO_SLI_1 = 401;
const int ID_AO_SLI_2 = 402;
const int ID_AO_SLI_3 = 403;
const int ID_AO_SLI_4 = 404;
const int ID_AO_SLI_5 = 405;
const int ID_AO_SLI_6 = 406;

// ParentWin Debug Options checkbox IDs
const int ID_WINDOWP_DO_CBOX_0 = 540;


// ParentWin Debug Options slider IDs
const int ID_DO_SLI_0 = 500;

// ParentWin Water Rendering radiobutton IDs
const int ID_WINDOWP_WR_RBUT_0 = 550;
const int ID_WINDOWP_WR_RBUT_1 = 551;
const int ID_WINDOWP_WR_RBUT_2 = 552;
const int ID_WINDOWP_WR_RBUT_3 = 553;

// ChildWin Mouse Options checkbox IDs
const int ID_WINDOWC_MO_CBOX_0 = 650;
const int ID_WINDOWC_MO_CBOX_1 = 651;

// ChildWin Mouse Options radiobutton IDs
const int ID_WINDOWC_MO_RBUT_0 = 680;
const int ID_WINDOWC_MO_RBUT_1 = 681;
const int ID_WINDOWC_MO_RBUT_2 = 682;
const int ID_WINDOWC_MO_RBUT_3 = 683;
const int ID_WINDOWC_MO_RBUT_4 = 684;

// ChildWin Mouse Options slider IDs
const int ID_MO_SLI_0 = 660;
const int ID_MO_SLI_1 = 661;
const int ID_MO_SLI_2 = 662;
const int ID_MO_SLI_3 = 663;
const int ID_MO_SLI_4 = 664;

// panel_paths ids
const int ID_PATH_USYNC_BTN = 701;
const int ID_PATH_OK_BTN = 702;

//water 4 specific options
const int ID_W4_BumpWaterBlurReflection = 720;
const int ID_W4_BumpWaterUseDepthTexture = 721;
const int ID_W4_BumpWaterShoreWaves = 722;
const int ID_W4_BumpWaterReflection = 723;
const int ID_W4_BumpWaterTexSizeReflection = 724;
const int ID_W4_BumpWaterRefraction = 725;
const int ID_W4_BumpWaterAnisotropy = 726;

const int ID_SHADOW_CHOICES = 740;

#define SLI_STYLE (wxSL_LABELS)
#define WX_DEF_V wxDefaultValidator
const wxPoint WX_DEF_P(-1, -1);
const wxSize WX_DEF_S(-1, -1);
const wxSize WX_SLI_S(200, -1);

const int UPDATE_W4_CONTROLS = 2003;
const int UPDATE_VIDEO_MODE = 2002;
const int UPDATE_QA_BOXES = 2001;
const int UPDATE_ALL = 2000;
const int UPDATE_EXPERTMODE_WARNING_ON_SIMPLETAB = 2003;

const wxSize TAB_SIZE(700, 500);

//#include <wx/string.h>
#include <wx/intl.h>
//#include <string>
#include <map>
struct Control {
	const wxString lbl;
	const wxString key;
	int id;
	const wxString def; //default value for confighandler, not control
	const wxString tTip[1];
};

const int Control_size = sizeof(Control);

typedef std::map<const wxString, const int> category_sizes_map;
typedef category_sizes_map::value_type category_sizes_map_type;

const Control UI_ZOOM[] = {
    {_("Scrollwheel speed"), _T("ScrollWheelSpeed"), ID_WINDOWP_UI_MW_SPD, _T("25.0"), {_("Higher values mean faster zoom with mouse wheel.\n"
											  "Negative values will invert zoom direction.\n"
											  "Results may vary depending on camera mode!")}}

};

const category_sizes_map_type UI_ZOOM_entry(_T("UI_ZOOM"), sizeof(UI_ZOOM) / Control_size);

//TODO is max_texture stages obsolte?
const Control RO_SLI[] = {
    {_("Shadow-map size"), _T("ShadowMapSize"), ID_RO_SLI_0, _T("2048"), {_("higher value = better looking shadows\n"
									    "possible values: 1024, 2048, 4096, 8192")}},
    {_("Tree view-distance"), _T("TreeRadius"), ID_RO_SLI_1, _T("1408"), {_("sets the maximum distance at which trees will still be rendered")}},
    {_("Terrain detail"), _T("GroundDetail"), ID_RO_SLI_2, _T("40"), {_("higher value = more terrain details")}},
    {_("Unit LOD distance"), _T("UnitLodDist"), ID_RO_SLI_3, _T("1000"), {_("higher value = units will remain detailed even when zooming out")}},
    {_("Grass detail"), _T("GrassDetail"), ID_RO_SLI_4, _T("3"), {_("higher value = more detailed grass")}},
    {_("Ground decals"), _T("GroundDecals"), ID_RO_SLI_5, _T("1"), {_("settings higher than 1 might have unwelcome side-effects / be very resource hungry")}},
    {_("Unit icon distance"), _T("UnitIconDist"), ID_RO_SLI_6, _T("10000"), {_("determines at which range units are still fully rendered\n"
									       "higher value = greater range = more units rendered at the same time")}},
    {_("Max simultaneous particles"), _T("MaxParticles"), ID_RO_SLI_7, _T("1000"), {_("limits how many particles are displayed at the same time")}},
    {_("Max nano simultaneous particles"), _T("MaxNanoParticles"), ID_RO_SLI_8, _T("2500"), {_("limits how many particles are displayed at the same time")}}
    //  not needed atm {_("Max texture stages (SM3)"), _T("SM3MaxTextureStages"), ID_RO_SLI_8,	"6"	, {_("Decrease this if you are having bad perfomance on maps in sm3 format,\nincrease if sm3 maps look ugly.")}}
};

const category_sizes_map_type RO_SLI_entry(_T("RO_SLI"), sizeof(RO_SLI) / Control_size);

const Control VO_CBOX[] = {
    {_("Run full-screen"), _T("Fullscreen"), ID_WINDOWP_VO_CBOX_0, _T("1"), {_("run fullscreen or in a window?")}},
    {_("Dual-screen mode"), _T("DualScreenMode"), ID_WINDOWP_VO_CBOX_1, _T("0"), {_("if you have two monitors you can use both")}},
    {_("Enable v-sync"), _T("VSync"), ID_WINDOWP_VO_CBOX_2, _T("-1"), {_("V-Sync on/off")}}};

const category_sizes_map_type VO_CBOX_entry(_T("VO_BOX"), sizeof(VO_CBOX) / Control_size);

const Control VO_RBUT[] = {
    {_("16-bit Z-buffer"), _T("DepthBufferBits"), ID_WINDOWP_VO_RBUT_0, _T("24"), {_("placeholder")}},
    {_("24-bit Z-buffer"), _T("DepthBufferBits"), ID_WINDOWP_VO_RBUT_1, _T("24"), {_("placeholder")}}};

const category_sizes_map_type VO_RBUT_entry(_T("VO_RBUT"), sizeof(VO_RBUT) / Control_size);


const Control VO_SLI[] = {
    {_("Full-scene anti-aliasing samples"), _T("FSAALevel"), ID_VO_SLI_0, _T("0"), {_("how much anti-aliasing should be applied")}}};

const category_sizes_map_type VO_SLI_entry(_T("VO_SLI"), sizeof(VO_SLI) / Control_size);

const Control VO_SLI_EXT[] = {
    {wxEmptyString, _T("FSAA"), -1, _T("0"), {wxEmptyString}}};

const category_sizes_map_type VO_SLI_EXT_entry(_T("VO_SLI_EXT"), sizeof(VO_SLI_EXT) / Control_size);

const Control AO_SLI[] = {
    {_("Maximum simultaneous sounds"), _T("MaxSounds"), ID_AO_SLI_0, _T("128"), {_("maximum different sounds played at the same time\n"
										   "Set this to zero to disable sound completely.")}},
    {_("Master sound volume"), _T("snd_volmaster"), ID_AO_SLI_1, _T("60"), {_("master sound volume")}},
    {_("General sound volume"), _T("snd_general"), ID_AO_SLI_2, wxEmptyString, {_("general volume relative to master volume")}},
    {_("Unit reply volume"), _T("snd_volunitreply"), ID_AO_SLI_3, _T("100"), {_("reply volume relative to master volume")}},
    {_("Battle volume"), _T("snd_volbattle"), ID_AO_SLI_4, _T("100"), {_("battle volume relative to global volume")}},
    {_("User interface volume"), _T("snd_volui"), ID_AO_SLI_5, _T("100"), {_("ui volume relative to global volume")}},
    {_("Air Absorbtion factor"), _T("snd_airAbsorption"), ID_AO_SLI_6, _T("0.0"), {_("How far sounds will have high frequencies dampened")}}

};

const category_sizes_map_type AO_SLI_entry(_T("AO_SLI"), sizeof(AO_SLI) / Control_size);

const Control QA_CBOX[] = {
    {_("Shadows (slow)"), _T("Shadows"), ID_WINDOWP_QA_CBOX_0, _T("0"), {_("enable shadows?")}},
    {_("3D trees"), _T("3DTrees"), ID_WINDOWP_QA_CBOX_1, _T("1"), {_("want better looking trees?\n"
								     "needs Geforce 2/Radeon 8500/Intel 830 or later class graphic card")}},
    {_("High-resolution clouds"), _T("AdvSky"), ID_WINDOWP_QA_CBOX_2, _T("1"), {_("want better looking sky?\n"
										  "needs Geforce 5/Radeon 9500/Intel 915 or later class graphic card")}},
    {_("Dynamic clouds (slow)"), _T("DynamicSky"), ID_WINDOWP_QA_CBOX_3, _T("0"), {_("want moving clouds in the sky?")}},
    {_("Reflective units"), _T("AdvUnitShading"), ID_WINDOWP_QA_CBOX_4, _T("1"), {_("shiny units?\n"
										    "needs Geforce 5/Radeon 9500/Intel 915 or later class graphic card")}},
    {_("Never use shaders when rendering SM3 maps"), _T("SM3ForceFallbackTex"), ID_WINDOWP_QA_CBOX_5, _T("0"), {_("problems with sm3 maps? enable this")}},
    {_("Enable LuaShaders support"), _T("LuaShaders"), ID_WINDOWP_QA_CBOX_6, _T("1"), {_("makes for some cool effects")}},
    {_("Use Pixelbuffer objects"), _T("UsePBO"), ID_WINDOWP_QA_CBOX_7, _T("1"), {_("If supported, it speeds up the dynamic loading of terrain textures -> smoother camera movement")}},
    {_("Compress textures"), _T("CompressTextures"), ID_WINDOWP_QA_CBOX_8, _T("0"), {_("Runtime texture compression. (Ideal for graphic cards with small amount of vram)")}},
    {_("High-resolution LOS textures"), _T("HighResLos"), ID_WINDOWP_QA_CBOX_9, _T("0"), {_("smoother Line of Sight overlays")}},
    {_("Draw smooth points"), _T("SmoothPoints"), ID_WINDOWP_QA_CBOX_10, _T("0"), {_("should points be anti-aliased")}},
    {_("Draw smooth lines"), _T("SmoothLines"), ID_WINDOWP_QA_CBOX_11, _T("0"), {_("should lines be anti-aliased")}},
};

const category_sizes_map_type QA_CBOX_entry(_T("QA_CBOX"), sizeof(QA_CBOX) / Control_size);

const Control UI_CBOX[] = {

    {_("Issue commands on mini-map"), _T("MiniMapFullProxy"), ID_WINDOWP_UI_CBOX_2, _T("1"), {_("Issue orders on the mini-map like you would ")}},
    {_("Show commands on mini-map"), _T("MiniMapDrawCommands"), ID_WINDOWP_UI_CBOX_3, _T("1"), {_("default value is \"on\"")}},
    {_("Draw icons on mini-map"), _T("MiniMapIcons"), ID_WINDOWP_UI_CBOX_4, _T("1"), {_("default value is \"on\"")}},
    {_("Draw markers on mini-map"), _T("MiniMapMarker"), ID_WINDOWP_UI_CBOX_5, _T("1"), {_("default value is \"on\"")}},
    {_("Mini-map on left (single screen)"), _T("MinimapOnLeft"), ID_WINDOWP_UI_CBOX_6, wxEmptyString, {_("left is the default")}},
    {_("Mini-map on left (dual screen)"), _T("DualScreenMiniMapOnLeft"), ID_WINDOWP_UI_CBOX_7, _T("0"), {_("left is the default")}},
    {_("Simplified mini-map colors"), _T("SimpleMiniMapColors"), ID_WINDOWP_UI_CBOX_8, _T("0"), {_("Use less colors")}},

    {_("Team-colored nanospray"), _T("TeamNanoSpray"), ID_WINDOWP_UI_CBOX_9, _T("1"), {_("Should nano particels be the color of your team?")}},
    {_("Colorized elevation map"), _T("ColorElev"), ID_WINDOWP_UI_CBOX_10, _T("1"), {_("makes differences in height clearer")}},

    {_("Show in-game clock"), _T("ShowClock"), ID_WINDOWP_UI_CBOX_11, _T("1"), {_("requires \"Enable LuaWidgets\" to be set.\nWill be displayed in the bottom right corner")}},
    {_("Show in-game player information"), _T("ShowPlayerInfo"), ID_WINDOWP_UI_CBOX_12, _T("1"), {_("requires \"Enable LuaWidgets\" to be set.\nWill be displayed in the bottom right corner")}},
    {_("Show in-game framerate"), _T("ShowFPS"), ID_WINDOWP_UI_CBOX_13, _T("0"), {_("requires \"Enable LuaWidgets\" to be set.\nWill be displayed in the bottom right corner")}},
    //TODO is there even a reason that it should be disabled?
    {_("Fix rendering on alt-tab"), _T("FixAltTab"), ID_WINDOWP_UI_CBOX_14, _T("0"), {_("Do not change if not needed")}},
    {_("Disallow helper AI's"), _T("NoHelperAIs"), ID_WINDOWP_UI_CBOX_15, _T("0"), {_("Disables Economy AI, etc.\nIf enabled might screw with LuaUi.")}},
    {_("Enable scroll on window edge"), _T("WindowedEdgeMove"), ID_WINDOWP_UI_CBOX_16, _T("1"), {_("Scroll the screen when mouse reaches the screen's edge.")}},
    {_("Invert Mouse"), _T("InvertMouse"), ID_WINDOWP_UI_CBOX_17, _T("0"), {_("Inverts the Mouse Y-axis in FPS mode")}},
    {_("Use Hardware Cursor"), _T("HardwareCursor"), ID_WINDOWP_UI_CBOX_18, _T("0"), {_("Use native OS mouse cursor (hardware accelerated)")}},


};

const category_sizes_map_type UI_CBOX_entry(_T("UI_CBOX"), sizeof(UI_CBOX) / Control_size);

const Control MO_SLI[] = {
    {_("Overhead camera"), _T("OverheadScrollSpeed"), ID_MO_SLI_0, _T("10"), {_("set the scroll speed (mouse + keyboard) for this mode")}},
    {_("Rotatable overhead camera"), _T("RotOverheadScrollSpeed"), ID_MO_SLI_1, _T("10"), {_("set the scroll speed (mouse + keyboard) for this mode")}},
    {_("Total war camera"), _T("TWScrollSpeed"), ID_MO_SLI_2, _T("10"), {_("set the scroll speed (mouse + keyboard) for this mode")}},
    {_("First person camera"), _T("FPSScrollSpeed"), ID_MO_SLI_3, _T("10"), {_("set the scroll speed (mouse + keyboard) for this mode")}},
    {_("Free camera"), _T("CamFreeScrollSpeed"), ID_MO_SLI_4, _T("500.0"), {_("set the scroll speed (mouse + keyboard) for this mode")}}};

const category_sizes_map_type MO_SLI_entry(_T("MO_SLI"), sizeof(MO_SLI) / Control_size);

const Control MO_SLI_EXT[] = {
    {wxEmptyString, _T("OverheadEnabled"), -1, _T("1"), {_("Make this the default view when startins Spring.\n"
							   "Can be changed ingame.")}},
    {wxEmptyString, _T("RotOverheadEnabled"), -1, _T("1"), {_("Make this the default view when startins Spring.\n"
							      "Can be changed ingame.")}},
    {wxEmptyString, _T("TWEnabled"), -1, _T("1"), {_("Make this the default view when startins Spring.\n"
						     "Can be changed ingame.")}},
    {wxEmptyString, _T("FPSEnabled"), -1, _T("1"), {_("Make this the default view when startins Spring.\n"
						      "Can be changed ingame.")}},
    {wxEmptyString, _T("CamFreeEnabled"), -1, _T("0"), {_("Make this the default view when startins Spring.\n"
							  "Can be changed ingame.")}},
};

const category_sizes_map_type MO_SLI_EXT_entry(_T("MO_SLI_EXT"), sizeof(MO_SLI_EXT) / Control_size);

const Control DO_SLI[] = {
    {_("Console verbose level (0=min,10=max)"), _T("VerboseLevel"), ID_DO_SLI_0, wxEmptyString, {_("How much information should be outputted?")}}};

const category_sizes_map_type DO_SLI_entry(_T("DO_SLI"), sizeof(DO_SLI) / Control_size);

const Control DO_CBOX[] = {
    {_("Catch AI exceptions"), _T("CatchAIExceptions"), ID_WINDOWP_DO_CBOX_0, _T("1"), {_("disable for AI debugging")}}};

const category_sizes_map_type DO_CBOX_entry(_T("DO_CBOX"), sizeof(DO_CBOX) / Control_size);

const Control WR_COMBOX[] = {
    {_("Basic"), _T("ReflectiveWater"), ID_WINDOWP_WR_COMBOX, _T("1"), {_("Depending on the power of your graphics card,\n"
									  "selecting higher quality than basic can have a\n"
									  "major impact on Spring's performance.\n")}}
    /*{_("reflective",              "ReflectiveWater", ID_WINDOWP_WR_COMBOX_1,	"1"},
	{_("reflective + refractive", "ReflectiveWater", ID_WINDOWP_WR_COMBOX_2,	"1"},
	{_("dynamic",                 "ReflectiveWater", ID_WINDOWP_WR_COMBOX_3,	"1"}*/
};

const category_sizes_map_type WR_COMBOX_entry(_T("WR_COMBOX"), sizeof(WR_COMBOX) / Control_size);

const wxString WR_COMBOX_CHOICES[] = {
    _("Basic"), _("Reflective"), _("Reflective + refractive"), _("Dynamic"), _("Bump-mapped")};

const Control MO_CBOX[] = {
    {_("Invert mouse y-axis"), _T("InvertMouse"), ID_WINDOWC_MO_CBOX_0, _T("0"), {_("swap up/down with down/up")}},
    {_("Mini-map 3-button mouse support"), _T("MiniMapFullProxy"), ID_WINDOWC_MO_CBOX_1, _T("1"), {_("if you don't want to able to use that button, disable it here")}}};

const category_sizes_map_type MO_CBOX_entry(_T("MO_CBOX"), sizeof(MO_CBOX) / Control_size);

const Control MO_RBUT[] = {
    {_("Overhead"), _T("CamMode"), ID_WINDOWC_MO_RBUT_0, _T("5"), {_("Static bird's eye view")}},
    {_("Rotatable overhead"), _T("CamMode"), ID_WINDOWC_MO_RBUT_1, _T("5"), {_("Same as overhead, but you can rotate around the z-axis")}},
    {_("Total war"), _T("CamMode"), ID_WINDOWC_MO_RBUT_2, _T("5"), {_("top-view camera, which can be tilted on the X axis")}},
    {_("First person"), _T("CamMode"), ID_WINDOWC_MO_RBUT_3, _T("5"), {_("Camera from unit's point of view")}},
    {_("Free camera"), _T("CamMode"), ID_WINDOWC_MO_RBUT_4, _T("5"), {_("Modify the view anyway you want")}}};

const category_sizes_map_type MO_RBUT_entry(_T("MO_RBUT"), sizeof(MO_RBUT) / Control_size);

const Control RC_TEXT[] = {
    {wxEmptyString, _T("XResolution"), -1, wxEmptyString, {_("screen width")}},
    {wxEmptyString, _T("YResolution"), -1, wxEmptyString, {_("screen height")}}};

const category_sizes_map_type RC_TEXT_entry(_T("RC_TEXT"), sizeof(RC_TEXT) / Control_size);


const Control W4_CONTROLS[] = {
    //booleans = checkboxes
    {_("Blur reflection"), _T("BumpWaterBlurReflection"), ID_W4_BumpWaterBlurReflection, _T("0"), {wxEmptyString}},
    {_("Use depth texture"), _T("BumpWaterUseDepthTexture"), ID_W4_BumpWaterUseDepthTexture, _T("1"), {_("enables smoother blending on coastlines")}},
    {_("Shore waves"), _T("BumpWaterShoreWaves"), ID_W4_BumpWaterShoreWaves, _T("1"), {_("Enables shorewaves")}},
    {_("Reflection"), _T("BumpWaterReflection"), ID_W4_BumpWaterReflection, _T("1"), {_("Turn on water reflections")}},
    // select boxes
    {_("Reflection texture size"), _T("BumpWaterTexSizeReflection"), ID_W4_BumpWaterTexSizeReflection, _T("512"), {wxEmptyString}},
    {_("Refraction"), _T("BumpWaterRefraction"), ID_W4_BumpWaterRefraction, _T("1"), {_("Turn on water refractions.\n(0:=off, 1:=screencopy(fast), 2:=own rendering pass(slow)).")}},
    // spin control
    {_("Anisotropy"), _T("BumpWaterAnisotropy"), ID_W4_BumpWaterAnisotropy, _T("0.0"), {wxEmptyString}},

};

const wxString W4_REFRACTION_CHOICES[] = {_("off"), _("screencopy(fast)"), _("own rendering pass(slow)")};
const wxString W4_TEXSIZE_CHOICES[] = {_T("128"), _T("256"), _T("512"), _T("1024")};
const wxString SHADOW_CHOICES[] = {_T("disabled"), _T("off"), _T("on")};

const category_sizes_map_type W4_CONTROLS_entry(_T("W4_CONTROLS"), sizeof(W4_CONTROLS) / Control_size);

const category_sizes_map_type entries_[] = {RC_TEXT_entry, MO_RBUT_entry, MO_CBOX_entry, WR_COMBOX_entry, DO_CBOX_entry, DO_SLI_entry, MO_SLI_EXT_entry,
					    MO_SLI_entry, UI_CBOX_entry, QA_CBOX_entry, AO_SLI_entry, AO_SLI_entry, VO_SLI_EXT_entry, VO_SLI_entry,
					    VO_RBUT_entry, VO_CBOX_entry, RO_SLI_entry, UI_ZOOM_entry, W4_CONTROLS_entry};

static category_sizes_map s_category_sizes(entries_, entries_ + sizeof(entries_) / sizeof(entries_[0]));

#endif
