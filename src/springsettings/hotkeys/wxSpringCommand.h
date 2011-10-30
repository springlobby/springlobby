#ifndef __SPRING_COMMAND_h__
#define __SPRING_COMMAND_h__

#include "../../wxkeybinder/keybinder.h"

class wxSpringCommand : public wxCmd
{
public:
	wxSpringCommand( const wxString& name, const wxString& description, int id );
	~wxSpringCommand(void);

	virtual wxCmd*	Clone() const;
	virtual int		GetType() const;

	static unsigned short	type_id;

protected:
	virtual void Exec (wxObject *obj, wxEvtHandler *client);
};

#endif
