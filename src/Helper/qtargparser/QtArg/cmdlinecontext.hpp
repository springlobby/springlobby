
/*!
	\file
	\brief Command Line Parser Context.

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

#ifndef QTARG__CMDLINECONTEXT_HPP__INCLUDED
#define QTARG__CMDLINECONTEXT_HPP__INCLUDED

// Qt include.
#include <QtCore/QList>
#include <QtCore/QStringList>


class QtArgCmdLine;


//
// QtArgCmdLineContext
//

//! Context of the command line.
class QtArgCmdLineContext {

		friend class QtArgCmdLine;

		static const QString m_null;

	public:
		//! \retval true if there are no more arguments.
		//! \retval false if there are one or more argument exists.
		bool atEnd() const
		{
			return ( m_current == m_argv.end() );
		}

		//! \return Next argument at command line.
		//! \retval Null QString if there are no more arguments at command line.
		const QString & next()
		{
			if( atEnd() )
				return m_null;

			return ( *( m_current++ ) );
		}

	protected:
		explicit QtArgCmdLineContext( const QStringList & argv )
			:	m_argv( argv )
			,	m_current( m_argv.begin() )
		{}

		//! Prepend context with new value.
		void prepend( const QString & value )
		{
			m_current = m_argv.insert( m_current, value );
		}


	private:
		//! Command line.
		QStringList m_argv;
		//! Current position.
		QStringList::Iterator m_current;
}; // class QtArgCmdLineContext

const QString QtArgCmdLineContext::m_null = QString::null;

#endif // QTARG__CMDLINECONTEXT_HPP__INCLUDED
