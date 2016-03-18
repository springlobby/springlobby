/*
 * BitmapComboBox.cpp
 *
 *  Created on: 18 марта 2016 г.
 *      Author: Руслан
 */

#include "bitmapcombobox.h"

namespace GUI {
namespace Controls {

BitmapComboBox::BitmapComboBox(wxWindow* parent, wxWindowID id,
		const wxString& value, const wxPoint& pos, const wxSize& size, int n,
		const wxString choices[], long style, const wxValidator& validator,
		const wxString& name) : wxOwnerDrawnComboBox(parent, id, value, pos, size, n, choices, style, validator, name){
	m_fontHeight = GetCharHeight();
	m_imgAreaWidth = 0;
	m_usedImgSize = wxSize(-1, -1);
}

BitmapComboBox::~BitmapComboBox() {
	// TODO Auto-generated destructor stub
}

void BitmapComboBox::OnDrawBackground(wxDC& dc, const wxRect& rect, int item,
		int flags) const {

    if ( flags & wxODCB_PAINTING_SELECTED )
    {
        const int vSizeDec = 0;  // Vertical size reduction of selection rectangle edges

        dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));

        wxColour selCol = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        dc.SetPen(selCol);
        dc.SetBrush(selCol);
        dc.DrawRectangle(rect.x,
                         rect.y+vSizeDec,
                         rect.width,
                         rect.height-(vSizeDec*2));
    }
    else
    {
        dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

        wxColour selCol = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
        dc.SetPen(selCol);
        dc.SetBrush(selCol);
        dc.DrawRectangle(rect);
    }
}

void BitmapComboBox::OnDrawItem(wxDC& dc, const wxRect& rect, int item,
		int flags) const {

    if ( item == wxNOT_FOUND )
         return;

    const wxBitmap& bmp = *getItemBitmap(item);
    if ( bmp.IsOk() )
    {
        wxCoord w = bmp.GetWidth();
        wxCoord h = bmp.GetHeight();

        // Draw the image centered
        dc.DrawBitmap(bmp,
                      rect.x + (m_usedImgSize.x-w)/2 + IMAGE_SPACING_LEFT,
                      rect.y + (rect.height-h)/2,
                      true);
    }

    const wxString& text = getItemText(item);
    if ( !text.empty() )
        dc.DrawText(text,
                    rect.x + (bmp.IsOk() ? m_imgAreaWidth : 0) + 1,
                    rect.y + (rect.height-dc.GetCharHeight())/2);
}

wxCoord BitmapComboBox::OnMeasureItem(size_t item) const {
    if ( m_usedImgSize.y >= 0 )
    {
        int imgHeightArea = m_usedImgSize.y + 2;
        return imgHeightArea > m_fontHeight ? imgHeightArea : m_fontHeight;
    }

    return DEFAULT_ITEM_HEIGHT;
}

int BitmapComboBox::Append(const wxString& item, const wxBitmap& bitmap) {

	int itemID = wxItemContainer::Append(item);

	appendItemBitmap(itemID, bitmap);

	return itemID;
}

wxCoord BitmapComboBox::OnMeasureItemWidth(size_t item) const {
	return -1;
}

wxBitmap* BitmapComboBox::getItemBitmap(int item) const {
	auto itor = m_bitmaps.find(item);

	return itor == m_bitmaps.end() ? const_cast<wxBitmap*>(&m_nullBitmap) : const_cast<wxBitmap*>(&itor->second);
}

wxString BitmapComboBox::getItemText(int item) const {
	return GetString(item);
}

void BitmapComboBox::appendItemBitmap(int item, const wxBitmap& bitmap) {

    if ( bitmap.IsOk() )
    {
        int width = bitmap.GetWidth();
        int height = bitmap.GetHeight();

        if ( m_usedImgSize.x < 0 )
        {
            // If size not yet determined, get it from this image.
            m_usedImgSize.x = width;
            m_usedImgSize.y = height;
        }

        m_imgAreaWidth = m_usedImgSize.x + IMAGE_SPACING_LEFT + IMAGE_SPACING_RIGHT;

        m_bitmaps[item] = bitmap;
    }
}

void BitmapComboBox::Clear() {
	m_bitmaps.clear();
	wxOwnerDrawnComboBox::Clear();
}

} /* namespace Controls */
} /* namespace GUI */

