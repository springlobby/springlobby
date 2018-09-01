/*
 * BitmapComboBox.h
 *
 *  Created on: 18 ����� 2016 �.
 *      Author: ������
 */

#ifndef SRC_GUI_CONTROLS_BITMAPCOMBOBOX_H_
#define SRC_GUI_CONTROLS_BITMAPCOMBOBOX_H_

#include <wx/dc.h>
#include <wx/odcombo.h>
#include <map>
namespace GUI
{
namespace Controls
{

class BitmapComboBox : public wxOwnerDrawnComboBox
{
public:
	BitmapComboBox(wxWindow* parent,
		       wxWindowID id = wxID_ANY,
		       const wxString& value = wxEmptyString,
		       const wxPoint& pos = wxDefaultPosition,
		       const wxSize& size = wxDefaultSize,
		       int n = 0,
		       const wxString choices[] = NULL,
		       long style = 0,
		       const wxValidator& validator = wxDefaultValidator,
		       const wxString& name = wxComboBoxNameStr);
	virtual ~BitmapComboBox();

	int Append(const wxString& item, const wxBitmap& bitmap = wxNullBitmap);
	virtual void Clear() override;

	virtual void OnDrawBackground(wxDC& dc, const wxRect& rect,
				      int item, int flags) const override;
	virtual void OnDrawItem(wxDC& dc, const wxRect& rect, int item, int flags) const override;
	virtual wxCoord OnMeasureItem(size_t item) const override;
	virtual wxCoord OnMeasureItemWidth(size_t item) const override;

protected:
	wxBitmap* getItemBitmap(int item) const;
	wxString getItemText(int item) const;
	void appendItemBitmap(int item, const wxBitmap& bitmap);

protected:
	const int DEFAULT_ITEM_HEIGHT = 13;
	const int IMAGE_SPACING_RIGHT = 4;
	const int IMAGE_SPACING_LEFT = 4;
	const int EXTRA_FONT_HEIGHT = 0;

protected:
	std::map<int, wxBitmap> m_bitmaps; // Images associated with items
	wxSize m_usedImgSize;		   // Size of bitmaps
	wxBitmap m_nullBitmap;

	int m_imgAreaWidth; // Width and height of area next to text field
	int m_fontHeight;
};

} /* namespace Controls */
} /* namespace GUI */

#endif /* SRC_GUI_CONTROLS_BITMAPCOMBOBOX_H_ */
