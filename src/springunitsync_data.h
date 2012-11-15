/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/


#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_DATA_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_DATA_H

#include <vector>
#include <map>
#include <wx/string.h>

struct UnitSyncMod
{
  UnitSyncMod() : name(_T("")),hash(_T("")) { }
  wxString name;
  wxString hash;
};

struct StartPos
{
  StartPos () : x(0), y(0) {}
  int x;
  int y;
};

struct MapInfo
{
  MapInfo () : description(_T("")),
    tidalStrength(0),
    gravity(0),
    maxMetal(0.0f),
    extractorRadius(0),
    minWind(0),
    maxWind(0),
    width(0),
    height(0),
    author(_T(""))
 {}
  wxString description;
  int tidalStrength;
  int gravity;
  float maxMetal;
  int extractorRadius;
  int minWind;
  int maxWind;

  int width;
  int height;
  std::vector<StartPos> positions;

  wxString author;
};

struct UnitSyncMap
{
  UnitSyncMap() : name(_T("")),hash(_T("")) { }
  wxString name;
  wxString hash;
  MapInfo info;
};

typedef std::map<wxString,wxString> LocalArchivesVector;

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_DATA_H
