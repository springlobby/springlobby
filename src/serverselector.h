/* This file is part of the Springlobby (GPL v2 or later), see COPYING */


class IServer;

class ServerSelector {
public:
	ServerSelector();
	IServer& GetServer();
	const IServer& GetServer() const;
	void SetCurrentServer(IServer* server);
	bool    IsServerAvailible() const;
protected:
	IServer* m_serv;
};

ServerSelector& serverSelector();
