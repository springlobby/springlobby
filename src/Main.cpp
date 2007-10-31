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

IMPLEMENT_APP(SettingsPlusPlus)

bool SettingsPlusPlus::OnInit()
{
    settings_frame* frame = new settings_frame(NULL);
    SetTopWindow(frame);
    frame->Show();
    return true;
}
 
int SettingsPlusPlus::OnExit()
{
	return 0;
}
