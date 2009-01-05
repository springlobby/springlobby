#ifndef OFFLINEBATTLE_H_INCLUDED
#define OFFLINEBATTLE_H_INCLUDED

#include "ibattle.h"

class OfflineBattle : public IBattle
{
	public:
			OfflineBattle ( const int id ): m_id( id ), m_me( User(_T("")) ) {}
			OfflineBattle ( ): m_id( 0 ), m_me( User(_T("")) ) {}
			~OfflineBattle (){};

			User& GetMe() { return m_me; }
			bool IsFounderMe() {return true;}

	protected:

			int m_id;
			User m_me;
};

#endif // OFFLINEBATTLE_H_INCLUDED
