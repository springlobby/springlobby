/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H
#define SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H

#include "ibattle.h"
#include "user.h"

class Ui;
class BattleBot;
class MainSinglePlayerTab;


class SinglePlayerBattle: public IBattle
{
public:

	SinglePlayerBattle(MainSinglePlayerTab& msptab );
	virtual ~SinglePlayerBattle();

	virtual bool IsFounderMe() const {
		return true;
	}

	virtual User& GetMe() {
		return m_me;
	}
	virtual const User& GetMe() const {
		return m_me;
	}

	virtual void SendHostInfo( HostInfo update );
	virtual void SendHostInfo( const std::string& /*unused*/ ) { }

	virtual void StartSpring();
private:
	void RemoveUnfittingBots();

	MainSinglePlayerTab& m_sptab;

	User m_me;
};


#endif // SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H
