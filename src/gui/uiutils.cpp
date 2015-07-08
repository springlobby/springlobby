/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

/// \file uiutils.cpp

#include <wx/colour.h>
#include <wx/image.h>
#include <wx/mstream.h>
#include <wx/bitmap.h>
#include <wx/log.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/clipbrd.h>
#include <wx/cmndata.h>
#include <wx/colordlg.h>
#include <wx/dataobj.h>

#include <cmath>
#include <clocale>

#include <lslutils/misc.h>

#include "uiutils.h"
#include "utils/conversion.h"
#include "gui/customdialogs.h"
#include "settings.h"
#include "log.h"
#include "images/colourbox.xpm"

bool AreColoursSimilar(const LSL::lslColor& col1, const LSL::lslColor& col2, int mindiff)
{
	int r, g, b;
	r = col1.Red() - col2.Red();
	g = col1.Green() - col2.Green();
	b = col1.Blue() - col2.Blue();
	r = r > 0 ? r : -r;
	g = g > 0 ? g : -g;
	b = b > 0 ? b : -b;
	int difference = std::min(r, g);
	difference = std::min(difference, b);
	return difference < mindiff;
}


void ColourDelta(int& r, int& g, int& b, const int& delta)
{
	int tmpdelta;
	if (delta > 0) {
		r += delta;
		tmpdelta = delta;
		if (r > 255) {
			tmpdelta += r - 255;
			r = 255;
		}
		g += tmpdelta;
		tmpdelta = delta;
		if (g > 255) {
			tmpdelta += g - 255;
			g = 255;
		}
		b += tmpdelta;
		if (b > 255)
			b = 255;
	} else {
		r += delta;
		tmpdelta = -delta;
		if (r < 0) {
			tmpdelta -= r;
			r = 0;
		}
		g -= tmpdelta;
		tmpdelta = -delta;
		if (g < 0) {
			tmpdelta -= g;
			g = 0;
		}
		b -= tmpdelta;
		if (b < 0)
			b = 0;
	}
}

wxColour ColourDelta(const wxColour& colour, const int& delta)
{
	int r = colour.Red();
	int g = colour.Green();
	int b = colour.Blue();
	ColourDelta(r, g, b, delta);
	return wxColour(r, g, b);
}


/**
 @brief Blends two images based on alpha channel present in foreground image.
 @param foreground Foreground image, must have an alpha channel
 @param background Background image, may have an alpha channel
 @param blend_alpha Whether the returned image will have an alpha channel.
 @return A copy of the background image with the foreground image blended on
 top of it. The returned image will have an alpha channel iff the background
 image has an alpha channel. In that case the alpha channel is blended
 identical to the red/green/blue channels.
*/
wxImage BlendImage(const wxImage& foreground, const wxImage& background, bool blend_alpha)
{
	if ((foreground.GetWidth() != background.GetWidth()) || (background.GetHeight() != foreground.GetHeight())) {
		slLogDebugFunc("size mismatch while blending");
		return background;
	}

	bool zhu = blend_alpha && background.HasAlpha();
	if (foreground.HasAlpha()) {
		wxImage ret(background.GetWidth(), foreground.GetHeight());
		const unsigned char* background_data = background.GetData();
		const unsigned char* foreground_data = foreground.GetData();
		const unsigned char* background_alpha = NULL;
		const unsigned char* foreground_alpha = foreground.GetAlpha();
		unsigned char* result_data = ret.GetData();
		unsigned char* result_alpha = NULL;
		unsigned int pixel_count = background.GetWidth() * background.GetHeight();

		if (zhu) {
			background_alpha = background.GetAlpha();
			ret.InitAlpha();
			result_alpha = ret.GetAlpha();
		}

		for (unsigned int i = 0, i_a = 0; i < pixel_count * 3; i += 3, i_a++) {
			unsigned char fore_alpha = foreground_alpha[i_a];
			float back_blend_fac = (255 - fore_alpha) / 255.0;
			float fore_blend_fac = fore_alpha / 255.0;

			result_data[i] = foreground_data[i] * fore_blend_fac + background_data[i] * back_blend_fac;
			result_data[i + 1] = foreground_data[i + 1] * fore_blend_fac + background_data[i + 1] * back_blend_fac;
			result_data[i + 2] = foreground_data[i + 2] * fore_blend_fac + background_data[i + 2] * back_blend_fac;

			if (zhu) {
				unsigned char back_alpha = background_alpha[i_a];
				result_alpha[i_a] = fore_alpha * fore_blend_fac + back_alpha * back_blend_fac;
			}
		}
		return ret;
	}
	slLogDebugFunc("cannot blend without alpha");
	return background;
}

