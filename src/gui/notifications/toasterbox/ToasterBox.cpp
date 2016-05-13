/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//---------------------------------------------------------------------------
//
// Title:       ToasterBox
// Author:      Aj Bommarito
// Created:     Wednesday, August 18, 2004 16:33:49
//
// Description: Creates MSN "Toaster" style popups.
// Version 1.0rc
//---------------------------------------------------------------------------

#include "ToasterBox.h"

#include <wx/dialog.h>
#include <wx/utils.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/timer.h>
#include <wx/log.h>

#include "utils/uievents.h"

ToasterBox::ToasterBox(wxWindow* _parent)
    : parent(_parent)
    , sleepTime(10)
    , pauseTime(1700)
    , popupText(_T("default"))
    , popupTop(wxPoint(0, 0))
    , popupPosition(wxPoint(100, 100))
    , popupSize(wxSize(150, 170))
    , colFg(*wxWHITE)
    , colBg(*wxBLACK)
    , bitmapFile(wxEmptyString)
    , m_bitmap(wxNullBitmap)
    , m_stack_direction(StackUp)
{
	//where to keep track of all the toasterboxwindows
	winList = new ToasterBoxWindowList();
	winList->DeleteContents(true);

	//set the bottom right corner of the dialog
	//at the bottom right of the screen
	bottomRight = wxPoint(wxGetDisplaySize().GetWidth(),
			      wxGetDisplaySize().GetHeight());
}

ToasterBox::~ToasterBox()
{

	delete winList;
	winList = 0;
}

void ToasterBox::SetPopupPosition(int x, int y)
{
	popupPosition = wxPoint(x, y);
}

void ToasterBox::SetPopupPosition(int pos)
{
	bottomRight = wxPoint(wxGetDisplaySize().GetWidth(),
			      wxGetDisplaySize().GetHeight());

	//top left
	if (pos == 0)
		popupPosition = wxPoint(0, 0);
	//top right
	if (pos == 1)
		popupPosition = wxPoint(wxGetDisplaySize().GetWidth() - popupSize.GetWidth(), 0);
	//bottom left
	if (pos == 2)
		popupPosition = wxPoint(0,
					wxGetDisplaySize().GetHeight() - popupSize.GetHeight());
	//bottom right
	if (pos == 3)
		popupPosition = wxPoint(bottomRight.x - popupSize.GetWidth(),
					bottomRight.y - popupSize.GetHeight());

	bottomRight = wxPoint(popupPosition.x + popupSize.GetWidth(),
			      popupPosition.y + popupSize.GetHeight());
}

void ToasterBox::SetPopupBackgroundColor(int r, int g, int b)
{
	colBg = wxColour(r, g, b);
}

void ToasterBox::SetPopupTextColor(int r, int g, int b)
{
	colFg = wxColour(r, g, b);
}

/**
 * Draw notification window
 */
void ToasterBox::Play()
{
	//create new window
	ToasterBoxWindow* tb = new ToasterBoxWindow(parent, this);
	//If it is GamePromotion make window wider and show it a bit longer
	if (notificationType == UiEvents::EventType::GamePromoted) {
		const int widthCorrection = 100;
		const int pauseCorrection = 1000; //Add one second above standard pause
		tb->SetPopupSize(popupSize.GetWidth() + widthCorrection, popupSize.GetHeight());
		tb->SetPopupPosition(popupPosition.x - widthCorrection, popupPosition.y);
		tb->SetPopupPauseTime(pauseTime + pauseCorrection);
		tb->SetPopupHeaderText("GAME PROMOTED!");
	} else {
		tb->SetPopupSize(popupSize.GetWidth(), popupSize.GetHeight());
		tb->SetPopupPosition(popupPosition.x, popupPosition.y);
		tb->SetPopupPauseTime(pauseTime);
	}
	tb->SetPopupScrollSpeed(sleepTime);
	tb->SetPopupTextColor(colFg.Red(), colFg.Green(), colFg.Blue());
	tb->SetPopupBackgroundColor(colBg.Red(), colBg.Green(), colBg.Blue());
	if (m_bitmap.IsOk()) {
		tb->SetPopupBitmap(m_bitmap);
	} else if (!bitmapFile.IsEmpty()) {
		tb->SetPopupBitmap(bitmapFile);
	} else {
		//No image for ToasterBox???
//		wxASSERT(false);
	}
	tb->SetPopupText(popupText, true);

	//clean up the list
	CleanList();

	//check to see if there is already a window displayed
	//by looking at the linked list
	if (!winList->IsEmpty()) {
		//there ARE other windows displayed already
		//reclac where it should display
		MoveAbove(tb);
	}

	//shift new window on to the list
	winList->Append(tb);

	//play new window
	if (!tb->Play()) {
		//if we didn't show the window properly, remove it from the list
		winList->DeleteNode(winList->Item(winList->GetCount() - 1));
		//delete the object too.
		delete tb;
		return;
	}
}

