#ifndef TDFCONTAINER_H
#define TDFCONTAINER_H

#include <wx/string.h>
#include <wx/colour.h>
#include <wx/log.h>
//#include "utils.h"
#include "utils/conversion.h"

class wxColour;
/// Todo: add TDFContainer class.
///

class TDFWriter
{
	public:
		TDFWriter( wxString &s );
		~TDFWriter();
		void EnterSection( const wxString &name );
		void LeaveSection();
		void Indent();
		wxString GetCurrentPath();
		void Append( const wxString &name, wxString value );
		template<class T>
		void Append( const wxString &name, T value );

		/// works like algorithms such as std::sort
		template<class T> void Append( const wxString &name, T begin, T end );

		void AppendLineBreak();
		void Close();
	protected:
	private:
		wxString &m_stream;
		int m_depth;
};

///
/// Parsing classes contributed by dmytry aka dizekat, copypasted from personal
/// project: render_control_new/utils/parsing.h and cpp , database.h and cpp
///

#include <sstream>
#include <vector>
#include <deque>
#include <map>

#include "autopointers.h"
#include "utils/mixins.hh"

class Tokenizer;

namespace SL {
class Node;
typedef RefcountedPointer<Node> PNode;
class DataList;
typedef RefcountedPointer<DataList> PDataList;
class DataLeaf;
typedef RefcountedPointer<DataLeaf> PDataLeaf;

class Node: public RefcountedContainer , public SL::NonCopyable
{
		friend class DataList;
	protected:
		DataList *parent;
		Node *list_prev;
		Node *list_next;
		void ListRemove();
		void ListInsertAfter( Node *what );
		wxString name;
	public:

		wxString Name();

		// Sets the name, and updates parent if present
		bool SetName( const wxString &name_ );
		Node(): parent( NULL ), list_prev( NULL ), list_next( NULL ), name( wxEmptyString ) {}
		virtual ~Node();
		DataList* Parent() const;// parent list
		//void SetParent(DataList *parent_);
		bool IsChildOf( DataList *what ) const;

		virtual void Save( TDFWriter &f );
		virtual void Load( Tokenizer &f );
};


/// Usage
/// Parsing:

/// int errs=0;
/// PDataList root(ParseTDF(some istream, &errs));// errs is optional, gives number of errors when loading.

/// Getting values:

/// PDataList game(root->Find(_T("GAME")))
/// if(!game.ok()){wxLogMessage(_T("Game tag is missing"));return false;}
/// wxString gamename=game->GetString(_T("Mapname"));

/// (see optional parameters for setting default and knowing if it failed)
/// and so on and so forth.
/// Saving tdf:
///


class DataList: public Node {
	protected:
		std::map<wxString, PNode> nodes;
		Node list_loop;// next is first, prev is last in the list
		typedef std::map<wxString, PNode>::iterator nodes_iterator;
	public:
		DataList();
		~DataList();

		bool Insert( PNode node );// return false if such entry already exists.
		bool InsertAt( PNode node, PNode where );// return false if such entry already exists. Inserts right before given

		// rename if such entry already exists. Names is like name!1 or name!2 etc
		void InsertRename( PNode node );
		void InsertRenameAt( PNode node, PNode where );
		bool Remove( const wxString &str );
		bool Remove( PNode node );
		bool Rename( const wxString &old_name, const wxString &new_name );
		PNode Find( const wxString &str );// find by name

		wxString Path();

		PNode FindByPath( const wxString &str );

		PNode Next( PNode what );
		PNode Prev( PNode what );
		PNode End();
		PNode First();
		PNode Last();

		virtual void Save( TDFWriter &f );
		virtual void Load( Tokenizer &f );

		int GetInt( const wxString &name, int default_value = 0, bool *it_worked = NULL );
		double GetDouble( const wxString &name, double default_value = 0, bool *it_worked = NULL );
		wxString GetString( const wxString &name, const wxString &default_value = wxString(), bool *it_worked = NULL );
		/// returns number of values that were successfully read
		int GetDoubleArray( const wxString &name, int n_values, double *values );

