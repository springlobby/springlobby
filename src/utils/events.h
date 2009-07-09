#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED

#include <assert.h>

#ifndef my_assert
#define my_assert(x) assert(x)
#endif

/// Double linked cyclic list implementation.
/// Beware! When a node is copied (duplicated), the list of a copy is empty!
class ListNodeBare {
	public:

		ListNodeBare * prev;
		ListNodeBare * next;// first and last for holder.
		inline void ConnectNextTo( ListNodeBare *a ) {
			Disconnect();
			next = a->next;
			prev = a;
			next->prev = this;
			prev->next = this;
		};
		inline void ConnectPrevTo( ListNodeBare *a ) {
			Disconnect();
			next = a;
			prev = a->prev;
			next->prev = this;
			prev->next = this;
		};
		inline void Disconnect() {
			if ( prev != this ) {//if we're connected
				my_assert( prev );
				my_assert( next );
				my_assert( next != this );
				next->prev = prev;
				prev->next = next;
				// short itself.
				prev = this;
				next = this;
			} else {
				// we're not connected,must be so on both ends.
				my_assert( next == this );
			};
		};
		ListNodeBare() {
			prev = this;
			next = this;
		};
		~ListNodeBare() {
			Disconnect();
		}

		ListNodeBare( const ListNodeBare &other ) {
			prev = this;
			next = this;
		}
};


/// A base class for event receivers
template<class TParamType>
class EventReceiverFuncBase: public ListNodeBare {
	public:
		virtual void OnEvent( TParamType param ) = 0;
		/// compiler bug workarounds:
};

/// use EventSender::SendEvent to send events.
template<class TParamType>
class EventSender: public ListNodeBare {
	public:
		void SendEvent( TParamType param ) {
			//std::cout<<"SendEvent called "<<std::endl;
			ListNodeBare *pt = next;
			ListNodeBare *end = static_cast<ListNodeBare *>( this );
			my_assert( pt->prev == end );
			while ( pt != end ) {
				my_assert( pt->next->prev == pt );
				( static_cast<EventReceiverFuncBase<TParamType> *>( pt ) )->OnEvent( param );
				pt = pt->next;
			}
		}
};

/// Use EventReceiverFunc<base_class, parameter_type, &base_class::method> to make event receiver adaptor
template < class TReceiverType, class TParamType, void ( TReceiverType::*TMethod )( TParamType ) >
class EventReceiverFunc: public EventReceiverFuncBase<TParamType>
{
		TReceiverType *receiver_obj;
	public:
		EventReceiverFunc() {}
		EventReceiverFunc( TReceiverType *receiver_obj_, EventSender<TParamType> *sender ):
				receiver_obj( receiver_obj_ )
		{
			ListNodeBare *a = static_cast<ListNodeBare *>( sender );
			ListNodeBare::ConnectPrevTo( a );
		}
		virtual void OnEvent( TParamType param ) {
			if ( receiver_obj ) {
				( receiver_obj->*TMethod )( param );/// TMethod is constant here at compile time, so chances are the call is inlined. It is faster than standard pointer-to-member function call.
			}
		}
};


#endif // EVENTS_H_INCLUDED

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

