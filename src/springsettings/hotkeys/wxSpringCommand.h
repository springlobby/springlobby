/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef __SPRING_COMMAND_h__
#define __SPRING_COMMAND_h__

#include "springsettings/wxkeybinder/keybinder.h"

class wxSpringCommand : public wxCmd
{
public:
	wxSpringCommand( const wxString& name, const wxString& description, int id );
	~wxSpringCommand(void);

	virtual wxCmd*	Clone() const;
	virtual int		GetType() const;

	static unsigned short	type_id;

private:
	virtual void Exec (wxObject *obj, wxEvtHandler *client);
};

#endif