bool ToasterBox::DoesTextFit()
{
	return false;
}

void ToasterBox::MoveAbove(ToasterBoxWindow* tb)
{
	//recalc where to place this popup

	if (m_stack_direction == StackUp)
		tb->SetPopupPosition(popupPosition.x,
				     popupPosition.y - (popupSize.GetHeight() * winList->GetCount()));
	else
		tb->SetPopupPosition(popupPosition.x,
				     popupPosition.y + (popupSize.GetHeight() * winList->GetCount()));
}

void ToasterBox::SetStackDirection(StackDirection dir)
{
	m_stack_direction = dir;
}

void ToasterBox::Notify()
{
	//if the list is empty, skip this
	if (winList->IsEmpty())
		return;

	wxLogDebug(_T("%s"), _T("clean&shrink"));
	//clean the window list
	CleanList();

	//figure out how many blanks we have
	ToasterBoxWindowListNode node = winList->GetFirst();
	if (!node)
		return;

	//( (our base position)-(position of this node) ) / (default height of the windows)+1
	//long blanks = ((popupPosition.y -
	//  node->GetData()->GetPosition().y) / popupSize.GetHeight()) +1;

	//move windows to fill in blank space
	for (int i = node->GetData()->GetPosition().y; i < popupPosition.y; i += 4) {
		if (i > popupPosition.y)
			i = popupPosition.y;
		//loop through all the windows
		for (unsigned int j = 0; j < winList->GetCount(); j++) {
			long ourNewHeight = i - (j * popupSize.GetHeight() - 8);
			ToasterBoxWindowListNode tmpNode = winList->Item(j);
			ToasterBoxWindow* tmpTb = tmpNode->GetData();
			//reset where the object THINKS its supposed to be
			tmpTb->SetPopupPosition(popupPosition.x, ourNewHeight);
			//actually move it
			tmpTb->SetSize(popupPosition.x, ourNewHeight,
				       tmpTb->GetSize().GetWidth(),
				       tmpTb->GetSize().GetHeight());

			//tmpNode = 0;
			tmpTb = 0;
		}

		wxMilliSleep(sleepTime);
		//DrawText();
		//Update();
	}
	StartAll();
	//node = 0;
}

void ToasterBox::CleanList()
{
	//wxLogDebug("Items on list bc %i", winList->GetCount());
	ToasterBoxWindowListNode node = winList->GetFirst();
	while (node) {
		ToasterBoxWindow* tbTemp = node->GetData();
		if (!tbTemp->IsShown()) {
			winList->DeleteNode(node);


			node = winList->GetFirst();
		} else
			node = node->GetNext();

		tbTemp = 0;
	}
	//node = 0;
	//wxLogDebug("Items on list ac %i", winList->GetCount());
}

void ToasterBox::StartAll(bool /*start*/)
{
	ToasterBoxWindowListNode node = winList->GetLast();
	while (node) {
		ToasterBoxWindow* tbTemp = node->GetData();
		tbTemp->PrintInfo();
		node = node->GetPrevious();
		tbTemp = 0;
	}
	//node = 0;
}
