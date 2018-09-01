/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/aboutdlg.h>
class slAboutBox : public wxAboutDialogInfo
{
public:
	slAboutBox();
	~slAboutBox();
	void Show();
	void openContactPage();
	void openNewTicket();
	void showDocs();
	void showChangelog();

private:
	wxAboutDialogInfo* info;
};


slAboutBox& aboutbox();
