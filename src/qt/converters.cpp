#include "converters.h"

#include <QtGui/QCursor>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>
#include "wx/gdicmn.h"
#include "wx/gdicmn.h"
#include "wx/window.h"

void wxMissingImplementation( const char fileName[], unsigned lineNumber,
	const char feature[] )
{
	// Make it look similar to the assert messages:

	fprintf( stderr, "%s(%d): Missing implementation of \"%s\"\n", fileName, lineNumber, feature );
}

#if wxUSE_GUI
wxPoint wxGetMousePosition()
{
	return wxQtConvertPoint( QCursor::pos() );
}

void wxGetMousePosition( int *x, int *y )
{
	wxPoint position = wxGetMousePosition();

	*x = position.x;
	*y = position.y;
}
#endif

wxWindow *wxFindWindowAtPoint(const wxPoint& pt)
{
	/* Another option is to use QApplication::topLevelAt()
	 * but that gives the QWidget so the wxWindow list must
	 * be traversed comparing with this, or use the pointer from
	 * a wxQtWidget/wxQtFrame to the window, but they have
	 * no standard interface to return that. */
	return wxGenericFindWindowAtPoint( pt );
}

wxWindow *wxFindWindowAtPointer(wxPoint& pt)
{
	pt = wxQtConvertPoint( QCursor::pos() );

	return wxFindWindowAtPoint( pt );
}

bool wxGetKeyState(wxKeyCode key)
{
	/* FIXME: Qt doesn't provide a method to check the state of keys others
	 * than modifiers (shift, control, alt, meta). A platform-specific method
	 * is needed, probably one per platform Qt runs on. */
	switch ( key )
	{
		case WXK_CONTROL:
			return QApplication::keyboardModifiers().testFlag(Qt::ControlModifier);
		case WXK_SHIFT:
			return QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier);
		case WXK_ALT:
			return QApplication::keyboardModifiers().testFlag(Qt::AltModifier);
		case WXK_WINDOWS_LEFT:
			return QApplication::keyboardModifiers().testFlag(Qt::MetaModifier);
		default:
			wxMISSING_IMPLEMENTATION( "wxGetKeyState for non-modifiers keys" );
			return false;
	}
}

int wxDisplayDepth()
{
	return QApplication::desktop()->depth();
}

void wxDisplaySize(int *width, int *height)
{
	*width = QApplication::desktop()->width();
	*height = QApplication::desktop()->height();
}

void wxDisplaySizeMM(int *width, int *height)
{
	*width = QApplication::desktop()->widthMM();
	*height = QApplication::desktop()->heightMM();
}

void wxBell()
{
	QApplication::beep();
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
	QRect r = QApplication::desktop()->availableGeometry();

	*x = r.x();
	*y = r.y();
	*width = r.width();
	*height = r.height();
}


bool wxColourDisplay()
{
	return QApplication::desktop()->depth() > 1;
}

bool wxLaunchDefaultApplication(const wxString& path, int WXUNUSED( flags ) )
{
	return QDesktopServices::openUrl( QUrl::fromLocalFile( wxQtConvertString( path ) ) );
}

#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtGui/QFont>
#include <QtCore/QSize>

#if wxUSE_DATETIME
	#include "wx/datetime.h"
	#include <QtCore/QDate>
#endif // wxUSE_DATETIME

wxPoint wxQtConvertPoint( const QPoint &point )
{
	if (point.isNull())
		return wxDefaultPosition;

	return wxPoint( point.x(), point.y() );
}

QPoint wxQtConvertPoint( const wxPoint &point )
{
	if (point == wxDefaultPosition)
		return QPoint();

	return QPoint( point.x, point.y );
}


QRect wxQtConvertRect( const wxRect &rect )
{
	return QRect( rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight() );
}

wxRect wxQtConvertRect( const QRect &rect )
{
	return wxRect( rect.x(), rect.y(), rect.width(), rect.height() );
}



//wxString wxQtConvertString( const QString &str )
//{
//	return wxString( qPrintable( str ));
//}

QString  wxQtConvertString( const wxString &str )
{
	return QString( str.mb_str() );
}


#if wxUSE_DATETIME

wxDateTime wxQtConvertDate(const QDate& date)
{
	if ( date.isNull() )
		return wxDateTime(date.day(),
			static_cast<wxDateTime::Month>(date.month() - 1),
			date.year(), 0, 0, 0, 0);
	else
		return wxDateTime();
}

QDate wxQtConvertDate(const wxDateTime& date)
{
	if ( date.IsValid() )
		return QDate(date.GetYear(), date.GetMonth() + 1, date.GetDay());
	else
		return QDate();
}

#endif // wxUSE_DATETIME

wxSize wxQtConvertSize( const QSize  &size )
{
	if (size.isNull())
		return wxDefaultSize;

	return wxSize(size.width(), size.height());
}

QSize  wxQtConvertSize( const wxSize &size )
{
	if (size == wxDefaultSize)
		return QSize();

	return QSize(size.GetWidth(), size.GetHeight());
}

Qt::Orientation wxQtConvertOrientation( long style, wxOrientation defaultOrientation )
{
	if (( style & ( wxHORIZONTAL | wxVERTICAL )) == 0 )
		style |= defaultOrientation;

	switch ( style & ( wxHORIZONTAL | wxVERTICAL ))
	{
		case wxHORIZONTAL:
			return Qt::Horizontal;

		case wxVERTICAL:
			return Qt::Vertical;
	}
	wxFAIL_MSG( "Unreachable code!" );
	return static_cast< Qt::Orientation >( -1 );
}


wxOrientation wxQtConvertOrientation( Qt::Orientation qtOrientation )
{
	switch ( qtOrientation )
	{
		case Qt::Horizontal:
			return wxHORIZONTAL;

		case Qt::Vertical:
			return wxVERTICAL;
	}
	wxFAIL_MSG( "Unreachable code!" );
	return static_cast< wxOrientation >( -1 );
}

