#ifndef SPRINGLOBBY_HEADERGUARD_ISINK_H
#define SPRINGLOBBY_HEADERGUARD_ISINK_H

#include "globalevents.h"
#include "crtpbase.h"

/** \brief mixin classes that provide automatic sink creation and setup

    originally created to work around name resolution problems when using sinks in templated classes
**/
template <class Derived, class EventDataType = GlobalEvents::GlobalEventData >
class UnitsyncReloadedSink : public CRTPbase<Derived> {
	protected:
		typedef UnitsyncReloadedSink<Derived,EventDataType>
			BaseType;
        using CRTPbase<Derived>::asImp;

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

template <class Derived, class EventDataType = GlobalEvents::GlobalEventData >
class SpringTerminatedSink : public CRTPbase<Derived> {
	protected:
		typedef SpringTerminatedSink<Derived,EventDataType>
			BaseType;
        using CRTPbase<Derived>::asImp;

	public:
		void OnSpringTerminated( EventDataType data ) { asImp().OnSpringTerminated( data ); }

    protected:
        typedef EventReceiverFunc<SpringTerminatedSink, EventDataType, &SpringTerminatedSink::OnSpringTerminated>
            EventReceiverFunction;
        EventReceiverFunction m_SpringTerminatedSink;

    public:
		SpringTerminatedSink ()
			: m_SpringTerminatedSink( this, &GetGlobalEventSender( GlobalEvents::OnSpringTerminated ) )
		{}
};

template <class Derived, class EventDataType = GlobalEvents::GlobalEventData >
class OnQuitSink : public CRTPbase<Derived> {
	protected:
		typedef OnQuitSink<Derived,EventDataType>
			BaseType;
        using CRTPbase<Derived>::asImp;

	public:
		void OnQuit( EventDataType data ) { asImp().OnQuit( data ); }

    protected:
        typedef EventReceiverFunc<OnQuitSink, EventDataType, &OnQuitSink::OnQuit>
            EventReceiverFunction;
        EventReceiverFunction m_OnQuitSink;

    public:
		OnQuitSink ()
			: m_OnQuitSink( this, &GetGlobalEventSender( GlobalEvents::OnQuit) )
		{}
};

template <class Derived, class EventDataType = GlobalEvents::GlobalEventData >
class BattleSyncReloadSink : public CRTPbase<Derived> {
	protected:
		typedef BattleSyncReloadSink<Derived,EventDataType>
			BaseType;
		using CRTPbase<Derived>::asImp;

	public:
		void OnBattleSyncReload( EventDataType data ) { asImp().OnBattleSyncReload( data ); }

	protected:
		typedef EventReceiverFunc<BattleSyncReloadSink, EventDataType, &BattleSyncReloadSink::OnBattleSyncReload>
			EventReceiverFunction;
		EventReceiverFunction m_BattleSyncReloadSink;

	public:
		BattleSyncReloadSink ()
			: m_BattleSyncReloadSink( this, &GetGlobalEventSender( GlobalEvents::BattleSyncReload) )
		{}
};

template <class Derived >
class OnLoginSink : public CRTPbase<Derived> {
	protected:
		typedef GlobalEvents::GlobalEventData
			EventDataType;
		typedef OnLoginSink<Derived>
			BaseType;
		using CRTPbase<Derived>::asImp;

	public:
		void OnLogin( EventDataType data ) { asImp().OnLogin( data ); }

	protected:
		typedef EventReceiverFunc<OnLoginSink, EventDataType, &OnLoginSink::OnLogin>
			EventReceiverFunction;
		EventReceiverFunction m_OnLoginSink;

	public:
		OnLoginSink()
			: m_OnLoginSink( this, &GetGlobalEventSender( GlobalEvents::OnLogin) )
		{}
};
#endif // SPRINGLOBBY_HEADERGUARD_ISINK_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

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
