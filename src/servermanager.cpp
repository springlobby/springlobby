/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "servermanager.h"
#include "user.h"
#include "serverselector.h"
#include "iserver.h"
#include "utils/conversion.h"
#include "downloader/prdownloader.h"
#include "settings.h"
#include "gui/ui.h"
#include "gui/mainwindow.h"
#include "gui/chatpanel.h"
#include "gui/mainchattab.h"
#include "utils/slconfig.h"
#include <wx/log.h>

ServerManager::ServerManager()
{
}

ServerManager::~ServerManager()
{
}

//Get instance of single object
ServerManager* ServerManager::Instance()
{
	if (m_Instance == nullptr) {
		m_Instance = new ServerManager();
	}
	return m_Instance;
}

//Release singleton
void ServerManager::Release()
{
	if (m_Instance != nullptr) {
		delete m_Instance;
	}
	m_Instance = nullptr;
}

//Try to connect to server
void ServerManager::ConnectToServer()
{
	if (serverSelector().GetServer().IsConnected() == true) {
		//TODO: Maybe call ReconnectToServer() before return?
		return;
	}

	const wxString server_name = sett().GetDefaultServer();
	const wxString nick = sett().GetServerAccountNick(server_name);
	const wxString pass = sett().GetServerAccountPass(server_name);
	bool autoconnect = cfg().ReadBool(_T( "/Server/Autoconnect" ));

	//TODO: Do not ask user about credentials in such way.
	if (!autoconnect || server_name.IsEmpty() || nick.IsEmpty() || pass.IsEmpty()) {
		ui().ShowConnectWindow();
		return;
	}

	//Do actual connection
	DoConnectToServer(server_name, nick, pass);
}

//Reconnecto to server
void ServerManager::ReconnectToServer()
{
	//TODO: Implement reconnection delay here!
	const wxString servname = sett().GetDefaultServer();
	const wxString pass = sett().GetServerAccountPass(servname);

	//TODO: Need to be reworked
	if (sett().GetServerAccountSavePass(servname) == false) {
		ui().ShowConnectWindow();
		return;
	}

	//Do actual connection
	DoConnectToServer(servname, sett().GetServerAccountNick(servname), pass);
}

//Disconnect from server
void ServerManager::DisconnectFromServer()
{
	if (serverSelector().GetServer().IsConnected()) {
		serverSelector().GetServer().Disconnect();
	}
}

//Register new Player on server
void ServerManager::RegisterNewUser(const wxString& servername, const wxString& username, const wxString& password)
{
	//TODO: No ui calls here!
	if (sett().ServerExists(servername) == false) {
		ui().OnRegistrationDenied(_T("Server does not exist in settings"));
		return;
	}
	serverSelector().GetServer().Register(STD_STRING(servername), STD_STRING(sett().GetServerHost(servername)), sett().GetServerPort(servername), STD_STRING(username), STD_STRING(password));
}

//Join chat channel
void ServerManager::JoinChannel(const wxString& name, const wxString& password)
{
	serverSelector().GetServer().JoinChannel(STD_STRING(name), STD_STRING(password));
}

//Download engine, map or mod
bool ServerManager::DownloadContent(const std::string& category, const std::string& name, const std::string& /*hash*/)
{
	int downloadJobsCreated = prDownloader().GetDownload(category, name);

	if (downloadJobsCreated < 1) {
		wxLogError(_("prDownloader failed to create thread!"));
		return false;
	}
	return true;
}

//Simple wrapper. Maybe not needed
bool ServerManager::IsConnected()
{
	return serverSelector().GetServer().IsConnected();
}

//Opens the accutial connection to a server.
void ServerManager::DoConnectToServer(const wxString& servername, const wxString& username, const wxString& password)
{
	IServer* server = &serverSelector().GetServer();

	//Disconnect from server if not done before
	if (server->IsConnected() == true) {
		server->Disconnect();
	}

	//FIXME: Does this two methods are really needed?
	server->SetUsername(STD_STRING(username));
	server->SetPassword(STD_STRING(password));

	const wxString host = sett().GetServerHost(servername);
	const int port = sett().GetServerPort(servername);

	//TODO: Rework this!!!
	if (server->uidata.panel == nullptr) {
		server->uidata.panel = ui().mw().GetChatTab().AddChatPanel(*server, servername);
	}
	server->uidata.panel->StatusMessage(_T("Connecting to server ") + servername + _T("..."));

	// Connect
	server->Connect(STD_STRING(servername), STD_STRING(host), port);
}

ServerManager* ServerManager::m_Instance = nullptr;
