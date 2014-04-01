/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef WINDOWATTRIBUTESPICKLE_H
#define WINDOWATTRIBUTESPICKLE_H

#include <wx/string.h>
#include <wx/toplevel.h>
#include "utils/mixins.h"

class wxSize;
//! automagically load/save window size and position in ctor/dtor
class WindowAttributesPickle : public SL::NonCopyable
{
public:
	WindowAttributesPickle( const wxString& name, wxTopLevelWindow* window, const wxSize& default_size );
	virtual ~WindowAttributesPickle();
protected:
	void SaveAttributes();
	void LoadAttributes();

	int    GetWindowWidth( const wxString& window );
	void   SetWindowWidth( const wxString& window, const int value );

	int    GetWindowHeight( const wxString& window );
	void   SetWindowHeight( const wxString& window, const int value );

	int    GetWindowTop( const wxString& window );
	void   SetWindowTop( const wxString& window, const int value );

	int    GetWindowLeft( const wxString& window );
	void   SetWindowLeft( const wxString& window, const int value );

	wxSize  GetWindowSize( const wxString& window, const wxSize& def );
	void    SetWindowSize( const wxString& window, const wxSize& size  );

	wxPoint GetWindowPos( const wxString& window, const wxPoint& def );
	void    SetWindowPos( const wxString& window, const wxPoint& pos );

	bool	GetWindowMaximized( const wxString& window );
	void	GetWindowMaximized( const wxString& window, bool maximized );

	const wxString m_name;
	wxTopLevelWindow* m_window;
	const wxSize m_default_size;
};

class WindowHintsPickle : public WindowAttributesPickle
{
public:
	WindowHintsPickle( const wxString& name, wxTopLevelWindow* window, const wxSize& default_size  );

protected:
	void LoadAttributes();
};

#endif // WINDOWATTRIBUTESPICKLE_H
