/*!
	\file
	\brief Base class of the QtArg exceptions.

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

#ifndef QTARG__EXCEPTIONS_HPP__INCLUDED
#define QTARG__EXCEPTIONS_HPP__INCLUDED

// C++ include.
#include <stdexcept>

// Qt include.
#include <QtCore/QString>


//
// QtArgBaseException
//

//! Base exception for all exceptions of the QtArg library.
class QtArgBaseException
	:	public std::runtime_error
{
	public:
		explicit QtArgBaseException( const QString & desc )
			:	std::runtime_error( desc.toLocal8Bit().constData() )
		{}

		virtual ~QtArgBaseException() throw()
		{}
}; // class QtArgBaseException


//
// QtArgUnknownArgumentEx
//

//! Throws when in command line found unknown argument.
class QtArgUnknownArgumentEx
	:	public QtArgBaseException
{
	public:
		explicit QtArgUnknownArgumentEx( const QString & desc )
			:	QtArgBaseException( desc )
		{}

		virtual ~QtArgUnknownArgumentEx() throw()
		{}
}; // class QtArgUnknownArgumentEx


//
// QtArgUnexpectedOptionEx
//

//! Throws when in command line found unexpected option.
/*!
	An option differs from an flag or argument
	by lack of the delimiter or delimiters as prefix.
*/
class QtArgUnexpectedOptionEx
	:	public QtArgBaseException
{
	public:
		explicit QtArgUnexpectedOptionEx( const QString & desc )
			:	QtArgBaseException( desc )
		{}

		virtual ~QtArgUnexpectedOptionEx() throw()
		{}
}; // class QtArgUnexpectedOptionEx


//
// QtArgNotDefinedOptionForTheArgumentEx
//

//! Throws when user doesn't define option for the argument
//! that expect it.
class QtArgNotDefinedOptionForTheArgumentEx
	:	public QtArgBaseException
{
	public:
		explicit QtArgNotDefinedOptionForTheArgumentEx( const QString & desc )
			:	QtArgBaseException( desc )
		{}

		virtual ~QtArgNotDefinedOptionForTheArgumentEx() throw()
		{}
}; // QtArgNotDefinedOptionForTheArgumentEx


//
// QtArgNotDefinedMandatoryArgumentEx
//

//! Throws when user doesn't define mandatory argument.
class QtArgNotDefinedMandatoryArgumentEx
	:	public QtArgBaseException
{
	public:
		explicit QtArgNotDefinedMandatoryArgumentEx( const QString & desc )
			:	QtArgBaseException( desc )
		{}

		virtual ~QtArgNotDefinedMandatoryArgumentEx() throw()
		{}
}; // QtArgNotDefinedMandatoryArgumentEx


//
// QtArgDissallowedFlagOrNameEx
//

//! Throws when the flag is equal to the delimiter or space.
class QtArgDissallowedFlagOrNameEx
	:	public QtArgBaseException
{
	public:
		explicit QtArgDissallowedFlagOrNameEx( const QString & desc )
			:	QtArgBaseException( desc )
		{}

		virtual ~QtArgDissallowedFlagOrNameEx() throw()
		{}
}; // class QtArgDissallowedFlagOrNameEx


//
// QtArgNameOrFlagAlreadyKnownEx
//

//! Throws when the flag or name already in use by another argument.
class QtArgNameOrFlagAlreadyKnownEx
	:	public QtArgBaseException
{
	public:
		explicit QtArgNameOrFlagAlreadyKnownEx( const QString & desc )
			:	QtArgBaseException( desc )
		{}

		virtual ~QtArgNameOrFlagAlreadyKnownEx() throw()
		{}
}; // class QtArgNameOrFlagAlreadyKnownEx


//
// QtArgNotDefinedNorNameNorFlagEx
//

//! Throws when in command line found argument without defined name and flag.
class QtArgNotDefinedNorNameNorFlagEx
	:	public QtArgBaseException
{
	public:
		explicit QtArgNotDefinedNorNameNorFlagEx()
			:	QtArgBaseException(
					QString::fromLatin1(
						"Not defined nor name nor flag for argument." ) )
		{}

		virtual ~QtArgNotDefinedNorNameNorFlagEx() throw()
		{}
}; // class QtArgNotDefinedNorNameNorFlagEx


//
// QtArgContraintNotObservedEx
//

//! Throws when constraint for the argument's values didn't observed.
class QtArgContraintNotObservedEx
	:	public QtArgBaseException
{
	public:
		explicit QtArgContraintNotObservedEx( const QString & desc )
			:	QtArgBaseException( desc )
		{}

		virtual ~QtArgContraintNotObservedEx() throw()
		{}
}; // class QtArgContraintNotObservedEx

#endif // QTARG__EXCEPTIONS_HPP__INCLUDED
