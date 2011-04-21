
/*!
	\file
	\brief Command line argument with multiply values.

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

#ifndef QTARG__MULTIARG_HPP__INCLUDED
#define QTARG__MULTIARG_HPP__INCLUDED

// Qt include.
#include <QtCore/QList>
#include <QtCore/QVariant>

// QtArg include.
#include "arg.hpp"


//
// class QtMultiArg
//

//! Command line argument with multiply values.
/*!
	Arguments based on this class can be specified multiply
	times in command line and value for such arguments is
	a QList< QVariant >.
*/
class QtMultiArg
	:	public QtArg
{
	public:
		explicit QtMultiArg(
			//! Flag of the argument.
			const QChar & flag,
			//! Name of the argument.
			const QString & name = QString(),
			//! Description of the argument.
			const QString & description = QString(),
			//! Is argument required?
			bool required = false,
			//! Should argument has a value?
			bool withValue = false,
			//! Visitor of the argument.
			QtArgVisitorIface * visitor = NULL,
			//! Constraint for the argument's values.
			QtArgConstraintIface * constraint = NULL );

		QtMultiArg(
			//! Flag of the argument.
			const QChar & flag,
			//! Various names of the argument.
			const QStringList & names,
			//! Description of the argument.
			const QString & description = QString(),
			//! Is argument required?
			bool required = false,
			//! Should argument has a value?
			bool withValue = false,
			//! Visitor of the argument.
			QtArgVisitorIface * visitor = NULL,
			//! Constraint for the argument's values.
			QtArgConstraintIface * constraint = NULL );

		explicit QtMultiArg(
			//! Name of the argument.
			const QString & name,
			//! Description of the argument.
			const QString & description = QString(),
			//! Is argument required?
			bool required = false,
			//! Should argument has a value?
			bool withValue = false,
			//! Visitor of the argument.
			QtArgVisitorIface * visitor = NULL,
			//! Constraint for the argument's values.
			QtArgConstraintIface * constraint = NULL );

		explicit QtMultiArg(
			//! Various names of the argument.
			const QtArgIface::NamesList & names,
			//! Description of the argument.
			const QString & description = QString(),
			//! Is argument required?
			bool required = false,
			//! Should argument has a value?
			bool withValue = false,
			//! Visitor of the argument.
			QtArgVisitorIface * visitor = NULL,
			//! Constraint for the argument's values.
			QtArgConstraintIface * constraint = NULL );

		explicit QtMultiArg(
			//! Various flags of the argument.
			const QtArgIface::FlagsList & flags,
			//! Description of the argument.
			const QString & description = QString(),
			//! Is argument required?
			bool required = false,
			//! Should argument has a value?
			bool withValue = false,
			//! Visitor of the argument.
			QtArgVisitorIface * visitor = NULL,
			//! Constraint for the argument's values.
			QtArgConstraintIface * constraint = NULL );

		QtMultiArg(
			//! Various flags of the argument.
			const QtArgIface::FlagsList & flags,
			//! Various names of the argument.
			const QtArgIface::NamesList & names,
			//! Description of the argument.
			const QString & description = QString(),
			//! Is argument required?
			bool required = false,
			//! Should argument has a value?
			bool withValue = false,
			//! Visitor of the argument.
			QtArgVisitorIface * visitor = NULL,
			//! Constraint for the argument's values.
			QtArgConstraintIface * constraint = NULL );

		QtMultiArg( const QtArg & arg );
		QtMultiArg & operator = ( const QtArg & arg );

		QtMultiArg( const QtMultiArg & arg );
		QtMultiArg & operator = ( const QtMultiArg & arg );

		virtual ~QtMultiArg();

		//! Set value of the argument.
		virtual void setValue( const QVariant & v );

		//! \name QtArgIface implementation.
		//! \{

			//! Check corrcetness of the argument constraint.
			/*!
				Should throw QtArgContraintNotObservedEx if constraint
				didn't observed.
			*/
			virtual void checkConstraint() const;

		//! \}
}; // class QtMultiArg


//
// QtMultiArg implementation.
//

inline
QtMultiArg::QtMultiArg(
	const QChar & flag,
	const QString & name,
	const QString & description,
	bool required,
	bool withValue,
	QtArgVisitorIface * visitor,
	QtArgConstraintIface * constraint )
		:	QtArg( flag, name, description,
				   required, withValue, visitor, constraint )
{
}

inline
QtMultiArg::QtMultiArg(
	const QChar & flag,
	const QStringList & names,
	const QString & description,
	bool required,
	bool withValue,
	QtArgVisitorIface * visitor,
	QtArgConstraintIface * constraint )
		:	QtArg( flag, names, description,
				   required, withValue, visitor, constraint )
{
}

inline
QtMultiArg::QtMultiArg(
	const QString & name,
	const QString & description,
	bool required,
	bool withValue,
	QtArgVisitorIface * visitor,
	QtArgConstraintIface * constraint )
		:	QtArg( name, description,
				   required, withValue, visitor, constraint )
{
}

inline
QtMultiArg::QtMultiArg(
	const QtArgIface::NamesList & names,
	const QString & description,
	bool required,
	bool withValue,
	QtArgVisitorIface * visitor,
	QtArgConstraintIface * constraint )
		:	QtArg( names, description,
				   required, withValue, visitor, constraint )
{
}

inline
QtMultiArg::QtMultiArg(
	const QtArgIface::FlagsList & flags,
	const QString & description,
	bool required,
	bool withValue,
	QtArgVisitorIface * visitor,
	QtArgConstraintIface * constraint )
		:	QtArg( flags, description,
				   required, withValue, visitor, constraint )
{
}

inline
QtMultiArg::QtMultiArg(
	const QtArgIface::FlagsList & flags,
	const QtArgIface::NamesList & names,
	const QString & description,
	bool required,
	bool withValue,
	QtArgVisitorIface * visitor,
	QtArgConstraintIface * constraint )
		:	QtArg( flags, names, description,
				   required, withValue, visitor, constraint )
{
}

inline
QtMultiArg::QtMultiArg( const QtArg & arg )
	:	QtArg( arg )
{
}

inline QtMultiArg &
QtMultiArg::operator = ( const QtArg & arg )
{
	QtArg::operator = ( arg );

	return *this;
}

inline
QtMultiArg::QtMultiArg( const QtMultiArg & arg )
	:	QtArg( arg )
{
}

inline QtMultiArg &
QtMultiArg::operator = ( const QtMultiArg & arg )
{
	QtArg::operator = ( arg );

	return *this;
}

inline
QtMultiArg::~QtMultiArg()
{
}

inline void
QtMultiArg::setValue( const QVariant & v )
{
	QList< QVariant > values = value().toList();
	values.append( v );

	QtArg::setValue( values );
}

inline void
QtMultiArg::checkConstraint() const
{
	if( isDefined() )
	{
		QList< QVariant > values = value().toList();

		foreach( QVariant v, values )
			if( constraint() && !constraint()->check( v ) )
				throw QtArgContraintNotObservedEx(
					QString::fromLatin1( "Constraint for the argument: %1"
					" hasn't observed. Wrong value is: %2" )
						.arg( names().size() ? names().front() : flags().front() )
						.arg( v.toString() ) );
	}
}

#endif // QTARG__MULTIARG_HPP__INCLUDED
