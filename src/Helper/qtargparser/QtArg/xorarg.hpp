
/*!
	\file
	\brief Command line argument with under the XOR mask.

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

#ifndef QTARG__XORARG_HPP__INCLUDED
#define QTARG__XORARG_HPP__INCLUDED

// QtArg include.
#include "arg.hpp"
#include "exceptions.hpp"


//
// Exceptions.
//

//
// QtArgXORMaskNotObservedEx
//

//! Throws when user defines more than one XORed argument.
class QtArgXORMaskNotObservedEx
	:	public QtArgBaseException
{
	public:
		explicit QtArgXORMaskNotObservedEx( const QString & desc )
			:	QtArgBaseException( desc )
		{}

		virtual ~QtArgXORMaskNotObservedEx() throw()
		{}
}; // class QtArgXORMaskNotObservedEx


//
// QtRequiredArgumentUnderXORMaskEx
//

//! Throws when user tries to add required argument under the XOR mask.
class QtRequiredArgumentUnderXORMaskEx
	:	public QtArgBaseException
{
	public:
		explicit QtRequiredArgumentUnderXORMaskEx( const QString & desc )
			:	QtArgBaseException( desc )
		{}

		virtual ~QtRequiredArgumentUnderXORMaskEx() throw()
		{}
}; // class QtArgXORMaskNotObservedEx


//
// class QtXorArg
//

//! Command line argument under the XOR mask.
/*!
	This class help to specify two or more arguments
	as a set of arguments in wich only one can be defined
	in the command line.

	Adding required argument under the XOR mask is dissallowed.
*/
class QtXorArg
	:	public QtArgIface
{
	public:
		QtXorArg( QtArgIface & arg1,
			QtArgIface & arg2,
			const QString & name,
			bool required = false );

		QtXorArg( const QtArgCmdLineIface::QtArgumentsList & args,
			const QString & name,
			bool required = false );

		QtXorArg( const QtXorArg & arg );
		QtXorArg & operator = ( const QtXorArg & arg );

		virtual ~QtXorArg();

		//! Add argument under the XOR mask.
		void add( QtArgIface & arg );

		//! Add argument under the XOR mask.
		void add( QtArgIface * arg );

		//! Set required flag.
		void setRequired( bool on = true );

		//! \name QtArgIface implemenation.
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

			//! \return Description of the argument.
			virtual QString description() const;

			/*!
				Check correctness of the names, flags and so on before parsing.

				Should throw QtArgDissallowedFlagOrNameEx or
				QtArgNameOrFlagAlreadyKnownEx.
			*/
			virtual void check(
				//! Delimiter.
				const QChar & delimiter,
				//! Already known flags.
				QtArgIface::FlagsList & alreadyKnownFlags,
				//! Already known names.
				QtArgIface::NamesList & alreadyKnownNames ) const;

			//! Check correctness of the state of the argument after parsing.
			virtual void check() const;

			/*!
				\brief Should return pointer to the QtArgIface if this
				argument know how to do it, as usual returns "this" if he does.

				\retval Pointer to the QtArgIface if flag \a flag determines
				this argument.
				\retval NULL if flag \a flag doesn't determine this argument.
			*/
			virtual QtArgIface * giveArgument( const QChar & flag );

			/*!
				\brief Should return pointer to the QtArgIface if this
				argument know how to do it, as usual returns "this" if he does.

				\retval Pointer to the QtArgIface if name \a name determines
				this argument.
				\retval NULL if name \a name doesn't determine this argument.
			*/
			virtual QtArgIface * giveArgument( const QString & name );

			//! \return List of the arguments handled by this argument.
			/*!
				Usually returns list with "this" pointer.
			*/
			virtual const QtArgCmdLineIface::QtArgumentsList & arguments() const;

			//! Check corrcetness of the argument constraint.
			/*!
				Should throw QtArgContraintNotObservedEx if constraint
				didn't observed.
			*/
			virtual void checkConstraint() const;

	protected:
			//! Process argument.
			//! \return The number of processed arguments.
			virtual int process( QtArgCmdLineContext & context );

			//! Process with visitor.
			virtual void visit( QtArgCmdLineContext & context );

		//! \}

	private:
		//! XORed arguments.
		QtArgCmdLineIface::QtArgumentsList m_args;
		//! Is argument required?
		bool m_required;
		//! Name of the argument.
		QString m_name;
}; // class QtXorArg


