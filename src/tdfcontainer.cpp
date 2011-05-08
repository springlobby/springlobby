#include "tdfcontainer.h"

#include "utils/conversion.h"
#include "utils/debug.h"
#include <wx/intl.h>
#include <wx/tokenzr.h>
#include <wx/log.h>
#include <cmath>

TDFWriter::TDFWriter( wxString &s ):
		m_stream( s ),
		m_depth( 0 )
{

}

TDFWriter::~TDFWriter() {
	Close();
}
void TDFWriter::EnterSection( const wxString &name ) {
	Indent();
	m_stream << _T( "[" ) << name << _T( "]\n" );
	Indent();
	m_stream << _T( "{\n" );
	m_depth++;
}
void TDFWriter::LeaveSection() {
	m_depth--;
	Indent();
	m_stream << _T( "}\n" );
}
void TDFWriter::Indent() {
	for ( int i = 0;i < m_depth;++i )m_stream << _T( "\t" );
}
//wxString GetCurrentPath();
void TDFWriter::Append( const wxString &name, wxString value ) {
	Indent();
	m_stream << name << _T( "=" ) << value << _T( ";\n" );
}

void TDFWriter::Close() {
	while ( m_depth > 0 )
		LeaveSection();
	if ( m_depth < 0 ) {
		wxLogWarning( _T( "error in TDFWriter usage: more LeaveSection() calls than EnterSection(). Please contact springlobby developers" ) );
	}
}

void TDFWriter::AppendLineBreak() {
	m_stream << _T( "\n" );
}

#include "sstream"
#include "fstream"
#include <iomanip>
#include <algorithm>


void Tokenizer::ReportError( const Token &t, const wxString &err ) {
	wxLogMessage( _T( "TDF parsing error at (%s), on token \"%s\" : %s" ), t.pos_string.c_str(), t.value_s.c_str(), err.c_str() );
	errors++;
}

//#define ReportError(a) {std::cerr<<"error "<<(a);}
//#define ReportError(a) {wxLogMessage(_T("error %s"),TowxString(a).c_str());}

void Tokenizer::EnterStream( std::istream &stream_, const wxString &name ) {
	skip_eol = false;
	include_stack.push_back( IncludeCacheEntry( &stream_, false ) );
	include_stack.back().name = name;
}

void Tokenizer::UnwindStack() {
	while ( ( !include_stack.empty() ) && include_stack.back().stream && ( !include_stack.back().stream->good() ) ) {
		include_stack.pop_back();
	}
}

char Tokenizer::PeekNextChar() {
	UnwindStack();
	//std::string tmp;
	char tmp = 0;
	if ( !include_stack.empty() )tmp = ( *include_stack.back().stream ).peek();
	return tmp;
}

char Tokenizer::GetNextChar() {
	UnwindStack();
	//std::string tmp;
	if ( include_stack.empty() )return 0;

	char c = ( *include_stack.back().stream ).get();

	if ( ( !skip_eol ) && ( c == 10 || c == 13 ) ) {// end of line
		include_stack.back().line += 1;
		include_stack.back().column = 1;

		// if next is different we'll skip it.
		//std::istream &stream=(*include_stack.back().stream);
		char nc = ( *include_stack.back().stream ).peek();
		if ( ( nc == 10 || nc == 13 ) && ( nc != c ) )skip_eol = true;
	} else {
		if ( !skip_eol )include_stack.back().column += 1;
		skip_eol = false;
	}

	return c;
}

bool Tokenizer::Good() {
	UnwindStack();
	return !include_stack.empty();
}

