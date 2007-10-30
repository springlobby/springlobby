//---------------------------------------------------------------------------
//
// Name:        Project2Frm.h
// Author:      Kosh
// Created:     29.10.2007 19:21:41
// Description: Project2Frm class declaration
//
//---------------------------------------------------------------------------

#ifndef __PROJECT2FRM_h__
#define __PROJECT2FRM_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/frame.h>
#else
	#include <wx/wxprec.h>
#endif

//Do not add custom headers between 
//Header Include Start and Header Include End.
//wxDev-C++ designer will remove them. Add custom headers after the block.
////Header Include Start
#include <wx/panel.h>
#include <wx/notebook.h>
////Header Include End

////Dialog Style Start
#undef Project2Frm_STYLE
#define Project2Frm_STYLE wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCLOSE_BOX
////Dialog Style End

class Project2Frm : public wxFrame
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		Project2Frm(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = Project2Frm_STYLE);
		virtual ~Project2Frm();
		
	private:
		//Do not add custom control declarations between
		//GUI Control Declaration Start and GUI Control Declaration End.
		//wxDev-C++ will remove them. Add custom code after the block.
		////GUI Control Declaration Start
		wxPanel *WxPanel7;
		wxPanel *WxPanel6;
		wxPanel *WxPanel5;
		wxPanel *WxPanel4;
		wxPanel *WxPanel3;
		wxPanel *WxPanel2;
		wxPanel *WxPanel1;
		wxPanel *Simple;
		wxPanel *Debug;
		wxPanel *Mouse;
		wxPanel *Audio;
		wxPanel *General;
		wxPanel *Video;
		wxPanel *Rendering;
		wxNotebook *Options;
		////GUI Control Declaration End
		
	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
			ID_WXPANEL7 = 1017,
			ID_WXPANEL6 = 1016,
			ID_WXPANEL5 = 1015,
			ID_WXPANEL4 = 1014,
			ID_WXPANEL3 = 1013,
			ID_WXPANEL2 = 1012,
			ID_WXPANEL1 = 1011,
			ID_SIMPLE = 1010,
			ID_DEBUG = 1009,
			ID_MOUSE = 1008,
			ID_AUDIO = 1007,
			ID_GENERAL = 1006,
			ID_VIDEO = 1005,
			ID_RENDERING = 1004,
			ID_OPTIONS = 1003,
			////GUI Enum Control ID End
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
		
	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
};

#endif
