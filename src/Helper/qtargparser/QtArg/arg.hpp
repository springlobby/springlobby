
/*!
	\file
	\brief Command Line Argument.

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

#ifndef QTARG__ARG_HPP__INCLUDED
#define QTARG__ARG_HPP__INCLUDED

// Qt include.
#include <QtCore/QChar>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QRegExp>
#include <QtCore/QList>

// QtArg include.
#include "exceptions.hpp"
#include "cmdlinecontext.hpp"
#include "visitor.hpp"
#include "argconstraint.hpp"
#include "cmdlineiface.hpp"


class QtArgCmdLine;

//
// class QtArgIface
//

//! Interface of the argument.
/*!
	Should be implemented by every class wanted
	to be an argument.
*/
class QtArgIface {

	friend class QtArgCmdLine;

	public:
		//! Type of the list of argument's names.
		typedef QStringList NamesList;
		//! Type of the list of argument's flags.
		typedef QList< QChar > FlagsList;

	protected:
		QtArgIface()
		{}

		QtArgIface( const NamesList & names,
			const FlagsList & flags )
			:	m_flags( flags )
			,	m_names( names )
		{}

		virtual ~QtArgIface()
		{}

	public:
		/*!
			\retval true if argument is required.
			\retval false otherwise.
		*/
		virtual bool isRequired() const = 0;

		/*!
			\retval true if argument has decided he gas been defined.
			\retval false otherwise.
		*/
		virtual bool isDefined() const = 0;

		/*!
			Check correctness of the names, flags and so on before parsing.

			Should throw QtArgDissallowedFlagOrNameEx or
			QtArgNameOrFlagAlreadyKnownEx on errors.
		*/
		virtual void check(
			//! Delimiter.
			const QChar & delimiter,
			//! Already known flags.
			FlagsList & alreadyKnownFlags,
			//! Already known names.
			NamesList & alreadyKnownNames ) const = 0;

		//! Check correctness of the state of the argument after parsing.
		virtual void check() const = 0;

		//! \return All flags of the argument.
		virtual const FlagsList & flags() const
		{
			return m_flags;
		}

		//! \return All names of the argument.
		virtual const NamesList & names() const
		{
			return m_names;
		}

		//! \return Description of the argument.
		virtual QString description() const = 0;

		/*!
			\brief Should return pointer to the QtArgIface if this
			argument know how to do it, usually returns "this" if he does.

			\retval Pointer to the QtArgIface if flag \a flag determines
			this argument.
			\retval NULL if flag \a flag doesn't determine this argument.
		*/
		virtual QtArgIface * giveArgument( const QChar & flag ) = 0;

		/*!
			\brief Should return pointer to the QtArgIface if this
			argument know how to do it, usually returns "this" if he does.

			\retval Pointer to the QtArgIface if name \a name determines
			this argument.
			\retval NULL if name \a name doesn't determine this argument.
		*/
		virtual QtArgIface * giveArgument( const QString & name ) = 0;

		//! \return List of the arguments handled by this argument.
		/*!
			Usually returns list with "this" pointer.
		*/
		virtual const QtArgCmdLineIface::QtArgumentsList &
		arguments() const = 0;

		//! Check corrcetness of the argument constraint.
		/*!
			Should throw QtArgContraintNotObservedEx if constraint
			didn't observed.
		*/
		virtual void checkConstraint() const = 0;

	protected:
		//! Process argument.
		//! Can throw exceptions.
		//! \return The number of processed arguments.
		virtual int process( QtArgCmdLineContext & context ) = 0;

		//! Process with visitor.
		virtual void visit( QtArgCmdLineContext & context ) = 0;

	protected:
		//! Flag of the argument. If defined.
		FlagsList m_flags;
		//! List of the names of the argument.
		NamesList m_names;
}; // class QtArgIface


//
// class QtArg
//

