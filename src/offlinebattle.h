#ifndef OFFLINEBATTLE_H_INCLUDED
#define OFFLINEBATTLE_H_INCLUDED

#include "ibattle.h"

class OfflineBattle : public IBattle
{
	public:
			OfflineBattle ( const int id ): m_id( id ) {}
			OfflineBattle ( ): m_id( 0 ) {}
			~OfflineBattle (){};

			User& GetMe();
			bool IsFounderMe() {return true;}

	protected:

			int m_id;
};

#endif // OFFLINEBATTLE_H_INCLUDED
