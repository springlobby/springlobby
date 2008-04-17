#ifndef MAINTORRENTTAB_H
#define MAINTORRENTTAB_H

//(*Headers(MainTorrentTab)
#include <wx/frame.h>
//*)

class wxStaticText;
class wxButton;
class wxListBox;

class MainTorrentTab: public wxFrame
{
	public:

		MainTorrentTab(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~MainTorrentTab();

		//(*Declarations(MainTorrentTab)
		wxStaticText* StaticText2;
		wxButton* Button1;
		wxStaticText* StaticText1;
		wxButton* Button2;
		wxListBox* ListBox1;
		//*)

	protected:

		//(*Identifiers(MainTorrentTab)
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT1;
		static const long ID_LISTBOX1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		//*)

	private:

		//(*Handlers(MainTorrentTab)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
