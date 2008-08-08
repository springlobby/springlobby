#ifndef SPRINGLOBBY_HEADERGUARD_MANAGEGROUPSPANEL_H
#define SPRINGLOBBY_HEADERGUARD_MANAGEGROUPSPANEL_H

#include <wx/scrolwin.h>

class wxStaticBoxSizer;
class wxStaticBox;
class wxStaticText;
class wxRadioButton;
class wxButton;
class wxTextCtrl;
class wxBoxSizer;
class Ui;

class ManageGroupsPanel: public wxScrolledWindow
{
     ManageGroupsPanel( wxWindow* parent );
     ~ManageGroupsPanel();
};

#endif
