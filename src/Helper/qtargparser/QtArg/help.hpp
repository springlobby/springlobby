
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

#ifndef QTARG__HELP_HPP__INCLUDED
#define QTARG__HELP_HPP__INCLUDED

// Qt include.
#include <QtCore/QTextStream>
#include <QtCore/QMap>

// QtArg include.
#include "cmdline.hpp"
#include "arg.hpp"
#include "helpiface.hpp"
#include "exceptions.hpp"

#include <iostream>


//
// QtArgHelpPrinterHasntDefinedEx
//

//! Throws when user hasn't defined help printer.
class QtArgHelpPrinterHasntDefinedEx
	:	public QtArgBaseException
{
	public:
		QtArgHelpPrinterHasntDefinedEx()
			:	QtArgBaseException( QString::fromLatin1(
					"Help printer hasn't defined" ) )
		{}

		virtual ~QtArgHelpPrinterHasntDefinedEx() throw()
		{}
}; // class QtArgHelpPrinterHasntDefinedEx


//
// QtHelpHasPrintedEx
//

//! Throws when help has printed.
class QtHelpHasPrintedEx
	:	public QtArgBaseException
{
	public:
		QtHelpHasPrintedEx()
			:	QtArgBaseException( QString::null )
		{}

		virtual ~QtHelpHasPrintedEx() throw()
		{}
}; // class QtHelpHasPrintedEx


//
// QtArgDefaultHelpPrinter
//

