/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include "uiutils.h"

wxString RefineMapname( wxString mapname )
{
  mapname = mapname.SubString(0, mapname.Find( '.', true ) - 1 );
  mapname.Replace(_T("_"), _T(" ") );
  mapname.Replace(_T("-"), _T(" ") );
  return mapname;
}


wxString RefineModname( wxString modname )
{
  modname.Replace(_T("Absolute Annihilation"), _T("AA") );
  modname.Replace(_T("Complete Annihilation"), _T("CA") );
  modname.Replace(_T("Balanced Annihilation"), _T("BA") );
  modname.Replace(_T("Expand and Exterminate"), _T("EE") );
  modname.Replace(_T("War Evolution"), _T("WarEv") );
  modname.Replace(_T("TinyComm"), _T("TC") );
  modname.Replace(_T("BETA"), _T("b") );
  modname.Replace(_T("Public Alpha"), _T("pa") );
  modname.Replace(_T("Public Beta"), _T("pb") );
  modname.Replace(_T("Public"), _T("p") );
  modname.Replace(_T("Alpha"), _T("a") );
  modname.Replace(_T("Beta"), _T("b") );
  return modname;
}
