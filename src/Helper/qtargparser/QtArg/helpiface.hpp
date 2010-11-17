
/*!
	\file
	\brief Help printer.

	\author Igor P. Mironchik (imironchick at gmail dot com).

	Copyright (c) 2010 Igor P. Mironchik

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef QTARG__HELPIFACE_HPP__INCLUDED
#define QTARG__HELPIFACE_HPP__INCLUDED

// Qt include.
#include <QtCore/QString>

// QtArg include.
#include "arg.hpp"


//
// QtArgHelpPrinterIface
//

//! Interface for the help printer.
class QtArgHelpPrinterIface {
	protected:
		virtual ~QtArgHelpPrinterIface() {}

	public:
		//! Print help on the specified argument.
		virtual void print( const QtArgIface * arg,
			const QChar & delimiter ) = 0;

		//! Print string \par str.
		virtual void print( const QString & str ) const = 0;

		//! Flush help into the out stream;
		virtual void flush( const QChar & delimite ) = 0;

	public:
		/*!
			\name This constants will be replaced with
			correct spaces.

			\li beforeFlags - space before flags.

			\li afterFlags - space after flags and ','.

			\li afterNames - space after names.

			\li beforeNames - space before names. Equals to
			beforeFlags + longest flags.

			\li beforeDescription - space before description. Equals to
			beforeNames + longest names.
		*/
		//! \{

			static const QString beforeFlags;

			static const QString afterFlags;

			static const QString afterNames;

			static const QString beforeNames;

			static const QString beforeDescription;

		//! \}

		//! New line in help string.
		static const QString newLine;
		//! String with names of the argument.
		static const QString namesString;
}; // class QtArgHelpPrinterIface

const QString QtArgHelpPrinterIface::beforeFlags =
	QString::fromLatin1( "%beforeFlags%" );

const QString QtArgHelpPrinterIface::afterFlags =
	QString::fromLatin1( "%afterFlags%" );

const QString QtArgHelpPrinterIface::afterNames =
	QString::fromLatin1( "%afterNames%" );

const QString QtArgHelpPrinterIface::beforeNames =
	QString::fromLatin1( "%beforeNames%" );

const QString QtArgHelpPrinterIface::beforeDescription =
	QString::fromLatin1( "%beforeDescription%" );

const QString QtArgHelpPrinterIface::newLine =
	QString::fromLatin1( "%newLine%" );

const QString QtArgHelpPrinterIface::namesString =
	QString::fromLatin1( "%namesString%" );

#endif // QTARG__HELPIFACE_HPP__INCLUDED
