#include "windowattributespickle.h"

#include <wx/toplevel.h>
#include "../settings.h"

WindowAttributesPickle::WindowAttributesPickle( const wxString& name, wxTopLevelWindow* window, const wxSize& default_size  )
	: m_name( name ),
	m_window( window )
{
	wxPoint pos = sett().GetWindowPos( m_name, wxDefaultPosition );
	wxSize size = sett().GetWindowSize( m_name, default_size );
	m_window->SetSize( pos.x , pos.y, size.GetWidth(), size.GetHeight() );
	m_window->Maximize( sett().GetWindowMaximized( m_name ) );
}

WindowAttributesPickle::~WindowAttributesPickle()
{
	sett().SetWindowSize( m_name, m_window->GetSize() );
	sett().SetWindowPos( m_name, m_window->GetPosition() );
	sett().GetWindowMaximized( m_name, m_window->IsMaximized() );
	sett().SaveSettings();
}