//! Default implementation of the QtArgHelpPrinterIface.
class QtArgDefaultHelpPrinter
	:	public QtArgHelpPrinterIface
{
	public:
		QtArgDefaultHelpPrinter( const QString & helpHandle )
			:	m_max_flags_length( 0 )
			,	m_max_names_length( 0 )
			,	m_helpHandle( helpHandle )
		{
		}

		virtual ~QtArgDefaultHelpPrinter() {}

		//! Print help on the specified argument.
		virtual void print( const QtArgIface * arg, const QChar & delimiter )
		{
			QString flags;

			{
				const QtArgIface::FlagsList & fs = arg->flags();

				if( fs.size() )
					flags.append( delimiter );

				foreach( QChar f, fs )
					flags.append( f );
			}

			if( m_max_flags_length < flags.length() )
				m_max_flags_length = flags.length();

			if( flags.length() )
			{
				m_help.append( QtArgHelpPrinterIface::beforeFlags );
				m_help.append( flags );
			}

			if( flags.length() && arg->names().size() )
			{
				m_help.append( QString::fromLatin1( "," ) );
				m_help.append( QtArgHelpPrinterIface::afterFlags );
				m_help.append( QtArgHelpPrinterIface::namesString );
				m_help.append( QtArgHelpPrinterIface::afterNames );

				m_names.append( arg->names() );
			}
			else if( arg->names().size() )
			{
				m_help.append( QtArgHelpPrinterIface::beforeNames );
				m_help.append( QtArgHelpPrinterIface::namesString );
				m_help.append( QtArgHelpPrinterIface::afterNames );

				m_names.append( arg->names() );
			}
			else if( flags.length() )
				m_help.append( QtArgHelpPrinterIface::afterNames );
			else
				m_help.append( QtArgHelpPrinterIface::beforeDescription );

			m_help.append( arg->description() );
			m_help.append( QtArgHelpPrinterIface::newLine );
		}

		//! Print string \par str.
		virtual void print( const QString & str ) const
		{
			QTextStream out( stdout );
			out << str << endl;
		}

		//! Flush help into the out stream;
		virtual void flush( const QChar & delimiter )
		{
			prepareNames();
			replaceNames( delimiter );

			const int beforeFlagsLength = 2;
			const int afterFlagsLength = 1;
			const int afterNamesLength = 3;
			const int beforeNamesLength = beforeFlagsLength
				+ afterFlagsLength + m_max_flags_length + 1;
			const int beforeDescriptionLength = beforeNamesLength
				+ afterNamesLength + m_max_names_length;

			const QString beforeFlags =
				QString( beforeFlagsLength, ' ' );
			const QString afterFlags =
				QString( afterFlagsLength, ' ' );
			const QString afterNames =
				QString( afterNamesLength, ' ' );
			const QString beforeNames =
				QString( beforeNamesLength, ' ' );
			const QString beforeDescription =
				QString( beforeDescriptionLength, ' ' );

			m_help.replace( QtArgHelpPrinterIface::beforeFlags,
				beforeFlags );
			m_help.replace( QtArgHelpPrinterIface::beforeNames,
				beforeNames );
			m_help.replace( QtArgHelpPrinterIface::beforeDescription,
				beforeDescription );

			// Replacing of the QtArgHelpPrinterIface::afterFlags.

			while( true )
			{
				QTextStream out( stdout );

				const int afterFlagsIndex =
					m_help.indexOf( QtArgHelpPrinterIface::afterFlags );

				if( afterFlagsIndex != -1 )
				{
					int newLineIndex =
						m_help.lastIndexOf( QtArgHelpPrinterIface::newLine,
							afterFlagsIndex );

					if( newLineIndex == -1 )
						newLineIndex = 0;

					const int space = beforeNamesLength -
						( afterFlagsIndex - newLineIndex ) +
						( newLineIndex > 0 ?
							QtArgHelpPrinterIface::newLine.length() : 0 );

					m_help.replace( afterFlagsIndex,
						QtArgHelpPrinterIface::afterFlags.length(),
						QString( space, ' ' ) );
				}
				else
					break;
			}

			// Replacing of the QtArgHelpPrinterIface::afterNames.

			while( true )
			{
				const int afterNamesIndex =
					m_help.indexOf( QtArgHelpPrinterIface::afterNames );

				if( afterNamesIndex != -1 )
				{
					int newLineIndex =
						m_help.lastIndexOf( QtArgHelpPrinterIface::newLine,
							afterNamesIndex );

					if( newLineIndex == -1 )
						newLineIndex = 0;

					const int space = beforeDescriptionLength -
						( afterNamesIndex - newLineIndex ) +
						( newLineIndex > 0 ?
							QtArgHelpPrinterIface::newLine.length() : 0 );

					m_help.replace( afterNamesIndex,
						QtArgHelpPrinterIface::afterNames.length(),
						QString( space, ' ' ) );
				}
				else
					break;
			}

			m_help.replace( QtArgHelpPrinterIface::newLine,
				QString::fromLatin1( "\n" ) );

			QTextStream out( stdout );
			out << m_helpHandle << endl;
			out << m_help;
		}

	private:
		//! \return Is name \par name unique in names \par list.
		inline bool
		isUnique( const QString & name, const QStringList & list ) const
		{
			bool contains = false;

			foreach( QString n, list )
				if( n.startsWith( name ) )
				{
					if( !contains )
						contains = true;
					else
						return false;
				}

			return true;
		}

		//! Prepare names for printing.
		inline void prepareNames()
		{
			QStringList rawNames;

			foreach( QStringList list, m_names )
			{
				foreach( QString name, list )
				{
					rawNames << name;
				}
			}

			for( int i = 0; i < m_names.size(); ++i )
			{
				for( int j = 0; j < m_names[ i ].size(); ++j )
				{
					int index = 1;

					QString & fullName = m_names[ i ][ j ];

					while( !isUnique( fullName.left( index ), rawNames ) )
						++index;

					if( index != fullName.length() )
					{
						fullName.insert( index, '[' );
						fullName.append( ']' );
					}
				}
			}
		}

		//! Replace names.
		inline void replaceNames( const QChar & delimiter )
		{
			foreach( QStringList list, m_names )
			{
				QString namesString;
				bool isFirst = true;

				foreach( QString name, list )
				{
					if( !isFirst )
						namesString.append( QString::fromLatin1( ", " ) );
					else
						isFirst = false;

					namesString.append( delimiter );
					namesString.append( delimiter );
					namesString.append( name );
				}

				if( m_max_names_length < namesString.length() )
					m_max_names_length = namesString.length();

				m_help.replace( m_help.indexOf(
						QtArgHelpPrinterIface::namesString ),
					QtArgHelpPrinterIface::namesString.length(),
					namesString );
			}
		}

	private:
		//! Prepared help string.
		QString m_help;
		//! Max length of the flags.
		int m_max_flags_length;
		//! Max length of the names.
		int m_max_names_length;

		//! Program description printed before help.
		QString m_helpHandle;

		typedef QList< QStringList > NamesList;
		//! Names of arguments.
		NamesList m_names;
}; // class QtArgDefaultHelpPrinter


