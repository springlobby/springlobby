/**
    This file is part of Settings++,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (Ren� Milk)
    visit http://spring.clan-sy.com/phpbb/viewtopic.php?t=12104
    for more info/help

    Settings++ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Settings++ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Settings++.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <unistd.h>

#include "configHandler.h"
#include "Defs.hpp"
#include "tab_render_detail.h"
#include "tab_quality_video.h"

#include "tab_abstract.h"
#include "tab_audio.h"
#include "tab_debug.h"

#include "tab_mouse.h"

#include "tab_simple.h"
//#include "tab_video.h"

#define TAB_SIZE wxSize(700,500)
