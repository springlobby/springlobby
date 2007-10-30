//---------------------------------------------------------------------------
//
// Name:        Project2App.cpp
// Author:      Kosh
// Created:     29.10.2007 19:21:41
// Description: 
//
//---------------------------------------------------------------------------

#include "main.h"
#include "frame.h"

IMPLEMENT_APP(Project2FrmApp)

bool Project2FrmApp::OnInit()
{
    Project2Frm* frame = new Project2Frm(NULL);
    SetTopWindow(frame);
    frame->Show();
    return true;
}
 
int Project2FrmApp::OnExit()
{
	return 0;
}
