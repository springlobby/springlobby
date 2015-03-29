/* This file is part of the Springlobby (GPL v2 or later), see COPYING */


class IServer;

class ServerSelector
{
public:
	ServerSelector();
	~ServerSelector();
	IServer& GetServer();
	const IServer& GetServer() const;
	void SetCurrentServer(IServer* server);
	bool IsServerAvailible() const;

private:
	IServer* m_serv;
};

ServerSelector& serverSelector();
