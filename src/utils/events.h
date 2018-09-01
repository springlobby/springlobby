/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED

#include <assert.h>
#include "utils/mixins.h"
#ifndef my_assert
#define my_assert(x) assert(x)
#endif

/// Double linked cyclic list implementation.
/// Beware! When a node is copied (duplicated), the copy is inserted after original into the list!
/// This is done to make it work when you're storing nodes in some container which invokes copy constructor on components.
class ListNodeBare
{
public:
	ListNodeBare* prev;
	ListNodeBare* next; // first and last for holder.
	inline void ConnectNextTo(ListNodeBare* a)
	{ /// connecting next to itself is equivalent to disconnect.
		Disconnect();
		next = a->next;
		prev = a;
		next->prev = this;
		prev->next = this;
	}
	inline void ConnectPrevTo(ListNodeBare* a)
	{ /// connecting prev to itself is equivalent to disconnect.
		Disconnect();
		next = a;
		prev = a->prev;
		next->prev = this;
		prev->next = this;
	}
	inline void StepNext()
	{ /// moves itself forward in the ring
		ListNodeBare* a = next;

		next->prev = prev; /// disconnect itself
		prev->next = next;

		next = a->next; /// reconnect itself after next
		prev = a;
		next->prev = this;
		prev->next = this;
	}
	inline void StepPrev()
	{ /// moves itself backward in the ring
		ListNodeBare* a = prev;

		next->prev = prev; /// disconnect itself
		prev->next = next;

		next = a; /// reconnect itself before prev
		prev = a->prev;
		next->prev = this;
		prev->next = this;
	}

	inline void SanityCheck()
	{
		my_assert(prev != 0);
		my_assert(next != 0);
		my_assert(prev->next == this);
		my_assert(next->prev == this);
	}

	inline void Disconnect()
	{
		if (prev != this) { //if we're connected
			my_assert(prev != 0);
			my_assert(next != 0);
			my_assert(next != this);
			next->prev = prev;
			prev->next = next;
			// short itself.
			prev = this;
			next = this;
		} else {
			// we're not connected,must be so on both ends.
			my_assert(next == this);
		};
	}
	ListNodeBare()
	    : prev(this)
	    , next(this)
	{
	}
	virtual ~ListNodeBare()
	{
		Disconnect();
	}

	ListNodeBare(const ListNodeBare& a)
	{
		next = a.next;
		prev = const_cast<ListNodeBare*>(&a);
		next->prev = this;
		prev->next = this;
	}

	ListNodeBare& operator=(const ListNodeBare& a)
	{
		if (this != &a) {
			next = a.next;
			prev = const_cast<ListNodeBare*>(&a);
			next->prev = this;
			prev->next = this;
		}
		return *this;
	}
};

/// A base class for event receivers
template <class TParamType>
class EventReceiverFuncBase : public ListNodeBare
{
public:
	virtual void OnEvent(TParamType /*param*/)
	{
	}
};

/// use EventSender::SendEvent to send events.
template <class TParamType>
class EventSender : public EventReceiverFuncBase<TParamType>
{
public:
	EventSender()
	    : m_enabled(true)
	{
	}

	void OnEvent(TParamType /*param */)
	{
		/// you may want to put assert(0) here, or log a message.
		/// This method is only called when you have multiple senders sharing same event list.
		/// Which happens when you unnecessarily make a copy of sender.
	}
	void SendEvent(TParamType param)
	{
		//std::cout<<"SendEvent called "<<std::endl;
		//ListNodeBare *pt=next;
		ListNodeBare* end = static_cast<ListNodeBare*>(this);
		//my_assert(pt->prev==end);
		EventReceiverFuncBase<TParamType> iterator_node;
		iterator_node.ConnectNextTo(end);
		while (m_enabled && (iterator_node.next != end)) {

			iterator_node.SanityCheck();

			(static_cast<EventReceiverFuncBase<TParamType>*>(iterator_node.next))->OnEvent(param);
			iterator_node.StepNext();
		}
	}

	void Enable(bool enable = true)
	{
		m_enabled = enable;
	}

protected:
	bool m_enabled;
};

/// Use EventReceiverFunc<base_class, parameter_type, &base_class::method> to make event receiver adaptor
template <class TReceiverType, class TParamType, void (TReceiverType::*TMethod)(TParamType)>
class EventReceiverFunc : public EventReceiverFuncBase<TParamType>, public SL::NonCopyable
{
	TReceiverType* receiver_obj;

public:
	EventReceiverFunc(TReceiverType* receiver_obj_, EventSender<TParamType>* sender = 0)
	    : receiver_obj(receiver_obj_)
	{
		if (sender) {
			ListNodeBare* a = static_cast<ListNodeBare*>(sender);
			ListNodeBare::ConnectPrevTo(a);
		}
	}

	void Connect(EventSender<TParamType>& sender)
	{
		Connect(&sender);
	}

	void Connect(EventSender<TParamType>* sender)
	{
		if (sender) {
			ListNodeBare* a = static_cast<ListNodeBare*>(sender);
			ListNodeBare::ConnectPrevTo(a);
		} else {
			ListNodeBare::Disconnect();
		}
	}

	virtual void OnEvent(TParamType param)
	{
		if (receiver_obj) {
			(receiver_obj->*TMethod)(param); /// TMethod is constant here at compile time, so chances are the call is inlined. It is faster than standard pointer-to-member function call.
		}
	}
};

/// Use EventReceiverFunc<base_class, parameter_type, &base_class::method> to make event receiver adaptor
template <class TReceiverType, class TParamType, class TAuxParamType, void (TReceiverType::*TMethod)(TParamType, TAuxParamType)>
class EventReceiverFuncAux : public EventReceiverFuncBase<TParamType>
{
	TReceiverType* receiver_obj;

public:
	TAuxParamType aux_param;
	EventReceiverFuncAux(TReceiverType* receiver_obj_, EventSender<TParamType>* sender = 0, TAuxParamType aux_param_ = TAuxParamType())
	    : receiver_obj(receiver_obj_)
	    , aux_param(aux_param_)
	{
		if (sender) {
			ListNodeBare* a = static_cast<ListNodeBare*>(sender);
			ListNodeBare::ConnectPrevTo(a);
		}
	}

	void Connect(EventSender<TParamType>& sender)
	{
		Connect(&sender);
	}

	void Connect(EventSender<TParamType>* sender)
	{
		if (sender) {
			ListNodeBare* a = static_cast<ListNodeBare*>(sender);
			ListNodeBare::ConnectPrevTo(a);
		} else {
			ListNodeBare::Disconnect();
		}
	}

	virtual void OnEvent(TParamType param)
	{
		if (receiver_obj) {
			(receiver_obj->*TMethod)(param, aux_param); /// TMethod is constant here at compile time, so chances are the call is inlined. It is faster than standard pointer-to-member function call.
		}
	}
};


#endif // EVENTS_H_INCLUDED
