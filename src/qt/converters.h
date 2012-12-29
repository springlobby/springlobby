#ifndef CONVERTERS_H
#define CONVERTERS_H

/** this code was copied from the wxQT branch of wxWidgets **/
#ifdef SL_QT_MODE
///////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/converter.h
// Purpose:     Converter utility classes and functions
// Author:      Peter Most, Kolya Kosenko
// Created:     02/28/10
// RCS-ID:      $Id$
// Copyright:   (c) Peter Most
//              (c) 2010 Kolya Kosenko
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/defs.h"
#include <QtCore/Qt>

// Rely on overloading and let the compiler pick the correct version, which makes
// them easier to use then to write wxQtConvertQtRectToWxRect() or wxQtConvertWxRectToQtRect()

class WXDLLIMPEXP_FWD_CORE wxPoint;
class QPoint;
wxPoint wxQtConvertPoint( const QPoint &point );
QPoint wxQtConvertPoint( const wxPoint &point );

class WXDLLIMPEXP_FWD_CORE wxRect;
class QRect;
wxRect wxQtConvertRect( const QRect &rect );
QRect  wxQtConvertRect( const wxRect &rect );

class WXDLLIMPEXP_FWD_BASE wxString;
class QString;
wxString wxQtConvertString( const QString &str );
QString  wxQtConvertString( const wxString &str );

#if wxUSE_DATETIME

class WXDLLIMPEXP_FWD_BASE wxDateTime;
class QDate;

wxDateTime wxQtConvertDate(const QDate& date);
QDate wxQtConvertDate(const wxDateTime& date);

#endif // wxUSE_DATETIME

class WXDLLIMPEXP_FWD_BASE wxSize;
class QSize;
wxSize wxQtConvertSize( const QSize  &size );
QSize  wxQtConvertSize( const wxSize &size );

Qt::Orientation wxQtConvertOrientation( long style, wxOrientation defaultOrientation );
wxOrientation wxQtConvertOrientation( Qt::Orientation );

void wxMissingImplementation( const char fileName[], unsigned lineNumber,
	const char feature[] );

#define wxMISSING_IMPLEMENTATION( feature )\
	wxMissingImplementation( __FILE__, __LINE__, feature )

class QImage;
class wxImage;
QImage wxQtConvertImage( const wxImage &image );
wxImage wxQtConvertImage( QImage qtImage );

/** ****************************************************** **/
#endif //#ifdef SL_QT_MODE
#endif // CONVERTERS_H
