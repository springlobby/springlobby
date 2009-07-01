#include "offlinebattle.h"
#include "spring.h"
#include "ui.h"

OfflineBattle::OfflineBattle(  const int id  ):
m_id( id ),
m_me( User(_T("")) )
{
	m_opts.founder = m_me.GetNick();
	AddUser( m_me );
	m_me.BattleStatus().spectator = true;
	m_me.BattleStatus().sync = true;
}

OfflineBattle::OfflineBattle():
m_id( 0 ),
m_me( User(_T("")) )
{
}

void OfflineBattle::StartSpring()
{
	spring().Run(*this);
	ui().OnSpringStarting();
}
