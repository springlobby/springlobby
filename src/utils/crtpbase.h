#ifndef SPRINGLOBBY_HEADERGUARD_CRTPBASE_H
#define SPRINGLOBBY_HEADERGUARD_CRTPBASE_H

template <class Derived>
class CRTPbase {
	public:
		virtual ~CRTPbase(){}
	protected:
		Derived& asImp () { return static_cast<Derived&>(*this); }
		const Derived& asImp () const { return static_cast<const Derived&>(*this); }
};

#endif // SPRINGLOBBY_HEADERGUARD_CRTPBASE_H
