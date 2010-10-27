#include "wxSpringCommand.h"

unsigned short wxSpringCommand::type_id = 0x2345u;

wxSpringCommand::wxSpringCommand( const wxString& name, const wxString& descr, int id ) : 
						wxCmd( id /*wxID_INVALID*/, name, descr )
{
}

wxSpringCommand::~wxSpringCommand(void)
{
}

wxCmd* wxSpringCommand::Clone() const
{
	wxCmd* pNewCmd = new wxSpringCommand( this->GetName(), this->GetDescription(), this->GetId() );
	pNewCmd->DeepCopy(this);
	return pNewCmd;
}

int	wxSpringCommand::GetType() const
{
	return this->type_id;
}

void wxSpringCommand::Exec(wxObject*, wxEvtHandler*)
{
}
