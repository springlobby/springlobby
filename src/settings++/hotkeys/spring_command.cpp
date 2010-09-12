#include "spring_command.h"

unsigned short spring_command::type_id = 0x2345u;

spring_command::spring_command( const wxString& name, const wxString& descr, int id ) : 
						wxCmd( id /*wxID_INVALID*/, name, descr )
{
}

spring_command::~spring_command(void)
{
}

wxCmd* spring_command::Clone() const
{
	wxCmd* pNewCmd = new spring_command( this->GetName(), this->GetDescription(), this->GetId() );
	pNewCmd->DeepCopy(this);
	return pNewCmd;
}

int	spring_command::GetType() const
{
	return this->type_id;
}

void spring_command::Exec(wxObject *obj, wxEvtHandler *client)
{
}
