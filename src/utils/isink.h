#ifndef SPRINGLOBBY_HEADERGUARD_ISINK_H
#define SPRINGLOBBY_HEADERGUARD_ISINK_H

#include "globalevents.h"


template <class Derived, class EventDataType = GlobalEvents::GlobalEventData >
class UnitsyncReloadedSink {
	protected:
		Derived& asImp () { return static_cast<Derived&>(*this); }
		const Derived& asImp () const { return static_cast<const Derived&>(*this); }

		typedef UnitsyncReloadedSink<Derived,EventDataType>
			BaseType;

	public:
		void OnUnitsyncReloaded( EventDataType data ) { asImp().OnUnitsyncReloaded( data ); }

    protected:
        typedef EventReceiverFunc<UnitsyncReloadedSink, EventDataType, &UnitsyncReloadedSink::OnUnitsyncReloaded>
            EventReceiverFunction;
        EventReceiverFunction OnUsync_reload;

    public:
		UnitsyncReloadedSink ()
			: OnUsync_reload( this, &GetGlobalEventSender( GlobalEvents::OnUnitsyncReloaded ) )
		{}
};


#endif // SPRINGLOBBY_HEADERGUARD_ISINK_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/
