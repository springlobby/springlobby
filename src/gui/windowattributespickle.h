#ifndef WINDOWATTRIBUTESPICKLE_H
#define WINDOWATTRIBUTESPICKLE_H

#include <wx/string.h>
#include <wx/toplevel.h>
class wxSize;
//! automagically load/save window size and position in ctor/dtor
class WindowAttributesPickle
{
public:
	WindowAttributesPickle( const wxString& name, wxTopLevelWindow* window, const wxSize& default_size );
	~WindowAttributesPickle();
protected:
	void SaveAttributes();
	void LoadAttributes();
private:
	const wxString m_name;
	wxTopLevelWindow* m_window;
	const wxSize m_default_size;
};

#endif // WINDOWATTRIBUTESPICKLE_H
