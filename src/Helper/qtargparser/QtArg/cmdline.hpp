
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

#ifndef QTARG__CMDLINE_HPP__INCLUDED
#define QTARG__CMDLINE_HPP__INCLUDED


// QtArg include.
#include "cmdlineiface.hpp"
#include "exceptions.hpp"
#include "visitor.hpp"
#include "arg.hpp"
#include "cmdlinecontext.hpp"

// Qt include.
#include <QtCore/QCoreApplication>
#include <QtCore/QVariant>
#include <QtCore/QChar>
#include <QtCore/QString>
#include <QtCore/QStringList>


//
// QtArgCmdLine
//

//! Command line arguments parser.
class QtArgCmdLine
	:	public QtArgCmdLineIface
{
	public:
		/*!
			Construct QtArgCmdLine and initialize him with
			QCoreApplication::arguments(). So be carefull to
			initialize QCoreApplication first.
		*/
		QtArgCmdLine();

		/*!
			We expect programm name as first argument.
			That means that \arg args[ 0 ] should be a name
			of the programm.
		*/
		explicit QtArgCmdLine( const QStringList & args );

		/*!
			We expect programm name as first argument.
			That means that \arg argv[ 0 ] should be a name
			of the programm.
		*/
		QtArgCmdLine( int argc, char ** argv );

		~QtArgCmdLine();

		//! \return List of arguments handled by this QtArgCmdLine.
		const QtArgCmdLineIface::QtArgumentsList & arguments() const;

		//! \return Delimiter char.
		const QChar & delimiter() const;

		//! Set delimiter char.
		void setDelimiter( const QChar & delim );

		//! Find argument by his name.
		QtArgIface * findArgument( const QString & name );

		//! Find argument by his flag.
		QtArgIface * findArgument( const QChar & flag );

		//! Add new argument.
		//! Can throw exceptions.
		void addArg( QtArgIface & arg );

		//! Add new argument.
		//! Can throw exceptions.
		void addArg( QtArgIface * arg );

		//! Parse arguments.
		//! Can throw exceptions.
		void parse();

	private:
		//! Command line context.
		QtArgCmdLineContext m_context;
		//! User-defined arguments.
		//! Result of the parsing stores in this arguments.
		QtArgCmdLineIface::QtArgumentsList m_args;

		//! Delimiter. With this QChar starts every option or flag.
		QChar m_delimiter;

	private:
		//! Add new argument.
		void addArgImpl( QtArgIface * arg );
		//! Check correctness of all arguments.
		//! Can throw Exceptions.
		void checkArgumentsCorrectness() const;
		//! \return true if \a str is an flag or string of the flags.
		bool isFlag( const QString & str ) const;
		//! \return true if \a str is an argument.
		bool isArgument( const QString & str ) const;
		/*!
			Split string into argument and his value.

			Value can be QString::null if there is no '=' character
			will be found.

			\par toCheck - QString that should be splitted.
			\par argument - Name of the argument.
			\par value - Value of the argument.
		*/
		void splitArgumentAndValue( const QString & toCheck,
			QString & argument, QString & value );
		//! Removes all delimiters from the beginning of the given string.
		QString removeDelimiters( const QString & str ) const;
		//! Check all mndatory argumentss are defined.
		void checkMandatoryArguments() const;
}; // class QtArgCmdLine


//
// QtArgCmdLine implementation.
//

namespace /* anonymous */ {

	//! Every flag starts with this character.
	//! Every option name start with doubled this character.
	const QChar defaultDelimiter = QChar( 0x2D );

} /* namespace anonymous */

inline
QtArgCmdLine::QtArgCmdLine()
	:	m_context( QCoreApplication::arguments() )
	,	m_delimiter( defaultDelimiter )
{
	// Skip program name.
	m_context.next();
}

inline
QtArgCmdLine::QtArgCmdLine( const QStringList & args )
	:	m_context( args )
	,	m_delimiter( defaultDelimiter )
{
	// Skip program name.
	m_context.next();
}

inline
QtArgCmdLine::QtArgCmdLine( int argc, char ** argv )
	:	m_context( QStringList() )
	,	m_delimiter( defaultDelimiter )
{
	QStringList tmp;

	if( argc > 1 )
	{
		for( int i = 1; i < argc; ++i )
			tmp.append( QString::fromLocal8Bit( argv[ i ] ) );
	}

	m_context = QtArgCmdLineContext( tmp );
}

inline
QtArgCmdLine::~QtArgCmdLine()
{
}

inline const QtArgCmdLineIface::QtArgumentsList &
QtArgCmdLine::arguments() const
{
	return m_args;
}

inline const QChar &
QtArgCmdLine::delimiter() const
{
	return m_delimiter;
}

inline void
QtArgCmdLine::setDelimiter( const QChar & delim )
{
	m_delimiter = delim;
}

inline void
QtArgCmdLine::addArg( QtArgIface & arg )
{
	addArgImpl( &arg );
}

inline void
QtArgCmdLine::addArg( QtArgIface * arg )
{
	addArgImpl( arg );
}