wxIcon charArr2wxIcon(const unsigned char* arg, int size)
{
	wxIcon ic;
	ic.CopyFromBitmap(charArr2wxBitmap(arg, size));
	return ic;
}

wxBitmap charArr2wxBitmap(const unsigned char* arg, int size)
{
	return wxBitmap(charArr2wxImage(arg, size));
}

//wxBitmap charArr2wxBitmap(const unsigned char * arg, int size)
//{
//    return wxBitmap( charArr2wxImage( arg, size) );
//}

wxImage charArr2wxImage(const unsigned char* arg, int size)
{
	wxMemoryInputStream istream(arg, size);
	wxImage img = wxImage(istream, wxBITMAP_TYPE_PNG);
	if (img.HasAlpha() == false) {
		img.InitAlpha();
	}
	return img;
}

wxBitmap charArr2wxBitmapWithBlending(const unsigned char* dest, int dest_size, const unsigned char* text, int text_size)
{
	wxImage dest_img(charArr2wxImage(dest, dest_size));
	wxImage text_img(charArr2wxImage(text, text_size));
	wxImage ret = BlendImage(text_img, dest_img);

	return wxBitmap(ret);
}

wxBitmap BlendBitmaps(const wxBitmap& background, const wxBitmap& overlay, const int /*dim*/)
{
	wxImage back = background.ConvertToImage();
	wxImage front = overlay.ConvertToImage();
	wxImage ret = BlendImage(front, back);
	return wxBitmap(ret);
}


namespace
{
//! Helper class for BorderInvariantResizeImage
struct Resizer
{
	// Author: Tobi Vollebregt

	Resizer(wxImage& result, const wxImage& image, bool alpha)
	    : width(result.GetWidth())
	    , height(result.GetHeight())
	    , imwidth(image.GetWidth())
	    , imheight(image.GetHeight())
	    , half_min_width((std::min(width, imwidth) + 1) / 2)    // round up to cover middle pixel
	    , half_min_height((std::min(height, imheight) + 1) / 2) // if new width/height is uneven.
	    , bytes_per_pixel(alpha ? 1 : 3)
	    , image_data(alpha ? image.GetAlpha() : image.GetData())
	    , result_data(alpha ? result.GetAlpha() : result.GetData())
	{
	}

	void CopyRow(int result_y, int image_y)
	{
		unsigned char* result_row = result_data + bytes_per_pixel * result_y * width;
		const unsigned char* image_row = image_data + bytes_per_pixel * image_y * imwidth;
		const int bytes = bytes_per_pixel * half_min_width;

		memcpy(result_row, image_row, bytes);
		memcpy(result_row + bytes_per_pixel * width - bytes, image_row + bytes_per_pixel * imwidth - bytes, bytes);

		if (width > imwidth) {
			unsigned char* result_pixel = result_row + bytes;
			const unsigned char* image_pixel = image_row + bytes;

			for (int x = half_min_width; x < width - half_min_width; ++x, result_pixel += bytes_per_pixel) {
				memcpy(result_pixel, image_pixel, bytes_per_pixel);
			}
		}
	}

	void CopyTopAndBottomRows()
	{
		for (int y = 0; y < half_min_height; ++y) {
			CopyRow(y, y);
			CopyRow(height - 1 - y, imheight - 1 - y);
		}
	}

	void CopyCenterRows()
	{
		for (int y = half_min_height; y < height - half_min_height; ++y) {
			CopyRow(y, half_min_height - 1);
		}
	}

	void operator()()
	{
		CopyTopAndBottomRows();
		CopyCenterRows();
	}

