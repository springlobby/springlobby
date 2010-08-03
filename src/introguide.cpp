#include "introguide.h"

#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#if wxUSE_TOGGLEBTN
#include <wx/tglbtn.h>
#endif
#include <wx/checkbox.h>

#include "ui.h"
#include "mainwindow.h"
#include "uiutils.h"

BEGIN_EVENT_TABLE( IntroGuide, wxFrame )
	EVT_TEXT_URL        (	TEXT_DISPLAY,  IntroGuide::OnLinkEvent      )
#if  wxUSE_TOGGLEBTN
EVT_TOGGLEBUTTON        ( SPRING_DESC, IntroGuide::OnSpringDesc         )
EVT_TOGGLEBUTTON        ( INSTALL_DESC, IntroGuide::OnInstallDesc       )
EVT_TOGGLEBUTTON        ( SINGLE_DESC, IntroGuide::OnSingleDesc         )
EVT_TOGGLEBUTTON        ( MULTI_DESC, IntroGuide::OnMultiDesc           )
EVT_TOGGLEBUTTON        ( GRAPHIC_TROUB, IntroGuide::OnGraphicTroubles  )
EVT_TOGGLEBUTTON        ( HELP_LINKS, IntroGuide::OnWikiLinks           )
#else
EVT_CHECKBOX            ( SPRING_DESC, IntroGuide::OnSpringDesc         )
EVT_CHECKBOX            ( INSTALL_DESC, IntroGuide::OnInstallDesc       )
EVT_CHECKBOX            ( SINGLE_DESC, IntroGuide::OnSingleDesc         )
EVT_CHECKBOX            ( MULTI_DESC, IntroGuide::OnMultiDesc           )
EVT_CHECKBOX            ( GRAPHIC_TROUB, IntroGuide::OnGraphicTroubles  )
EVT_CHECKBOX            ( HELP_LINKS, IntroGuide::OnWikiLinks           )
#endif
END_EVENT_TABLE()
IntroGuide::IntroGuide():
wxFrame( (wxFrame*)&ui().mw(), -1, _("Newbies guide"), wxPoint(50, 50), wxSize(800,600) )
{
	SpringOverview << _("Spring is just an engine, alone it won't do much, mods are games that run on the engine.\n");
	SpringOverview << _("You will need to install at least a map and a mod in order to play.\n\n");
	SpringOverview << _("As a user, you can extend functionality downloading small scripts (LuaUI Widgets) and activate them ingame trough the F11-button list.\n\n");
	SpringOverview << _("For players with previous RTS experience, one tricky part of spring is that fog of war is present, but not visualized by default ingame until you press the \"L\" button.\n\n");
	SpringOverview << _("For developers, spring is highly customizable, with a scripting engine, you'd want to check out the forum/wiki to get started.\n"); // spring overview
	InstallOverview << _("If you skip the automatic content installer for some reason, you can still download and install content manually.\n");
	InstallOverview << _("The most popular download site is http://www.springfiles.com .\n");
	InstallOverview << _("The installation directory, differs according to the operative system:\n");
	InstallOverview << _("Windows XP: Spring's installation path.\n");
	InstallOverview << _("Windows Vista/Windows 7: C:\\Users\\username\\App Data\\local\\spring (This folder is default insibie, change your file manager settings to display it).\n");
	InstallOverview << _("Linux: ~/.spring (this folder is default invisible, you might want to create a visible link using \"ln -s ~/.spring ~/spring\").\n");
	InstallOverview << _("As obvious as it could be, maps belong to the maps subdirectory, mods to the mods subdirectory, and widgets to LuaUI\\Widgets subdirectory.\n\n");
	InstallOverview << _("In order for the lobby to detect correctly a freshly installed map/mod, you'll need to trigger a refresh, to do it click on tools->Reload map/mod list."); // installing content
	SinglePlayer << _("Select in order, first the mod, then the map.\n");
	SinglePlayer << _("You'll need to add at least one bot to play, if the mod ship it's own AI, it's highly reccomended to use that since bots are not compatible with all mods.\n");
	SinglePlayer << _("You can drag both the bot and yourself(the sun icon) anywhere, map's default start positions will snap on.\n");
	SinglePlayer << _("You can change alliances (same number equals to allied players) and colour by hovering the icon.\n");
	SinglePlayer << _("Bonus number is a resource multiplier bonus, if the bot feels too weak, try increasing the number on the bot or add more of them, if it feels too strong, do on yourself.\n");
	SinglePlayer << _("EVEN IF THE BOT DOESN'T APPEAR IN THE PLAYER LIST INGAME, HE'LL STILL BE THERE, EXPLORE THE WHOLE MAP TO FIND IT.");
	MultiPlayer << _("The lobby has an irc-like chat for talk and a battlelist\n");
	MultiPlayer << _("You can get a list of available chat commands using /help\n");
	MultiPlayer << _("Asking questions such as \"Is anyone here\" or \"Can anyone help me\" is actually considered rude if you don't specify your problem.\n");
	MultiPlayer << _("It's also highly reccomended to avoid asking questions such as \"what is the best mod\" since it's a very touchy subject for many players.\n");
	MultiPlayer << _("In order to play online, you'll NEED the latest STABLE OFFICIAL RELEASE, older or newer or development versions won't work.\n\n\n");
	MultiPlayer << _("Joining a battle:\n");
	MultiPlayer << _("In order to join a battle, simply double click on it on the list, and the lobby will prompt you for downloading the necesary files.\n");
	MultiPlayer << _("When you are in a battleroom, focus your attention to the middle bar on the the screen.\n");
	MultiPlayer << _("When two or more players have the same team number, they will share controls (also referred as comshare or coop).\n");
	MultiPlayer << _("When two or more players have the same ally number, they will be allied togheter.\n");
	MultiPlayer << _("The host has possibility to override your values anytime with the exception of spectator.\n");
	MultiPlayer << _("If you are not a spectator, you will have to press the ready button once you're ready to play.");
	MultiPlayer << _("The unsynced status (the ! status icon) appears when you lack the map or the mod, or they are corrupted, you might need a list refresh to update them (tools->reload map/mod list).\n");
	MultiPlayer << _("If you are insecure about the gameplay of a mod, it's suggested that you activate the spectator button at first, so you'll be able to observe other players to learn.\n");
	MultiPlayer << _("The game will start when the host decides so, just sit down and wait (don't forget to press ready).\n");
	MultiPlayer << _("Some hosts are automatically controlled (autohosts), they can be controlled trough a list of chat commands ( for the list, send !help in the chat).\n\n\n");
	MultiPlayer << _("Hosting a battle:\n");
	MultiPlayer << _("Almost the totality of the options in the hosting dialog cannot be changed without reopening the battle (sorry this won't change' in the forseeable future).\n");
	MultiPlayer << _("You'll need to open the port indicated in the hosting dialog (default: 8452), type: UDP in your firewall/router in order to let other players to join your game.\n");
	MultiPlayer << _("Alternatively, you can enable the option \"Hole punching\"(not reccomended) which uses a small hack to bypass the firewall, but it might not work for all the firewalls/routers, and it might not work for all the clients.\n");
	MultiPlayer << _("More reccomended instead, is to use the option \"Use Relayhost\", which will spawn a bot that your lobby will control, the bot will host for you instead.\n"); // online play
	GraphicProblems << _("Most of the graphic-related problems come from old or bad video drivers.\n");
	GraphicProblems << _("Before considering \"But XYZ game works fine!\", remember that most games on Windows uses DirectX, while Spring uses OpenGL, also the OpenGL spec is rather large, and certain bugs appear only with very specific conditions.\n");
	GraphicProblems << _("That being said, the first thing you should do open the Spring settings editor, and lower the graphic detail, some video cards don't work well with some advanced options.\n");
	GraphicProblems << _("If the problem persists, try to update your video drivers to the latest version (as weird as it may sound, ATI users might actually want to try older versions too, see the forums for an explanation).\n");
	GraphicProblems << _("If your map/models are white or generally untextured, and you're using Linux, there's a solution at  http://springrts.com/phpbb/viewtopic.php?f=20&t=20809.\n\n");
	GraphicProblems << _("If you're using an integrated card, chances are you're simply unable to run smoothly Spring, because of the high requirements which those chips can't provide.\n\n");
	GraphicProblems << _("For further help, visit the help section of the forums at http://springrts.com ."); // graphic problems
	UsefulLinks << _("Main website: http://springrts.com\n");
	UsefulLinks << _("Lobby website: http://springlobby.info\n");
	UsefulLinks << _("Games page: http://springrts.com/wiki/Games\n");
	UsefulLinks << _("General info website: http://springinfo.info\n");
	UsefulLinks << _("Wiki: http://springrts.com/wiki\n");
	UsefulLinks << _("Frequently Asked Questions: http://springrts.com/wiki/FAQ\n");
	UsefulLinks << _("Development infos: http://springrts.com/wiki/Engine_Development\n"); // guides and links

	wxBoxSizer* MainSizer = new wxBoxSizer(wxVERTICAL);
	wxFlexGridSizer* ButtonSizer = new wxFlexGridSizer(3,2);
	wxBoxSizer* TextSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* HeaderSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* Heading = new wxStaticText( this, -1, _("Introduction to Spring and Frequently Asked Questions\nIt's HIGHLY reccomended you read this troughroughtfuly\nClick on a button to view the topic"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	Heading->SetForegroundColour(wxColour(20,0,200));
	wxFont tempfont = Heading->GetFont();
	tempfont.SetPointSize(14);
	Heading->SetFont(tempfont);
	HeaderSizer->Add(Heading,1,wxEXPAND|wxRIGHT);
#if wxUSE_TOGGLEBTN
	SpringDescription = new wxToggleButton( this, SPRING_DESC, _( "Spring overview" ) );
	InstallingContent = new wxToggleButton( this, INSTALL_DESC, _( "Installing content" ) );
	SingleDescription = new wxToggleButton( this, SINGLE_DESC, _( "Singleplayer mode" ) );
	MultiDescription = new wxToggleButton( this, MULTI_DESC, _( "Multiplayer mode" ) );
	GraphicTroubles = new wxToggleButton( this, GRAPHIC_TROUB, _( "Graphic and performance problems" ) );
	WikiLinks = new wxToggleButton( this, HELP_LINKS, _( "Useful links" ) );
#else
	SpringDescription = new wxCheckBox( this, SPRING_DESC, _( "Spring overview" ) );
	InstallingContent = new wxCheckBox( this, INSTALL_DESC, _( "Installing content" ) );
	SingleDescription = new wxCheckBox( this, SINGLE_DESC, _( "Singleplayer mode" ) );
	MultiDescription = new wxCheckBox( this, MULTI_DESC, _( "Multiplayer mode" ) );
	GraphicTroubles = new wxCheckBox( this, GRAPHIC_TROUB, _( "Graphic and performance problems" ) );
	WikiLinks = new wxCheckBox( this, HELP_LINKS, _( "Useful links" ) );
#endif
	ButtonSizer->Add(SpringDescription,0,wxEXPAND|wxRIGHT);
	ButtonSizer->Add(InstallingContent,0,wxEXPAND|wxRIGHT);
	ButtonSizer->Add(SingleDescription,0,wxEXPAND|wxRIGHT);
	ButtonSizer->Add(MultiDescription,0,wxEXPAND|wxRIGHT);
	ButtonSizer->Add(GraphicTroubles,0,wxEXPAND|wxRIGHT);
	ButtonSizer->Add(WikiLinks,0,wxEXPAND|wxRIGHT);

	m_text_stuff = new wxTextCtrl( this, TEXT_DISPLAY, _T( "" ), wxDefaultPosition, wxSize(-1,-1), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_AUTO_URL );
	TextSizer->Add(m_text_stuff,1,wxEXPAND|wxTOP,2);

	MainSizer->Add(HeaderSizer);
	MainSizer->Add(ButtonSizer);
	MainSizer->Add(TextSizer, 4, wxEXPAND | wxALL, 2);

	SpringDescription->SetValue(true); // default activate spring help text
	m_text_stuff->SetValue(SpringOverview);

	SetSizer(MainSizer);
	Layout();
	Refresh();
}

void IntroGuide::OnLinkEvent( wxTextUrlEvent& event )
{
	if ( !event.GetMouseEvent().LeftDown() )
		return;

	wxString url = m_text_stuff->GetRange( event.GetURLStart(), event.GetURLEnd());
	OpenWebBrowser( url );
}

void IntroGuide::OnSpringDesc(wxCommandEvent& /*unused*/)
{
	SpringDescription->SetValue(true);
	InstallingContent->SetValue(false);
	SingleDescription->SetValue(false);
	MultiDescription->SetValue(false);
	GraphicTroubles->SetValue(false);
	WikiLinks->SetValue(false);
	m_text_stuff->SetValue(SpringOverview);
}

void IntroGuide::OnInstallDesc(wxCommandEvent& /*unused*/)
{
	SpringDescription->SetValue(false);
	InstallingContent->SetValue(true);
	SingleDescription->SetValue(false);
	MultiDescription->SetValue(false);
	GraphicTroubles->SetValue(false);
	WikiLinks->SetValue(false);
	m_text_stuff->SetValue(InstallOverview);
}

void IntroGuide::OnSingleDesc(wxCommandEvent& /*unused*/)
{
	SpringDescription->SetValue(false);
	InstallingContent->SetValue(false);
	SingleDescription->SetValue(true);
	MultiDescription->SetValue(false);
	GraphicTroubles->SetValue(false);
	WikiLinks->SetValue(false);
	m_text_stuff->SetValue(SinglePlayer);
}

void IntroGuide::OnMultiDesc(wxCommandEvent& /*unused*/)
{
	SpringDescription->SetValue(false);
	InstallingContent->SetValue(false);
	SingleDescription->SetValue(false);
	MultiDescription->SetValue(true);
	GraphicTroubles->SetValue(false);
	WikiLinks->SetValue(false);
	m_text_stuff->SetValue(MultiPlayer);
}

void IntroGuide::OnGraphicTroubles(wxCommandEvent& /*unused*/)
{
	SpringDescription->SetValue(false);
	InstallingContent->SetValue(false);
	SingleDescription->SetValue(false);
	MultiDescription->SetValue(false);
	GraphicTroubles->SetValue(true);
	WikiLinks->SetValue(false);
	m_text_stuff->SetValue(GraphicProblems);
}

void IntroGuide::OnWikiLinks(wxCommandEvent& /*unused*/)
{
	SpringDescription->SetValue(false);
	InstallingContent->SetValue(false);
	SingleDescription->SetValue(false);
	MultiDescription->SetValue(false);
	GraphicTroubles->SetValue(false);
	WikiLinks->SetValue(true);
	m_text_stuff->SetValue(UsefulLinks);
}

