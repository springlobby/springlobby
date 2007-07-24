//
// Class: BattleMapTab
//

#include <wx/splitter.h>
#include <wx/intl.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/sizer.h>

#include "battlemaptab.h"
#include "ui.h"
#include "iunitsync.h"
#include "user.h"
#include "battle.h"
#include "utils.h"
#include "chatpanel.h"
#include "mapctrl.h"
#include "uiutils.h"

BEGIN_EVENT_TABLE(BattleMapTab, wxPanel)

END_EVENT_TABLE()


BattleMapTab::BattleMapTab( wxWindow* parent, Ui& ui, Battle& battle ) : wxPanel( parent, -1 ),m_ui(ui), m_battle(battle)
{

}


BattleMapTab::~BattleMapTab()
{

}