	const int width, height;
	const int imwidth, imheight;
	const int half_min_width, half_min_height;
	const int bytes_per_pixel;
	const unsigned char* const image_data;
	unsigned char* const result_data;
};
}

typedef std::vector<double> huevec;


void hue(huevec& out, int amount, int level)
{
	if (level <= 1) {
		if (long(out.size()) < amount)
			out.push_back(0.0);
		if (long(out.size()) < amount)
			out.push_back(0.5);
	} else {
		hue(out, amount, level - 1);
		const int lower = out.size();
		hue(out, amount, level - 1);
		const int upper = out.size();
		for (int i = lower; i < upper; ++i)
			out.at(i) += 1.0 / (1 << level);
	}
}

void hue(huevec& out, int amount)
{
	int level = 0;
	while ((1 << level) < amount)
		++level;

	out.reserve(amount);
	hue(out, amount, level);
}

std::vector<LSL::lslColor>& GetBigFixColoursPalette(int numteams)
{
	static std::vector<LSL::lslColor> result;
	slLogDebugFunc("numteams: %d", numteams);
	huevec huevector;
	static int satvalbifurcatepos;
	static std::vector<double> satvalsplittings;
	if (satvalsplittings.empty()) // insert ranges to bifurcate
	{
		satvalsplittings.push_back(1);
		satvalsplittings.push_back(0);
		satvalbifurcatepos = 0;
	}
	hue(huevector, numteams);
	int bisectionlimit = 20;
	for (int i = result.size(); i < numteams; i++) {
		double hue = huevector[i];
		double saturation = 1;
		double value = 1;
		int switccolors = i % 3; // why only 3 and not all combinations? because it's easy, plus the bisection limit cannot be divided integer by it

		if ((i % bisectionlimit) == 0) {
			satvalbifurcatepos = satvalbifurcatepos % (satvalsplittings.size() - 1);
			std::vector<double>::iterator toinsert = satvalsplittings.begin() + satvalbifurcatepos + 1;
			satvalsplittings.insert(toinsert, (satvalsplittings[satvalbifurcatepos] - satvalsplittings[satvalbifurcatepos + 1]) / 2 + satvalsplittings[satvalbifurcatepos + 1]);
			satvalbifurcatepos += 2;
		}

		if (switccolors == 1) {
			saturation = satvalsplittings[satvalbifurcatepos - 1];
		} else if (switccolors == 2) {
			value = satvalsplittings[satvalbifurcatepos - 1];
		}
		hue += 0.17; // use as starting point a zone where color band is narrow so that small variations means high change in visual effect
		if (hue > 1)
			hue -= 1;
		wxImage::HSVValue hsvcolor(hue, saturation, value);
		wxImage::RGBValue rgbvalue = wxImage::HSVtoRGB(hsvcolor);
		LSL::lslColor col(rgbvalue.red, rgbvalue.green, rgbvalue.blue);
		result.push_back(col);
	}
	return result;
}


wxImage BorderInvariantResizeImage(const wxImage& image, int width, int height)
{
	if (!image.IsOk() || (width == image.GetWidth() && height == image.GetHeight()))
		return image;

	wxImage ret(width, height);
	Resizer data_resize(ret, image, false);
	data_resize();

	if (image.HasAlpha()) {
		ret.InitAlpha();
		Resizer alpha_resize(ret, image, true);
		alpha_resize();
	}

	return ret;
}


wxColour GetColourFromUser(wxWindow* parent, const wxColour& colInit)
{
#ifdef __WXMSW__
	const wxString caption = _("Choose color");
#else
	const wxString caption = _("Choose color (only first 16 will be saved)");
#endif
	const wxString palette = _T("Default");
	wxColourData data;
	data = sett().GetCustomColors(palette);
	data.SetChooseFull(true);
	if (colInit.Ok()) {
		data.SetColour((wxColour&)colInit); // const_cast
	}

	wxColour colRet;
	wxColourDialog dialog(parent, &data);
	if (!caption.empty())
		dialog.SetTitle(caption);
	if (dialog.ShowModal() == wxID_OK) {
		colRet = dialog.GetColourData().GetColour();
	}
	//else: leave it invalid
	sett().SaveCustomColors(dialog.GetColourData(), palette);

	return colRet;
}