inline void
QtArgCmdLine::parse()
{
	checkArgumentsCorrectness();

	QString lastArgNameOrFlag;

	while( !m_context.atEnd() )
	{
		const QString & arg = m_context.next();
		QString name, nameWithoutDelims;

		{
			QString value;

			splitArgumentAndValue( arg, name, value );

			nameWithoutDelims = removeDelimiters( name );

			if( !value.isNull() )
				m_context.prepend( value );
		}

		// Argument.

		if( isArgument( arg ) )
		{
			QtArgIface * argument = findArgument( nameWithoutDelims );
			lastArgNameOrFlag = name;

			if( argument != NULL )
			{
				argument->process( m_context );
				argument->visit( m_context );
				argument->checkConstraint();
			}
			else
				throw QtArgUnknownArgumentEx(
					QString::fromLatin1( "Unknown argument: %1" )
						.arg( name ) );
		}

		// Flag.

		else if( isFlag( arg ) )
		{
			for( int i = 0, i_max = nameWithoutDelims.length() - 1;
				i <= i_max; ++i )
			{
				QtArgIface * argument = findArgument( nameWithoutDelims[ i ] );
				lastArgNameOrFlag = QString( m_delimiter )
					+ nameWithoutDelims[ i ];

				if( i != i_max )
				{
					QStringList dummyArgsList( nameWithoutDelims
						.right( nameWithoutDelims.length() - i - 1 ) );
					QtArgCmdLineContext dummyContext( dummyArgsList );

					if( argument->process( dummyContext ) )
					{
						argument->visit( m_context );
						argument->checkConstraint();

						break;
					}
				}
				else
					argument->process( m_context );

				argument->visit( m_context );
				argument->checkConstraint();
			}
		}

		// Something unexpected.

		else
			throw QtArgUnexpectedOptionEx(
				QString::fromLatin1( "Unexpected option: %1. "
					"Argument \"%2\" doesn't expect any values." )
						.arg( name ).arg( lastArgNameOrFlag ) );
	}

	checkMandatoryArguments();
}

inline void
QtArgCmdLine::addArgImpl( QtArgIface * arg )
{
	if( !m_args.contains( arg ) )
	{
		m_args.append( arg );

		checkArgumentsCorrectness();
	}
}

inline void
QtArgCmdLine::checkArgumentsCorrectness() const
{
	QtArgIface::FlagsList flags;
	QtArgIface::NamesList names;

	foreach( QtArgIface * arg, m_args )
		arg->check( m_delimiter, flags, names );
}

inline bool
QtArgCmdLine::isFlag( const QString & str ) const
{
	if( !isArgument( str ) )
		if( str.startsWith( m_delimiter ) )
			return true;

	return false;
}

inline bool
QtArgCmdLine::isArgument( const QString & str ) const
{
	static const QString doubleDelimiter =
		QString( m_delimiter ) + m_delimiter;

	if( str.startsWith( doubleDelimiter ) )
		return true;

	return false;
}

inline void
QtArgCmdLine::splitArgumentAndValue( const QString & toCheck,
	QString & argument, QString & value )
{
	const int index = toCheck.indexOf( '=' );

	if( index != -1 )
	{
		argument = toCheck.left( index );
		value = toCheck.right( toCheck.length() - index - 1 );
	}
	else
	{
		argument = toCheck;
		value = QString::null;
	}
}

inline QString
QtArgCmdLine::removeDelimiters( const QString & str ) const
{
	QString tmp = str;

	if( tmp.startsWith( m_delimiter ) )
		tmp.remove( 0, 1 );

	if( tmp.startsWith( m_delimiter ) )
		tmp.remove( 0, 1 );

	return tmp;
}

inline QtArgIface *
QtArgCmdLine::findArgument( const QString & name )
{
	QtArgIface * ret = NULL;

	foreach( QtArgIface * arg, m_args )
	{
		QtArgIface * a = arg->giveArgument( name );

		if( ret == NULL )
			ret = a;
		else if( a )
			throw QtArgUnknownArgumentEx(
				QString::fromLatin1( "Unknown argument: %1%1%2" )
					.arg( m_delimiter )
					.arg( name ) );
	}

	if( ret == NULL )
		throw QtArgUnknownArgumentEx(
			QString::fromLatin1( "Unknown argument: %1%1%2" )
				.arg( m_delimiter )
				.arg( name ) );

	return ret;
}

inline QtArgIface *
QtArgCmdLine::findArgument( const QChar & flag )
{
	foreach( QtArgIface * arg, m_args )
	{
		QtArgIface * ret = arg->giveArgument( flag );

		if( ret != NULL )
			return ret;
	}

	throw QtArgUnknownArgumentEx(
		QString::fromLatin1( "Unknown argument: %1%2" )
			.arg( m_delimiter )
			.arg( flag ) );
}

inline void
QtArgCmdLine::checkMandatoryArguments() const
{
	foreach( const QtArgIface * arg, m_args )
		arg->check();
}

#endif // QTARG__CMDLINE_HPP__INCLUDED
