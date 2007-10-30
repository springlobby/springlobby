#include <wx/wx.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <unistd.h>

#include "ConfigHandler.h"
#include "Defs.hpp"
#include "tab_rendering.h"
#include "tab_abstract.h"
#include "tab_audio.h"
#include "tab_debug.h"
#include "tab_general.h"
#include "tab_mouse.h"

#include "tab_simple.h"
#include "tab_video.h"

#define TAB_SIZE wxSize(700,500)