wxImage ReplaceChannelStatusColour(wxBitmap img, const wxColour& colour)
{
	wxImage ret = img.ConvertToImage();
	wxImage::HSVValue origcolour = wxImage::RGBtoHSV(wxImage::RGBValue(colour.Red(), colour.Green(), colour.Blue()));

	double bright = origcolour.value - 0.1 * origcolour.value;
	bright = LSL::Util::Clamp(bright, 0.0, 1.0);
	wxImage::HSVValue hsvdarker1(origcolour.hue, origcolour.saturation, bright);
	bright = origcolour.value - 0.5 * origcolour.value;
	bright = LSL::Util::Clamp(bright, 0.0, 1.0);
	wxImage::HSVValue hsvdarker2(origcolour.hue, origcolour.saturation, bright);

	wxImage::RGBValue rgbdarker1 = wxImage::HSVtoRGB(hsvdarker1);
	wxImage::RGBValue rgbdarker2 = wxImage::HSVtoRGB(hsvdarker2);


	ret.Replace(164, 147, 0, rgbdarker2.red, rgbdarker2.green, rgbdarker2.blue);

	ret.Replace(255, 228, 0, rgbdarker1.red, rgbdarker1.green, rgbdarker1.blue);

	ret.Replace(255, 253, 234, colour.Red(), colour.Green(), colour.Blue());

	return ret;
}

wxSize MakeFit(const wxSize& original, const wxSize& bounds)
{
	if ((bounds.GetWidth() <= 0) || (bounds.GetHeight() <= 0))
		return wxSize(0, 0);
	int sizex = (original.GetWidth() * bounds.GetHeight()) / original.GetHeight();
	if (sizex <= bounds.GetWidth()) {
		return wxSize(sizex, bounds.GetHeight());
	} else {
		int sizey = (original.GetHeight() * bounds.GetWidth()) / original.GetWidth();
		return wxSize(bounds.GetWidth(), sizey);
	}
}

void CopyToClipboard(const wxString& text)
{
	if (wxTheClipboard->Open()) {
		// This data objects are held by the clipboard,
		// so do not delete them in the app.
		wxTheClipboard->SetData(new wxTextDataObject(text));
		wxTheClipboard->Close();
	}
}


void OpenWebBrowser(const wxString& url)
{
	if (sett().GetWebBrowserUseDefault()
	    // These shouldn't happen, but if they do we use the default browser anyway.
	    ||
	    sett().GetWebBrowserPath() == wxEmptyString || sett().GetWebBrowserPath() == _T("use default")) {
		if (!wxLaunchDefaultBrowser(url)) {
			wxLogWarning(_T("can't launch default browser"));
			customMessageBoxNoModal(SL_MAIN_ICON, _("Couldn't launch browser. URL is: ") + url, _("Couldn't launch browser."));
		}
	} else {
		if (!wxExecute(sett().GetWebBrowserPath() + _T(" ") + url, wxEXEC_ASYNC)) {
			wxLogWarning(_T("can't launch browser: %s"), sett().GetWebBrowserPath().c_str());
			customMessageBoxNoModal(SL_MAIN_ICON, _("Couldn't launch browser. URL is: ") + url + _("\nBroser path is: ") + sett().GetWebBrowserPath(), _("Couldn't launch browser."));
		}
	}
}

wxBitmap getColourIcon(const wxColour& colour)
{
	wxImage img(colourbox_xpm);

	img.Replace(1, 1, 1, colour.Red(), colour.Green(), colour.Blue());

	int r, g, b;
	r = colour.Red() + 80;
	g = colour.Green() + 80;
	b = colour.Blue() + 80;
	img.Replace(2, 2, 2, r > 255 ? 255 : r, g > 255 ? 255 : g, b > 255 ? 255 : b);
	if (img.HasAlpha() == false)
	{
		img.InitAlpha();
	}
	/*r = colour.Red()-60; g = colour.Green()-60; b = colour.Blue()-60;
	img.Replace( 200, 200, 200, r<0?0:r, g<0?0:g, b<0?0:b );*/
	return wxBitmap(img);
	//	assert( IsOk() );
}
DEFINE_EVENT_TYPE(REFRESH_EVENT)