void Tokenizer::ReadToken( Token &token ) {
start:

	SkipSpaces();


	token.value_s.clear();

	if ( !Good() ) {
		token.type = Token::type_eof;
		token.pos_string = _T( "EOF" );
		return;
	}

	token.pos_string = wxString();
	if ( !include_stack.empty() && !include_stack.back().name.empty() )token.pos_string << include_stack.back().name << _T( " , " );
	token.pos_string << _( "line " ) << include_stack.back().line << _( " , column " ) << include_stack.back().column;

	char c = GetNextChar();
	token.value_s += c;
	// first find what token is it, and handle all except numbers
	switch ( c ) {
		case '[': {
				token.type = Token::type_section_name;
				token.value_s.clear();
				bool skip_next_eol_char = false;
				while ( Good() ) {
					c = GetNextChar();
					// wxString has problem with zero characters, replace by space.
					if ( c == 0 ) {
						c = ' ';
					}
					if ( c == '\\' ) {
						if ( Good() ) {
							token.value_s += GetNextChar();
						} else {
							ReportError( token, wxT( "Quotes not closed before end of file" ) );
							return;
						}
					} else if ( c == ']' ) {
						return;
					} else
					{
						token.value_s += c;
					}
					// handle end of line
					if ( skip_next_eol_char ) {
						skip_next_eol_char = false;
					} else if ( c == 10 || c == 13 ) {
						//++line;
						//column=1;
						if ( ( PeekNextChar() == 10 || PeekNextChar() == 13 ) && ( PeekNextChar() != c ) )skip_next_eol_char = true;
					}
				}
				ReportError( token, wxT( "Quotes not closed before end of file" ) );
			}
		case '{':
			token.type = Token::type_enter_section;
			return;
		case '}':
			token.type = Token::type_leave_section;
			return;
		case ';':
			token.type = Token::type_semicolon;
			return;
		case '=':
			token.type = Token::type_entry_value;
			token.value_s = _T( "" );
			while ( Good() && PeekNextChar() != ';' ) {
				unsigned char c_ = GetNextChar();
				token.value_s += c_;
			}
			return;
		case '/':// handle comments
			if ( PeekNextChar() == '/' ) {
				//SkipToEOL();
				if ( !include_stack.empty() ) {
					std::string tmp;
					std::getline( ( *include_stack.back().stream ), tmp );
					include_stack.back().line += 1;
					include_stack.back().column = 1;
				}

				goto start;
			}
			else if ( PeekNextChar() == '*' ) {// multi-line comment
				while ( Good() ) {
					char c1 = GetNextChar();
					if ( ( c1 == '*' ) && ( PeekNextChar() == '/' ) ) {
						GetNextChar();
						break;
					}
				}
				goto start;
			}
		default:
			while ( Good() && PeekNextChar() != '=' ) {
				unsigned char c_ = GetNextChar();
				token.value_s += c_;
			}
			token.type = Token::type_entry_name;
			return;
	}
}

void Tokenizer::SkipSpaces() {
	while ( Good() && IsWhitespace( PeekNextChar() ) ) {
		GetNextChar();
	}
}

Token Tokenizer::GetToken( int i ) {
	int p = buffer_pos + i;
	if ( p < 0 )return Token();
	while ( int( token_buffer.size() ) < p + 1 ) {
		Token t;
		ReadToken( t );
		if ( t.IsEOF() )return t;
		token_buffer.push_back( t );
	}

	return token_buffer[p];
}