//
// QtXorArg implementation.
//

inline
QtXorArg::QtXorArg( QtArgIface & arg1,
	QtArgIface & arg2,
	const QString & name,
	bool required )
	:	QtArgIface()
	,	m_required( required )
	,	m_name( name )
{
	add( arg1 );
	add( arg2 );
}

inline
QtXorArg::QtXorArg( const QtArgCmdLineIface::QtArgumentsList & args,
	const QString & name,
	bool required )
	:	m_required( required )
	,	m_name( name )
{
	foreach( QtArgIface * arg, args )
		add( arg );
}

inline
QtXorArg::QtXorArg( const QtXorArg & arg )
	:	QtArgIface()
	,	m_args( arg.arguments() )
	,	m_required( arg.m_required )
	,	m_name( arg.m_name )
{
}

inline QtXorArg &
QtXorArg::operator = ( const QtXorArg & arg )
{
	m_args = arg.arguments();
	m_required = arg.isRequired();
	m_name = arg.m_name;

	return *this;
}

inline
QtXorArg::~QtXorArg()
{
}

inline void
QtXorArg::add( QtArgIface & arg )
{
	add( &arg );
}

inline void
QtXorArg::add( QtArgIface * arg )
{
	if( arg && !m_args.contains( arg ) )
	{
		if( !arg->isRequired() )
				m_args.append( arg );
		else
			throw QtRequiredArgumentUnderXORMaskEx(
				QString::fromLatin1( "A try to add required argument under the XOR mask." ) );
	}
}

inline bool
QtXorArg::isRequired() const
{
	return m_required;
}

inline void
QtXorArg::setRequired( bool on )
{
	m_required = on;
}

inline bool
QtXorArg::isDefined() const
{
	bool defined = false;

	foreach( QtArgIface * arg, m_args )
		if( arg->isDefined() )
			defined = true;

	return defined;
}

inline void
QtXorArg::check(
	const QChar & delimiter,
	QtArgIface::FlagsList & alreadyKnownFlags,
	QtArgIface::NamesList & alreadyKnownNames ) const
{
	foreach( QtArgIface * arg, m_args )
		arg->check( delimiter, alreadyKnownFlags, alreadyKnownNames );
}

inline void
QtXorArg::check() const
{
	if( isRequired() && !isDefined() )
		throw QtArgNotDefinedMandatoryArgumentEx(
				QString::fromLatin1( "Not defined mandatory argument: %1" )
					.arg( m_name ) );

	QString definedArgumentName;

	foreach( QtArgIface * arg, m_args )
	{
		arg->check();

		if( arg->isDefined() && !definedArgumentName.isEmpty() )
			throw QtArgXORMaskNotObservedEx(
				QString::fromLatin1( "Defined more than one argument under XOR mask."
					" First defined argument: %1. Second defined argument: %2." )
						.arg( definedArgumentName )
						.arg( arg->names().size() ? arg->names().front()
							  : arg->flags().front() ) );

		if( arg->isDefined() )
			definedArgumentName =
				( arg->names().size() ? arg->names().front()
					: arg->flags().front() );
	}
}

inline QString
QtXorArg::description() const
{
	return QString::null;
}

inline QtArgIface *
QtXorArg::giveArgument( const QChar & flag )
{
	foreach( QtArgIface * arg, m_args )
	{
		QtArgIface * ret = arg->giveArgument( flag );

		if( ret != NULL )
			return ret;
	}

	return NULL;
}

inline QtArgIface *
QtXorArg::giveArgument( const QString & name )
{
	foreach( QtArgIface * arg, m_args )
	{
		QtArgIface * ret = arg->giveArgument( name );

		if( ret != NULL )
			return ret;
	}

	return NULL;
}

inline const QtArgCmdLineIface::QtArgumentsList &
QtXorArg::arguments() const
{
	return m_args;
}

inline int
QtXorArg::process( QtArgCmdLineContext & )
{
	return 0;
}

inline void
QtXorArg::visit( QtArgCmdLineContext & )
{
}

inline void
QtXorArg::checkConstraint() const
{
	if( isDefined() )
	{
		foreach( QtArgIface * arg, m_args )
			arg->checkConstraint();
	}
}

#endif // QTARG__XORARG_HPP__INCLUDED