//! Base class of the command line argument.
class QtArg
	:	public QtArgIface
{
	public:
		explicit QtArg(
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

		QtArg(
			//! Flag of the argument.
			const QChar & flag,
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

		explicit QtArg(
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

		explicit QtArg(
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

		explicit QtArg(
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

		QtArg(
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

		QtArg( const QtArg & arg );
		QtArg & operator = ( const QtArg & arg );

		virtual ~QtArg();

		//! \return Flag of the argument.
		virtual QChar flag() const;
		/*!
			Returns first value from the list of flags
			if defined more than one.
		*/
		//! Set new value for the flag of the argument.
		/*!
			Clears all previously defined flags from the list
			and set this new value as only one defined.
		*/
		virtual void setFlag( const QChar & fl );

		//! Set new value of the name of the argument.
		/*!
			Clears all previously defined names from the list
			and set this new value as only one defined.
		*/
		virtual void setFlags( const QtArgIface::FlagsList & f );

		//! \return Name of the argument.
		/*!
			Returns first value from the list of names
			if defined more than one.
		*/
		QString name() const;
		//! Set new value of the name of the argument.
		/*!
			Clears all previously defined names from the list
			and set this new value as only one defined.
		*/
		void setName( const QString & n );

		//! Set new value of the name of the argument.
		/*!
			Clears all previously defined names from the list
			and set this new value as only one defined.
		*/
		void setNames( const QtArgIface::NamesList & n );

		//! Set value of the description of the argument.
		//! Rewrites previously defined description.
		void setDescription( const QString & ds );

		//! Set value of the required property.
		void setRequired( bool on = true );

		//! \return Whether this argument should have a value or not.
		bool isWithValue() const;
		//! Set value of the property that hold whether this argument
		//! should have a value or not.
		void setWithValue( bool on = true );

		//! \return Visitor of this argument,
		//! NULL if visitor wasn't defined.
		QtArgVisitorIface * visitor() const;
		//! Set visitor to this argument.
		//! Replace previously defined visitor if defined.
		void setVisitor( QtArgVisitorIface * v );

		//! \return Constraint for the values,
		//! NULL if constraint wasn't defined.
		QtArgConstraintIface * constraint() const;
		//! Set constraint to this argument.
		//! Replace previously defined constraint if defined.
		void setConstraint( QtArgConstraintIface * c );

		//! \return Value of the argument.
		const QVariant & value() const;
		//! \return Value of the argument.
		QVariant & value();
		//! Set value of the argument.
		virtual void setValue( const QVariant & v );

		//! Set m_defined flag.
		void setDefined( bool on = true );

		//! \return true if argument was present in the command line.
		bool isPresent() const;
		//! Set m_present flag.
		void setPresent( bool on = true );

		//! \name Equal and not equal operators.
		//! \{
			friend bool operator == ( const QtArg & a1, const QtArg & a2 );
			friend bool operator != ( const QtArg & a1, const QtArg & a2 );
			friend bool operator == ( const QtArg & a1, const QString & a2 );
			friend bool operator != ( const QtArg & a1, const QString & a2 );
			friend bool operator == ( const QtArg & a1, const QChar & a2 );
			friend bool operator != ( const QtArg & a1, const QChar & a2 );
			friend bool operator == ( const QtArg & a1,
				const QtArgIface::FlagsList & a2 );
			friend bool operator != ( const QtArg & a1,
				const QtArgIface::FlagsList & a2 );
			friend bool operator == ( const QtArg & a1,
				const QtArgIface::NamesList & a2 );
			friend bool operator != ( const QtArg & a1,
				const QtArgIface::NamesList & a2 );
		//! \}

		//! \name QtArgIface implementation.
		//! \{

			/*!
				\retval true if argument is required.
				\retval false otherwise.
			*/
			virtual bool isRequired() const;

			/*!
				\retval true if argument has decided he gas been defined.
				\retval false otherwise.
			*/
			virtual bool isDefined() const;

			//! \throws QtArgDissallowedFlagOrNameEx if the flag is equal
			//! to the delimiter or one of the names is empty or equal
			//! to the space symbol.
			virtual void check(
				//! Delimiter.
				const QChar & delimiter,
				//! Already known flags.
				QtArgIface::FlagsList & alreadyKnownFlags,
				//! Already known names.
				QtArgIface::NamesList & alreadyKnownNames ) const;

			//! Check correctness of the state of the argument after parsing.
			virtual void check() const;

			//! \return Description of the argument.
			virtual QString description() const;

			/*!
				\brief Should return pointer to the QtArgIface if this
				argument know how to do it, usually returns "this" if he does.

				\retval Pointer to the QtArgIface if flag \a flag determines
				this argument.
				\retval NULL if flag \a flag doesn't determine this argument.
			*/
			virtual QtArgIface * giveArgument( const QChar & flag );

			/*!
				\brief Should return pointer to the QtArgIface if this
				argument know how to do it, usually returns "this" if he does.

				\retval Pointer to the QtArgIface if name \a name determines
				this argument.
				\retval NULL if name \a name doesn't determine this argument.
			*/
			virtual QtArgIface * giveArgument( const QString & name );

			//! \return List of the arguments handled by this argument.
			/*!
				Usually returns list with "this" pointer.
			*/
			virtual const QtArgCmdLineIface::QtArgumentsList &
			arguments() const;

			//! Check corrcetness of the argument constraint.
			/*!
				Should throw QtArgContraintNotObservedEx if constraint
				didn't observed.
			*/
			virtual void checkConstraint() const;

	protected:
			//! Process argument.
			//! Can throw exceptions.
			//! \return The number of processed arguments.
			virtual int process( QtArgCmdLineContext & context );

			//! Process with visitor.
			virtual void visit( QtArgCmdLineContext & context );

		//! \}

	private:
		//! Description of the argument.
		QString m_description;

		//! Is argument required?
		bool m_required;
		//! Should argument has a value?
		bool m_withValue;
		//! Is this argument and probaly his value defined.
		bool m_defined;
		//! Was this argument present in the command line.
		bool m_present;

		//! Value of the argument.
		QVariant m_value;

		//! Visitor of the argument.
		//! QtArg doesn't handle life cycle of the
		//! QtArgVisitor. So you must care about it.
		QtArgVisitorIface * m_visitor;

		//! Constraint foor the argument's values.
		QtArgConstraintIface * m_constraint;

		//! Arguments handled by this argument.
		QtArgCmdLineIface::QtArgumentsList m_args;
}; // class QtArg


//
// QtArg implementation.
//

inline
QtArg::QtArg(
	const QChar & flag,
	const QString & name,
	const QString & description,
	bool required,
	bool withValue,
	QtArgVisitorIface * visitor,
	QtArgConstraintIface * constraint )
		:	QtArgIface( name.length() ?
				QtArgIface::NamesList() << name : QtArgIface::NamesList(),
				QtArgIface::FlagsList() << flag )
		,	m_description( description )
		,	m_required( required )
		,	m_withValue( withValue )
		,	m_defined( false )
		,	m_present( false )
		,	m_visitor( visitor )
		,	m_constraint( constraint )
{
	m_args << this;
}

inline
QtArg::QtArg(
	const QChar & flag,
	const QtArgIface::NamesList & names,
	const QString & description,
	bool required,
	bool withValue,
	QtArgVisitorIface * visitor,
	QtArgConstraintIface * constraint )
		:	QtArgIface( names, QtArgIface::FlagsList() << flag )
		,	m_description( description )
		,	m_required( required )
		,	m_withValue( withValue )
		,	m_defined( false )
		,	m_present( false )
		,	m_visitor( visitor )
		,	m_constraint( constraint )
{
	m_args << this;
}

inline
QtArg::QtArg(
	const QString & name,
	const QString & description,
	bool required,
	bool withValue,
	QtArgVisitorIface * visitor,
	QtArgConstraintIface * constraint )
		:	QtArgIface( QtArgIface::NamesList() << name,
				QtArgIface::FlagsList() )
		,	m_description( description )
		,	m_required( required )
		,	m_withValue( withValue )
		,	m_defined( false )
		,	m_present( false )
		,	m_visitor( visitor )
		,	m_constraint( constraint )
{
	m_args << this;
}

inline
QtArg::QtArg(
	const QtArgIface::NamesList & names,
	const QString & description,
	bool required,
	bool withValue,
	QtArgVisitorIface * visitor,
	QtArgConstraintIface * constraint )
		:	QtArgIface( names, QtArgIface::FlagsList() )
		,	m_description( description )
		,	m_required( required )
		,	m_withValue( withValue )
		,	m_defined( false )
		,	m_present( false )
		,	m_visitor( visitor )
		,	m_constraint( constraint )
{
	m_args << this;
}

inline
QtArg::QtArg(
	const QtArgIface::FlagsList & flags,
	const QString & description,
	bool required,
	bool withValue,
	QtArgVisitorIface * visitor,
	QtArgConstraintIface * constraint )
		:	QtArgIface( QtArgIface::NamesList(), flags )
		,	m_description( description )
		,	m_required( required )
		,	m_withValue( withValue )
		,	m_defined( false )
		,	m_present( false )
		,	m_visitor( visitor )
		,	m_constraint( constraint )
{
	m_args << this;
}

inline
QtArg::QtArg(
	const QtArgIface::FlagsList & flags,
	const QtArgIface::NamesList & names,
	const QString & description,
	bool required,
	bool withValue,
	QtArgVisitorIface * visitor,
	QtArgConstraintIface * constraint )
		:	QtArgIface( names, flags )
		,	m_description( description )
		,	m_required( required )
		,	m_withValue( withValue )
		,	m_defined( false )
		,	m_present( false )
		,	m_visitor( visitor )
		,	m_constraint( constraint )
{
	m_args << this;
}

inline
QtArg::QtArg( const QtArg & arg )
	:	QtArgIface( arg.m_names, arg.m_flags )
	,	m_description( arg.m_description )
	,	m_required( arg.m_required )
	,	m_withValue( arg.m_withValue )
	,	m_defined( arg.m_defined )
	,	m_present( arg.m_present )
	,	m_value( arg.m_value )
	,	m_visitor( arg.m_visitor )
	,	m_constraint( arg.m_constraint )
{
	m_args << this;
}

inline QtArg &
QtArg::operator = ( const QtArg & arg )
{
	m_flags = arg.m_flags;
	m_names = arg.m_names;
	m_description = arg.m_description;
	m_required = arg.m_required;
	m_withValue = arg.m_withValue;
	m_visitor = arg.m_visitor;
	m_defined = arg.m_defined;
	m_present = arg.m_present;
	m_value = arg.m_value;
	m_constraint = arg.m_constraint;

	return *this;
}

inline
QtArg::~QtArg()
{
}

inline QChar
QtArg::flag() const
{
	if( !m_flags.isEmpty() )
		return m_flags.front();
	else
		return QChar();
}

inline void
QtArg::setFlag( const QChar & fl )
{
	m_flags.clear();
	m_flags << fl;
}

inline void
QtArg::setFlags( const QtArgIface::FlagsList & f )
{
	m_flags = f;
}

inline QString
QtArg::name() const
{
	if( !m_names.isEmpty() )
		return m_names.front();
	else
		return QString();
}

inline void
QtArg::setName( const QString & n )
{
	m_names.clear();
	m_names << n;
}

inline void
QtArg::setNames( const QtArgIface::NamesList & n )
{
	m_names = n;
}


inline QString
QtArg::description() const
{
	return m_description;
}

inline void
QtArg::setDescription( const QString & ds )
{
	m_description = ds;
}


inline bool
QtArg::isRequired() const
{
	return m_required;
}

inline void
QtArg::setRequired( bool on )
{
	m_required = on;
}

inline bool
QtArg::isWithValue() const
{
	return m_withValue;
}

inline void
QtArg::setWithValue( bool on )
{
	m_withValue = on;
}


inline QtArgVisitorIface *
QtArg::visitor() const
{
	return m_visitor;
}

inline void
QtArg::setVisitor( QtArgVisitorIface * v )
{
	m_visitor = v;
}

inline QtArgConstraintIface *
QtArg::constraint() const
{
	return m_constraint;
}

inline void
QtArg::setConstraint( QtArgConstraintIface * c )
{
	m_constraint = c;
}

inline const QVariant &
QtArg::value() const
{
	return m_value;
}

inline QVariant &
QtArg::value()
{
	return m_value;
}

inline void
QtArg::setValue( const QVariant & v )
{
	m_value = v;
	setDefined();
}

inline bool
QtArg::isDefined() const
{
	return m_defined;
}

inline void
QtArg::setDefined( bool on )
{
	m_defined = on;

	setPresent();
}

inline bool
QtArg::isPresent() const
{
	return m_present;
}

inline void
QtArg::setPresent( bool on )
{
	m_present = on;
}

inline QtArgIface *
QtArg::giveArgument( const QChar & f )
{
	if( !m_flags.isEmpty() )
		foreach( QChar flag, m_flags )
			if( flag == f )
				return this;

	return NULL;
}

inline QtArgIface *
QtArg::giveArgument( const QString & n )
{
	if( m_names.size() )
		foreach( QString name, m_names )
			if( name.startsWith( n ) )
				return this;

	return NULL;
}

inline const QtArgCmdLineIface::QtArgumentsList &
QtArg::arguments() const
{
	return m_args;
}

inline void
QtArg::check(
	const QChar & delimiter,
	QtArgIface::FlagsList & alreadyKnownFlags,
	QtArgIface::NamesList & alreadyKnownNames ) const
{
	if( m_flags.isEmpty() && m_names.isEmpty() )
		throw QtArgNotDefinedNorNameNorFlagEx();

	if( m_flags.size() )
	{
		foreach( QChar flag, m_flags )
		{
			if( delimiter == flag || !flag.isLetterOrNumber() )
				throw QtArgDissallowedFlagOrNameEx(
					QString::fromLatin1( "Dissallowed flag: \"%1\"." )
						.arg( flag ) );

			if( alreadyKnownFlags.contains( flag ) )
				throw QtArgNameOrFlagAlreadyKnownEx(
					QString::fromLatin1( "Flag already in use: %1." )
						.arg( flag ) );

			alreadyKnownFlags.append( flag );
		}
	}

	if( m_names.size() )
	{
		foreach( QString name, m_names )
		{
			if( name.isEmpty() ||
				name.contains( QRegExp( QString::fromLatin1( "\\s" ) ) ) )
					throw QtArgDissallowedFlagOrNameEx(
						QString::fromLatin1( "Dissallowed name: \"%1\"." )
							.arg( name ) );

			if( alreadyKnownNames.contains( name ) )
				throw QtArgNameOrFlagAlreadyKnownEx(
					QString::fromLatin1( "Name already in use: %1." )
						.arg( name ) );

			alreadyKnownNames.append( name );
		}
	}
}

inline void
QtArg::check() const
{
	if( !isDefined() )
	{
		if( isRequired() && !isPresent() )
			throw QtArgNotDefinedMandatoryArgumentEx(
				QString::fromLatin1( "Not defined mandatory argument: %1" )
					.arg( names().size() ? names().front() : flags().front() ) );

		if( isPresent() && isWithValue() )
			throw QtArgNotDefinedOptionForTheArgumentEx(
				QString::fromLatin1( "Not defined value of the argument: %1" )
					.arg( names().size() ? names().front() : flags().front() ) );
	}
}

inline int
QtArg::process( QtArgCmdLineContext & context )
{
	if( isWithValue() )
	{
		if( !context.atEnd() )
		{
			setValue( context.next() );

			return 1;
		}
		else
			setPresent();
	}
	else
		setValue( QVariant( true ) );

	return 0;
}

inline void
QtArg::visit( QtArgCmdLineContext & context )
{
	if( visitor() ) visitor()->visit( context );
}

inline void
QtArg::checkConstraint() const
{
	if( m_constraint && isDefined() )
		if( !m_constraint->check( m_value ) )
			throw QtArgContraintNotObservedEx(
				QString::fromLatin1( "Constraint for the argument: %1"
				" hasn't observed. Wrong value is: %2" )
					.arg( names().size() ? names().front() : flags().front() )
					.arg( m_value.toString() ) );
}

namespace /* anonymous */ {

	inline bool
	isNamesEqual( const QtArgIface::NamesList & l1,
		const QtArgIface::NamesList & l2 )
	{
		if( l1.size() == l2.size() )
		{
			foreach( QString name, l1 )
				if( !l2.contains( name ) )
					return false;
		}
		else
			return false;

		return true;
	}

	inline bool
	isFlagsEqual( const QtArgIface::FlagsList & f1,
		const QtArgIface::FlagsList & f2 )
	{
		if( f1.size() == f2.size() )
		{
			foreach( QChar flag, f1 )
				if( !f2.contains( flag ) )
					return false;
		}
		else
			return false;

		return true;
	}

} /* namespace anonymous */

inline bool operator == ( const QtArg & a1, const QtArg & a2 )
{
	if( a1.m_flags.isEmpty() && a2.m_flags.isEmpty() )
		return isNamesEqual( a1.m_names, a2.m_names );
	else if( !a1.m_flags.isEmpty() && !a2.m_flags.isEmpty() )
	{
		if( isFlagsEqual( a1.m_flags, a2.m_flags ) )
			return isNamesEqual( a1.m_names, a2.m_names );
		else
			return false;
	}
	else
		return false;
}

inline bool operator != ( const QtArg & a1, const QtArg & a2 )
{
	return !( a1 == a2 );
}

inline bool operator == ( const QtArg & a1, const QString & a2 )
{
	QtArg tmp = a1;
	tmp.setName( a2 );

	return ( a1 == tmp );
}

inline bool operator != ( const QtArg & a1, const QString & a2 )
{
	QtArg tmp = a1;
	tmp.setName( a2 );

	return ( a1 != tmp );
}

inline bool operator == ( const QtArg & a1, const QChar & a2 )
{
	QtArg tmp = a1;
	tmp.setFlag( a2 );

	return ( a1 == tmp );
}

inline bool operator != ( const QtArg & a1, const QChar & a2 )
{
	QtArg tmp = a1;
	tmp.setFlag( a2 );

	return ( a1 != tmp );
}

inline bool operator == ( const QtArg & a1, const QtArgIface::FlagsList & a2 )
{
	QtArg tmp = a1;
	tmp.setFlags( a2 );

	return ( a1 == tmp );
}

inline bool operator != ( const QtArg & a1, const QtArgIface::FlagsList & a2 )
{
	QtArg tmp = a1;
	tmp.setFlags( a2 );

	return ( a1 != tmp );
}

inline bool operator == ( const QtArg & a1, const QtArgIface::NamesList & a2 )
{
	QtArg tmp = a1;
	tmp.setNames( a2 );

	return ( a1 == tmp );
}

inline bool operator != ( const QtArg & a1, const QtArgIface::NamesList & a2 )
{
	QtArg tmp = a1;
	tmp.setNames( a2 );

	return ( a1 != tmp );
}

#endif // QTARG__ARG_HPP__INCLUDED