void Tokenizer::Step( int i ) {
	buffer_pos += i;
}
namespace SL {
Node::~Node() {
	//if(parent)parent->Remove(name);
}

DataList* Node::Parent() const { // parent list
	return parent;
}

bool Node::IsChildOf( DataList *what ) const {
	DataList *current = Parent();
	while ( current ) {
		if ( current == what )return true;
		current = current->Parent();
	}
	return false;
}

void Node::ListRemove() {
	//if(parent->list_first==this)parent->list_first=next;
	//if(parent->list_last==this)parent->list_last=prev;
	if ( list_prev ) {
		list_prev->list_next = this->list_next;
	}
	if ( list_next ) {
		list_next->list_prev = this->list_prev;
	}
	list_prev = NULL;
	list_next = NULL;
}

void Node::ListInsertAfter( Node *what ) {
	ListRemove();
	if ( !what )return;
	this->list_next = what->list_next;
	if ( what->list_next )what->list_next->list_prev = this;
	what->list_next = this;
	this->list_prev = what;
}

wxString Node::Name() {
	return name;
}
bool Node::SetName( const wxString &name_ ) {
	if ( parent ) {
		return parent->Rename( name, name_ );
	}
	name = name_;
	return true;
}

void Node::Save( TDFWriter &/*unused*/ ) {
	/// nothing to save there.
}
void Node::Load( Tokenizer &/*unused*/ ) {
	/// nothing to load there.
	//ASSERT_LOGIC(0,_T("this function should not be called."));
}

/// ***********************************************************
///  class DataList
/// ***********************************************************

DataList::DataList() {
	//parent = NULL;
	list_loop.list_prev = &list_loop;
	list_loop.list_next = &list_loop;
	list_loop.parent = this;
	list_loop.Reference();
}

DataList::~DataList() {// disconnect from childs
	PNode node = First();
	while ( node.Ok() && node != End() ) {
		//std::cout<<"printing"<<std::endl;
		PNode nextnode = Next( node );
		node->parent = NULL;
		node->ListRemove();
		node = nextnode;
	}
}

bool DataList::Insert( PNode node )/// return false if such entry already exists.
{
	if ( !node.Ok() )return false;
	bool inserted = nodes.insert( std::pair<wxString, PNode>( ( *node ).name.Lower(), node ) ).second;
	if ( !inserted )return false;

	node->parent = this;
	node->ListInsertAfter( list_loop.list_prev );
	return true;
}

bool DataList::InsertAt( PNode node, PNode where )/// return false if such entry already exists.
{
	if ( !node.Ok() )return false;
	if ( !( where->list_prev ) )return false;
	bool inserted = nodes.insert( std::pair<wxString, PNode>( ( *node ).name, node ) ).second;
	if ( !inserted )return false;

	node->parent = this;
	node->ListInsertAfter( where->list_prev );
	return true;
}

#ifdef use_std_string
static const char* rename_prefix = "!";
#else
static const wxChar* rename_prefix = _T( "!" );
#endif

void DataList::InsertRename( PNode node ) {/// rename if such entry already exists. str contains new name.
	if ( !node.Ok() )return;

	if ( !Insert( node ) ) {
		wxString original_name = node->Name();
		for ( int n = 0;n < 10000;++n ) {
			//wxString tmp=str+wxString(rename_prefix);
#ifdef use_std_string
			std::ostringstream os;
			os << original_name << rename_prefix << n;
			node->name = os.str();
#else
			wxString tmp;
			tmp << original_name;
			tmp << rename_prefix;
			tmp << n;
			node->name = tmp;
#endif
			if ( Insert( node ) ) {
				return;
			}
		}
		wxLogError( _T( "insertRename: iterated over 10 000 names, way too many" ) );
	}
}

void DataList::InsertRenameAt( PNode node, PNode where ) {// rename if such entry already exists. str contains new name.
	if ( !node.Ok() )return;
	if ( !where->list_prev )return;

	if ( !InsertAt( node, where ) ) {
		for ( int n = 0;n < 10000;++n ) {

#ifdef use_std_string
			std::ostringstream os;
			os << node->Name() << rename_prefix << n;
			node->name = os.str();
#else
			wxString tmp;
			tmp << ( node->Name() );
			tmp << rename_prefix;
			tmp << n;
			node->name = tmp;
#endif
			if ( InsertAt( node, where ) ) {
				return;
			}
		}
		wxLogError( _T( "insertRename: iterated over 10 000 names, way too many" ) );
	}
}

bool DataList::Remove( const wxString &str ) {
	//PNode node=nodes.find(str.Lower())->last;
	PNode node = Find( str );
	if ( !node.Ok() )return false;
	if ( nodes.erase( str.Lower() ) <= 0 ) return false;

	node->parent = NULL;
	node->ListRemove();
	return true;
}

bool DataList::Remove( PNode node ) {
	if ( !node.Ok() )return false;
	if ( nodes.erase( node->Name().Lower() ) <= 0 ) return false;

	node->parent = NULL;
	node->ListRemove();
	return true;
}

bool DataList::Rename( const wxString &old_name, const wxString &new_name ) {
	// check that new name is not used up.
	if ( nodes.find( new_name.Lower() ) != nodes.end() )return false;
	nodes_iterator i = nodes.find( old_name.Lower() );
	if ( i == nodes.end() )return false;
	PNode node = i->second;

	ASSERT_LOGIC( node.Ok(), _T( "Internal TDF tree consistency (1)" ) );
	ASSERT_LOGIC( node->Name().Lower() == old_name.Lower(), _T( "Internal TDF tree consistency (2)" ) );

	node->name = new_name.Lower();
	nodes.erase( i );
	bool inserted = nodes.insert( std::pair<wxString, PNode>( ( *node ).name.Lower(), node ) ).second;
	ASSERT_LOGIC( inserted, _T( "DataList::Rename failed" ) );
	return inserted;
}

/// find by name. unused.
PNode DataList::Find( const wxString &str ) {
	if ( str == _T( ".." ) )return Parent();
	if ( str == _T( "." ) )return this;
	nodes_iterator i = nodes.find( str.Lower() );
	if ( i != nodes.end() ) {
		ASSERT_LOGIC( i->second->Name().Lower() == str.Lower(), _T( "Internal TDF tree consistency (3)" ) );
		return i->second;
	}
	return NULL;
}

wxString DataList::Path() {
	wxString result;
	PDataList tmp( this );
	while ( tmp.Ok() ) {
		result = wxString( _T( "/" ) ) + tmp->Name();
		tmp = tmp->Parent();
	}
	return name;
}

PNode DataList::FindByPath( const wxString &str ) {
	if ( str.empty() )return this;
	int i = 0;
	wxString buff;
	PDataList current_dir( this );
	if ( str[i] == '/' ) {// go to root
		PDataList tmp = Parent();
		while ( tmp.Ok() ) {
			current_dir = tmp;
			tmp = tmp->Parent();
		}
	}
	else {
		buff += str[0];

	}
	i = 1;
	while ( ( unsigned int )( i ) < str.size() ) {
		if ( str[i] == '/' ) {
			if ( buff == _T( ".." ) ) {
				current_dir = current_dir->Parent();
				if ( !current_dir.Ok() )
					return NULL;
			}
			else if ( buff != _T( "." ) && !buff.empty() ) {//
				PNode node = current_dir->Find( buff );
				if ( !node.Ok() )
					return NULL;
				PDataList datalist( node );
				if ( datalist.Ok() ) {
					current_dir = datalist;
				}
				else
					return NULL;
			}
			buff = _T( "" );
		}
		else {
			buff += str[i];
		}
		++i;
	}
	if ( current_dir.Ok() ) {
		if ( !buff.empty() ) {
			return current_dir->Find( buff );
		}
		else
			return PNode( current_dir );
	}
	else {
		return NULL;
	}
}

PNode DataList::Next( PNode what ) {
	if ( what.Ok() )return what->list_next;
	return NULL;
}
PNode DataList::Prev( PNode what ) {
	if ( what.Ok() )return what->list_prev;
	return NULL;
}
PNode DataList::End() {
	return PNode( &list_loop );
}
PNode DataList::First() {
	return PNode( list_loop.list_next );
}
PNode DataList::Last() {
	return PNode( list_loop.list_prev );
}

/*
void DataList::PrintContent(std::ostream &s) {
    PNode node=First();

    while(node.Ok() && node!=End()){
      //std::cout<<"printing"<<std::endl;
      #ifdef use_std_string
      s<<std::endl<<"'"<<node->Name()<<"'={";
        node->PrintContent(s);
        s<<"}"<<std::endl;
      #else
        s<<std::endl<<"'"<<node->Name().mb_str()<<"'={";
        node->PrintContent(s);
        s<<"}"<<std::endl;
      #endif
      node=Next(node);
    }
}*/
void DataList::Save( TDFWriter &f ) {
	PNode node = First();
	if ( node == End() )return;

	while ( node.Ok() && node != End() ) {
		// if class name is not set properly, continue
		//if(node->ClassName().empty())continue;
		PDataList list( node );
		if ( list.Ok() ) {
			f.EnterSection( list->Name() );
			list->Save( f );
			f.LeaveSection();
		} else {
			PDataLeaf leaf( node );
			if ( leaf.Ok() ) {
				leaf->Save( f );
			}
		}
		node = Next( node );
	}

}

void DataList::Load( Tokenizer &f ) {
	while ( f.Good() ) {
		Token t = f.TakeToken();
		switch ( t.type ) {
			case Token::type_leave_section:
			case Token::type_eof:
				return;
			case Token::type_entry_name:
				{
					PDataLeaf new_leaf( new DataLeaf );
					new_leaf->SetName( t.value_s );
					new_leaf->Load( f );
					Insert( PNode( new_leaf ) );
				}
				break;

			case Token::type_section_name:
				{
					Token t2 = f.TakeToken();
					if ( t2.type != Token::type_enter_section ) {
						f.ReportError( t, _T( "'{' expected" ) );
					} else {
						PDataList new_list( new DataList );
						new_list->SetName( t.value_s );
						new_list->Load( f );// will eat the '}'
						Insert( PNode( new_list ) );
					}
				}
				break;
			default:
				f.ReportError( t, _T( "[sectionname] or entryname= expected." ) );
		}

	}
}


int DataList::GetInt( const wxString &f_name, int default_value, bool *it_worked ) {
	PDataLeaf leaf( Find( f_name ) );
	if ( !leaf.ok() ) {
		if ( it_worked ) {
			*it_worked = false;
		}
		return default_value;
	}
	wxString s = leaf->GetValue();
	long result = default_value;
	if ( !s.ToLong( &result ) ) {
		if ( it_worked ) {
			*it_worked = false;
		}
		return result;
	}
	if ( it_worked ) {
		*it_worked = true;
	}
	return result;
}
double DataList::GetDouble( const wxString &f_name, double default_value, bool *it_worked ) {
	PDataLeaf leaf( Find( f_name ) );
	if ( !leaf.ok() ) {
		if ( it_worked ) {
			*it_worked = false;
		}
		return default_value;
	}
	wxString s = leaf->GetValue();
	double result = default_value;
	if ( !s.ToDouble( &result ) ) {
		if ( it_worked ) {
			*it_worked = false;
		}
		return result;
	}
	if ( it_worked ) {
		*it_worked = true;
	}
	return result;

}
wxString DataList::GetString( const wxString &f_name, const wxString &default_value, bool *it_worked ) {
	PDataLeaf leaf( Find( f_name ) );
	if ( !leaf.ok() ) {
		if ( it_worked ) {
			*it_worked = false;
		}
		return default_value;
	}
	if ( it_worked ) {
		*it_worked = true;
	}
	return leaf->GetValue();
}
int DataList::GetDoubleArray( const wxString &f_name, int n_values, double *values ) {
	PDataLeaf leaf( Find( f_name ) );
	if ( !leaf.ok() ) {
		return 0;
	}
	wxStringTokenizer tok( leaf->GetValue() );
	int i = 0;
	int values_read = 0;
	for ( i = 0;i < n_values && tok.HasMoreTokens();++i ) {
		wxString s = tok.GetNextToken();
		if ( s.ToDouble( values + i ) )values_read++;
	}
	return values_read;
}

wxColour DataList::GetColour( const wxString &f_name, const wxColour &default_value, bool *it_worked ) {
	double values[3];
	if ( GetDoubleArray( f_name, 3, values ) != 3 ) {
		if ( it_worked ) {
			*it_worked = false;
		}
		return default_value;
	}
	if ( it_worked ) {
		*it_worked = true;
	}
	return wxColour( values[0]*255.99, values[1]*255.99, values[2]*255.99 );
}


wxString DataLeaf::GetValue() {
	return value;
}
void DataLeaf::SetValue( const wxString &value_ ) {
	value = value_;
}

void DataLeaf::Save( TDFWriter &f ) {
	f.Append( Name(), GetValue() );
}
void DataLeaf::Load( Tokenizer &f ) {
	Token t = f.TakeToken();
	value = t.value_s;
	t = f.TakeToken();
	if ( t.value_s != _T( ";" ) ) {
		f.ReportError( t, _T( "; expected" ) );
	}
}
} // end namespace SL

SL::PDataList ParseTDF( std::istream &s, int *error_count ) {
	Tokenizer t;
	t.EnterStream( s );
	SL::PDataList result( new SL::DataList );
	result->Load( t );
	if ( error_count ) {
		*error_count = t.NumErrors();
	}
	return result;
}