//
// QtArgHelp
//

//! Argument for the help priniting.
/*!
	Defined as argument with "h" flag and
	"help" name.
*/
class QtArgHelp
	:	public QtArg
{
	public:
		QtArgHelp( QtArgCmdLine * cmdLine );

		virtual ~QtArgHelp();

		//! \return Printer implementation.
		QtArgHelpPrinterIface * printer();

		//! Set printer implementation.
		void setPrinter( QtArgHelpPrinterIface * p );

		//! Set long argument's description.
		void setArgumentDescription( QtArgIface * arg, const QString & desc );

		//! \name QtArgIface implementation.
		//! \{

			//! Process with visitor.
			virtual void visit( QtArgCmdLineContext & context );

		//! \}

	private:
		//! Print help on the arg \param arg.
		void handleArgument( QtArgIface * arg ) const;

	private:
		//! Printer implemenation.
		QtArgHelpPrinterIface * m_printer;
		//! Pointer to the QtArgCmdLine.
		QtArgCmdLine * m_cmdLine;
		//! Long descriptions.
		QMap< QtArgIface*, QString > m_descriptions;
}; // class QtArgHelp


//
// QtArgHelp implementation.
//

inline
QtArgHelp::QtArgHelp( QtArgCmdLine * cmdLine )
	:	QtArg( 'h', QString::fromLatin1( "help" ),
			QString::fromLatin1( "Print this help." ) )
	,	m_printer( NULL )
	,	m_cmdLine( cmdLine )
{
}

inline
QtArgHelp::~QtArgHelp()
{
}

inline QtArgHelpPrinterIface *
QtArgHelp::printer()
{
	return m_printer;
}

inline void
QtArgHelp::setPrinter( QtArgHelpPrinterIface * p )
{
	m_printer = p;
}

inline void
QtArgHelp::setArgumentDescription( QtArgIface * arg, const QString & desc )
{
	m_descriptions.insert( arg, desc );
}

inline void
QtArgHelp::handleArgument( QtArgIface * arg ) const
{
	const QtArgCmdLineIface::QtArgumentsList & args =
		arg->arguments();

	if( args.size() > 1 )
	{
		foreach( QtArgIface * child, args )
			handleArgument( child );
	}
	else
		m_printer->print( args.front(), m_cmdLine->delimiter() );
}

inline void
QtArgHelp::visit( QtArgCmdLineContext & context )
{
	if( m_printer )
	{
		if( !context.atEnd() )
		{
			const QString & argName = context.next();

			try {
				QtArgIface * arg = m_cmdLine->findArgument( argName );

				QMap< QtArgIface*, QString>::ConstIterator it =
					m_descriptions.find( arg );

				if( it != m_descriptions.end() )
					m_printer->print( it.value() );
				else
					m_printer->print( arg->description() );
			}
			catch( const QtArgUnknownArgumentEx & x )
			{
				m_printer->print(
					QString::fromLatin1( "Unknown argument: %1%1%2" )
						.arg( m_cmdLine->delimiter() )
						.arg( argName ) );
			}
		}
		else
		{
			const QtArgCmdLineIface::QtArgumentsList & args =
				m_cmdLine->arguments();

			foreach( QtArgIface * arg, args )
				handleArgument( arg );

			m_printer->flush( m_cmdLine->delimiter() );
		}

		throw QtHelpHasPrintedEx();
	}
	else
		throw QtArgHelpPrinterHasntDefinedEx();
}

#endif // QTARG__HELP_HPP__INCLUDED
