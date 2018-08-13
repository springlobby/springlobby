/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "windowattributespickle.h"

#include <lslutils/misc.h>
#include <wx/settings.h>

#include "utils/slconfig.h"

const unsigned int DEFSETT_MW_WIDTH = 880;
const unsigned int DEFSETT_MW_HEIGHT = 600;
const unsigned int DEFSETT_MW_TOP = 50;
const unsigned int DEFSETT_MW_LEFT = 50;

WindowAttributesPickle::WindowAttributesPickle(const wxString& name, wxTopLevelWindow* window, const wxSize& default_size)
    : m_default_size(default_size)
    , m_name(name)
    , m_window(window)
{
	LoadAttributes();
}

void WindowAttributesPickle::LoadAttributes()
{
	wxPoint pos = GetWindowPos(m_name, wxDefaultPosition);
	wxSize size = GetWindowSize(m_name, m_default_size);
	m_window->SetSize(pos.x, pos.y, size.GetWidth(), size.GetHeight());
#ifndef __WXMAC__
	m_window->Maximize(GetWindowMaximized(m_name));
#endif
}

WindowAttributesPickle::~WindowAttributesPickle()
{
	SaveAttributes();
}

void WindowAttributesPickle::SaveAttributes()
{
	SetWindowSize(m_name, m_window->GetSize());
	SetWindowPos(m_name, m_window->GetPosition());
#ifndef __WXMAC__
	GetWindowMaximized(m_name, m_window->IsMaximized());
#endif
}

WindowHintsPickle::WindowHintsPickle(const wxString& name, wxTopLevelWindow* window, const wxSize& default_size)
    : WindowAttributesPickle(name, window, default_size)
{
	// nop
}

void WindowHintsPickle::LoadAttributes()
{
	WindowAttributesPickle::LoadAttributes();
	wxSize size = GetWindowSize(m_name, m_default_size);
	m_window->SetSizeHints(size);
}


// methods to interact with slconfig
wxPoint WindowAttributesPickle::GetWindowPos(const wxString& window, const wxPoint& def)
{
	wxPoint ret = def;
	ret.x = cfg().Read(_T( "/GUI/" ) + window + _T( "/left" ), ret.x);
	ret.y = cfg().Read(_T( "/GUI/" ) + window + _T( "/top" ), ret.y);
	return ret;
}

void WindowAttributesPickle::SetWindowPos(const wxString& window, const wxPoint& pos)
{
	SetWindowLeft(window, pos.x);
	SetWindowTop(window, pos.y);
}

//some code duplication necessary to be able to simply use wx defaults
wxSize WindowAttributesPickle::GetWindowSize(const wxString& window, const wxSize& def)
{
	wxSize ret = def;
	ret.SetHeight(cfg().Read(_T( "/GUI/" ) + window + _T( "/height" ), ret.GetHeight()));
	ret.SetWidth(cfg().Read(_T( "/GUI/" ) + window + _T( "/width" ), ret.GetWidth()));
	return ret;
}

void WindowAttributesPickle::SetWindowSize(const wxString& window, const wxSize& size)
{
	SetWindowWidth(window, size.GetWidth());
	SetWindowHeight(window, size.GetHeight());
}

bool WindowAttributesPickle::GetWindowMaximized(const wxString& window)
{
	return cfg().Read(_T( "/GUI/" ) + window + _T( "/maximized" ), 0l);
}

void WindowAttributesPickle::GetWindowMaximized(const wxString& window, bool maximized)
{
	cfg().Write(_T( "/GUI/" ) + window + _T( "/maximized" ), maximized);
}

/************* SPRINGLOBBY WINDOW POS/SIZE   ******************/
//! @brief Get width of MainWindow.
int WindowAttributesPickle::GetWindowWidth(const wxString& window)
{
	return cfg().Read(_T( "/GUI/" ) + window + _T( "/width" ), DEFSETT_MW_WIDTH);
}


//! @brief Set width position of MainWindow
void WindowAttributesPickle::SetWindowWidth(const wxString& window, const int value)
{
	cfg().Write(_T( "/GUI/" ) + window + _T( "/width" ),
		    LSL::Util::Clamp(value,
				     wxSystemSettings::GetMetric(wxSYS_WINDOWMIN_X),
				     wxSystemSettings::GetMetric(wxSYS_SCREEN_X)));
}


//! @brief Get height of MainWindow.
int WindowAttributesPickle::GetWindowHeight(const wxString& window)
{
	return cfg().Read(_T( "/GUI/" ) + window + _T( "/height" ), DEFSETT_MW_HEIGHT);
}


//! @brief Set height position of MainWindow
void WindowAttributesPickle::SetWindowHeight(const wxString& window, const int value)
{
	cfg().Write(_T( "/GUI/" ) + window + _T( "/height" ),
		    LSL::Util::Clamp(value,
				     wxSystemSettings::GetMetric(wxSYS_WINDOWMIN_Y),
				     wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)));
}


//! @brief Get top position of MainWindow.
int WindowAttributesPickle::GetWindowTop(const wxString& window)
{
	return cfg().Read(_T( "/GUI/" ) + window + _T( "/top" ), DEFSETT_MW_TOP);
}


//! @brief Set top position of MainWindow
void WindowAttributesPickle::SetWindowTop(const wxString& window, const int value)
{
	cfg().Write(_T( "/GUI/" ) + window + _T( "/top" ),
		    LSL::Util::Clamp(value,
				     0,
				     wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) - 20));
}


//! @brief Get left position of MainWindow.
int WindowAttributesPickle::GetWindowLeft(const wxString& window)
{
	return cfg().Read(_T( "/GUI/" ) + window + _T( "/left" ), DEFSETT_MW_LEFT);
}

//! @brief Set left position of MainWindow
void WindowAttributesPickle::SetWindowLeft(const wxString& window, const int value)
{
	cfg().Write(_T( "/GUI/" ) + window + _T( "/left" ),
		    LSL::Util::Clamp(value,
				     0,
				     wxSystemSettings::GetMetric(wxSYS_SCREEN_X) - 20));
}
