#ifndef SPRINGLOBBY_HEADERGUARD_MAINSINGLEPLAYERTAB_H
#define SPRINGLOBBY_HEADERGUARD_MAINSINGLEPLAYERTAB_H


#include <wx/panel.h>


class Ui;
class wxNotebook;
class wxImageList;
class wxBoxSizer;
class SinglePlayerTab;


class MainSinglePlayerTab : public wxPanel
{
  public:
    MainSinglePlayerTab( wxWindow* parent, Ui& ui );
    ~MainSinglePlayerTab();



  protected:

    Ui& m_ui;

    wxBoxSizer* m_main_sizer;
    wxImageList* m_imagelist;
    wxNotebook* m_tabs;

    SinglePlayerTab* m_sp_tab;

};


#endif // SPRINGLOBBY_HEADERGUARD_MAINSINGLEPLAYERTAB_H
