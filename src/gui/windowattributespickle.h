#ifndef WINDOWATTRIBUTESPICKLE_H
#define WINDOWATTRIBUTESPICKLE_H

#include <wx/string.h>
class wxTopLevelWindow;
class wxSize;
//! automagically load/save window size and position in ctor/dtor
class WindowAttributesPickle
{
public:
	WindowAttributesPickle( const wxString& name, wxTopLevelWindow* window, const wxSize& default_size );
	~WindowAttributesPickle();
private:
	const wxString m_name;
	wxTopLevelWindow* m_window;
};

#endif // WINDOWATTRIBUTESPICKLE_H
