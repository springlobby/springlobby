#include "windowattributespickle.h"

#include "../settings.h"

WindowAttributesPickle::WindowAttributesPickle( const wxString& name, wxTopLevelWindow* window, const wxSize& default_size  )
	: m_name( name ),
	m_window( window ),
	m_default_size( default_size )
{
	LoadAttributes();
}

void WindowAttributesPickle::LoadAttributes()
{
	wxPoint pos = sett().GetWindowPos( m_name, wxDefaultPosition );
	wxSize size = sett().GetWindowSize( m_name, m_default_size );
	m_window->SetSize( pos.x , pos.y, size.GetWidth(), size.GetHeight() );
	m_window->Maximize( sett().GetWindowMaximized( m_name ) );
}

WindowAttributesPickle::~WindowAttributesPickle()
{
	SaveAttributes();
}

void WindowAttributesPickle::SaveAttributes()
{
	sett().SetWindowSize( m_name, m_window->GetSize() );
	sett().SetWindowPos( m_name, m_window->GetPosition() );
	sett().GetWindowMaximized( m_name, m_window->IsMaximized() );
	sett().SaveSettings();
}
