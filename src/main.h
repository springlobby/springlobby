//---------------------------------------------------------------------------
//
// Name:        Project2App.h
// Author:      Kosh
// Created:     29.10.2007 19:21:41
// Description: 
//
//---------------------------------------------------------------------------

#ifndef __PROJECT2FRMApp_h__
#define __PROJECT2FRMApp_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#else
	#include <wx/wxprec.h>
#endif

class Project2FrmApp : public wxApp
{
	public:
		bool OnInit();
		int OnExit();
};

#endif
