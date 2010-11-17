
/*!
	\file
	\brief Command Line Parser.

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

#ifndef QTARG__CMDLINEIFACE_HPP__INCLUDED
#define QTARG__CMDLINEIFACE_HPP__INCLUDED


// Qt include.
#include <QtCore/QChar>

class QtArgIface;


//
// QtArgCmdLineIface
//

//! Command line arguments parser interface.
class QtArgCmdLineIface {
	public:
		//! Type of the list of the arguments in the command line.
		typedef QList< QtArgIface* > QtArgumentsList;

		virtual ~QtArgCmdLineIface()
		{}

		//! \return List of arguments handled by this QtArgCmdLine.
		virtual const QtArgumentsList & arguments() const = 0;

		//! \return Delimiter char.
		virtual const QChar & delimiter() const = 0;

		//! Set delimiter char.
		virtual void setDelimiter( const QChar & delim ) = 0;

		//! Add new argument.
		//! Can throw exceptions.
		virtual void addArg( QtArgIface & arg ) = 0;

		//! Add new argument.
		//! Can throw exceptions.
		virtual void addArg( QtArgIface * arg ) = 0;

		//! Parse arguments.
		//! Can throw exceptions.
		virtual void parse() = 0;
}; // class QtArgCmdLineIface

#endif // QTARG__CMDLINEIFACE_HPP__INCLUDED
