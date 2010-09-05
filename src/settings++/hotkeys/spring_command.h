#ifndef __SPRING_COMMAND_h__
#define __SPRING_COMMAND_h__

#include "wx\keybinder.h"

class spring_command : public wxCmd
{
public:
	spring_command( const wxString& name, const wxString& description, int id );
	~spring_command(void);

	virtual wxCmd*	Clone() const;
	virtual int		GetType() const;

	static unsigned short	type_id;

protected:
	virtual void Exec (wxObject *obj, wxEvtHandler *client);
};

#endif