		wxColour GetColour( const wxString &name, const wxColour &default_value = wxColour( 0, 0, 0 ), bool *it_worked = NULL );

};

class DataLeaf: public Node {
		friend class DataList;
	protected:
		wxString value;
	public:
		wxString GetValue();
		void SetValue( const wxString &value );

		virtual void Save( TDFWriter &f );
		virtual void Load( Tokenizer &f );
};
} // end namespace SL

inline bool IsLetter( char c ) {
	return ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || ( c == '_' );
}
inline bool IsNum( char c ) {
	return c >= '0' && c <= '9';
}
inline bool IsNumDot( char c ) {
	return ( c >= '0' && c <= '9' ) || c == '.';
}
inline bool IsAlphaNum( char c ) {
	return IsLetter( c ) || IsNum( c );
}
inline bool IsWhitespace( char c ) {
	return ( c == ' ' ) || ( c == 10 ) || ( c == 13 ) || ( c == '\t' );
}
struct Token {
	enum TokenType {
		type_none,
		type_section_name,
		type_enter_section,
		type_leave_section,
		type_entry_name,
		type_entry_value,
		type_semicolon,
		type_eof
	};
	TokenType type;
	wxString value_s;

	wxString pos_string;// for error reporting

	bool IsEOF() const {
		return ( type == type_eof );
	}
	Token(): type( type_eof )
	{
	}

};

class Tokenizer {
		/// todo: clean up, move to CPP file

		/// simple reference counted pointer to stream.
		struct IncludeCacheEntry {
			wxString name; ///< used for error reporting
			int line;
			int column;
			std::istream *stream;
			//bool must_delete;
			int *refcount;

			IncludeCacheEntry( std::istream *stream_, bool must_delete_ = false ):
					line( 1 ),
					column( 1 ),
					stream( stream_ ),
					refcount( NULL )
			{
				if ( must_delete_ ) {
					refcount = new int;
					( *refcount ) = 1;
				}
			}
			IncludeCacheEntry( const IncludeCacheEntry &other ):
					line( other.line ),
					column( other.column ),
					stream( other.stream ),
					refcount( other.refcount )
			{
				stream = other.stream;
				refcount = other.refcount;
				if ( refcount )( *refcount ) += 1;
			}
			~IncludeCacheEntry()
			{
				if ( refcount ) {
					( *refcount )--;
					if ( ( *refcount ) <= 0 ) {
						delete stream;
						delete refcount;
					}
				}
			}
		};
		std::vector<IncludeCacheEntry> include_stack;
		void UnwindStack();

		std::deque<Token> token_buffer;
		//size_t max_buffer_size;
		size_t buffer_pos;

		bool skip_eol;
		char GetNextChar();
		char PeekNextChar();

		void ReadToken( Token &token );
		void SkipSpaces();

		int errors;

	public:
		Tokenizer(): buffer_pos( 0 ), skip_eol( false ), errors( 0 )
		{
		}

		void EnterStream( std::istream &stream_, const wxString &name = _T( "" ) );

		Token GetToken( int i = 0 );
		void Step( int i = 1 );
		inline Token TakeToken() {
			Token result = GetToken();
			Step();
			return result;
		}

		bool Good();

		void ReportError( const Token &t, const wxString &err );

		int NumErrors() const {
			return errors;
		}
};

inline Tokenizer &operator >>( Tokenizer &tokenizer, Token &token ) {
	token = tokenizer.TakeToken();
	return tokenizer;
}

SL::PDataList ParseTDF( std::istream &s, int *error_count = NULL );

//Defintions to not clutter up the class declaration
template<class T> void TDFWriter:: Append( const wxString &name, T value )
{
	Append( name, TowxString( value ) );
}

template<class T>
void TDFWriter::Append( const wxString &name, T begin, T end ) {
	Indent();
	m_stream << name << _T( "=" );
	for ( T it = begin;it != end;++it ) {
		if ( it != begin )m_stream << _T( " " );
		m_stream << ( *it );
	}
	m_stream << _T( ";\n" );
}

#endif // TDFCONTAINER_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

