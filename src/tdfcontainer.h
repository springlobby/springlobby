#ifndef TDFCONTAINER_H
#define TDFCONTAINER_H

#include <wx/string.h>
#include "utils.h"
/// Todo: add TDFContainer class.
///

class TDFWriter
{
  public:
    TDFWriter(wxString &s);
    ~TDFWriter();
    void EnterSection(const wxString &name);
    void LeaveSection();
    void Indent();
    wxString GetCurrentPath();
    void Append(const wxString &name, wxString value);
    template<class T> void Append(const wxString &name, T value){
      Append(name,TowxString(value));
    }
    /// works like algorithms such as std::sort
    template<class T>
    void Append(const wxString &name, T begin, T end){
      Indent();
      m_stream<<name<<_T("=");
      for(T it=begin;it!=end;++it){
        if(it!=begin)m_stream<<_T(" ");
        m_stream<<(*it);
      }
      m_stream<<_T(";\n");
    }
    void AppendLineBreak();
    void Close();
  protected:
  private:
  wxString &m_stream;
  int m_depth;
};

///
/// Parsing classes contributed by dmytry aka dizekat, copypasted from personal
/// project, render_control_new/utils/parsing.h and cpp , database.h and cpp
///

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <deque>

class Node;
typedef RefcountedPointer<Node> PNode;
class DataList;
typedef RefcountedPointer<DataList> PDataList;

class Node: public RefcountedContainer {
  friend class DataList;
protected:
	DataList *parent;
	Node *list_prev;
	Node *list_next;
	void ListRemove();
	void ListInsertAfter(Node *what);
	mystring name;
public:

    mystring Name();

    // Sets the name, and updates parent if present
    bool SetName(const mystring &name_);
    Node(): parent(NULL), list_prev(NULL), list_next(NULL){}
    virtual ~Node();
    virtual int type_id() const {
        return type_unknown;
    };
    DataList* Parent() const;// parent list
    //void SetParent(DataList *parent_);
    bool IsChildOf(DataList *what) const;

    virtual void Save(FileWriter &f);
    virtual void Load(FileLoader &f);
    virtual void UpdateTopology();
    virtual void Compile(CompilerOutput &out);

    /// create new clone
    virtual PNode Clone();

};

class DataList: public Node {
    DECLARE_NODE_CLASS(DataList)
  protected:
    //std::vector<PNode> ordered_nodes;
    //std::set<PNode,compare_pnode_name>
    std::map<mystring,PNode> nodes;
    Node list_loop;// next is first, prev is last in the list
    //std::map<PNode,mystring> names;
    typedef std::map<mystring,PNode>::iterator nodes_iterator;
    //typedef std::map<PNode,mystring>::iterator names_iterator;
    //DataList *parent;
  public:
    DataList();
    ~DataList();

    virtual int type_id() const;
    bool Insert(PNode node);// return false if such entry already exists.
    bool InsertAt(PNode node, PNode where);// return false if such entry already exists. Inserts right before given
    //bool Insert(const mystring &str, PDataList list);// return false if such entry already exists.

   // rename if such entry already exists. Names is like name!1 or name!2 etc
    void InsertRename(PNode node);
    void InsertRenameAt(PNode node,PNode where);
    bool Remove(const mystring &str);
    bool Remove(PNode node);
    bool Rename(const mystring &old_name, const mystring &new_name);
    PNode Find(const mystring &str);// find by name

    mystring Path();

    PNode FindByPath(const mystring &str);

    PNode Next(PNode what);
    PNode Prev(PNode what);
    PNode End();
    PNode First();
    PNode Last();

    virtual void Save(FileWriter &f);
    virtual void Load(FileLoader &f);
    virtual void UpdateTopology();
    virtual void Compile(CompilerOutput &out);

    virtual PNode Clone();
};


class TDFContainer
{
};

inline bool IsLetter(char c){
	return (c>='a' && c<='z')||(c>='A' && c<='Z')||(c=='_');
}
inline bool IsNum(char c){
	return c>='0' && c<='9';
}
inline bool IsNumDot(char c){
	return (c>='0' && c<='9')||c=='.';
}
inline bool IsAlphaNum(char c){
	return IsLetter(c)||IsNum(c);
}
/*
bool IsNumFirst(char c){
	return IsNum(c) || c=='+' || c=='-' || c=='.';
}*/
inline bool IsWhitespace(char c){
	return (c==' ') || (c==10) || (c==13) || (c=='\t');
}

#define mystring wxString

struct Token{
    //int line;
    //int column;
    //int token_start_line;
    //int token_start_column;
    //bool skip_eol;
    enum TokenType{
        type_none,
        type_string,
        type_identifier,
        type_symbol,
        type_int,
        type_double,
        type_enter_section,
        type_leave_section,
        type_eof,
        type_semicolon
    };
    TokenType type;
    mystring value_s;
    int value_i;
    double value_d;

    mystring pos_string;// for error reporting

    bool IsInteger(){
      return type==type_int;
    }
    bool IsNumber(){
      return (type==type_int)||(type==type_double);
    }
    bool IsString(){
      return (type==type_string);
    }

    bool IsEOF(){
      return (type==type_eof);
    }

    bool IsIdentifier(){
      return (type==type_identifier);
    }
    Token():type(type_eof), value_i(0), value_d(0)
    {
    }

    void Dump(std::ostream &os);
};

class Tokenizer{
  struct IncludeCacheEntry{// simple reference counted pointer to stream.
    mystring name;/// used for error reporting
    int line, column;

    std::istream *stream;
    //bool must_delete;
    int *refcount;
    IncludeCacheEntry(std::istream *stream_, bool must_delete_=false):
    stream(stream_),
    refcount(NULL)
    {
      line=1;
      column=1;
      if(must_delete_){
        refcount=new int;
        (*refcount)=1;
      }
    }
    IncludeCacheEntry(const IncludeCacheEntry &other){
      stream=other.stream;
      refcount=other.refcount;
      if(refcount)(*refcount)+=1;
    }
    ~IncludeCacheEntry()
    {
      if(refcount){
        (*refcount)--;
        if((*refcount)<=0){
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

  void ReadToken(Token &token);
  void SkipSpaces();

  public:
  Tokenizer(): /*max_buffer_size(1024),*/ buffer_pos(0), skip_eol(false)
  {
  }

  void EnterStream(std::istream &stream_, const std::string &name="");

  //std::string GetNext();
  //char GetNext();
  Token GetToken(int i=0);
  void Step(int i=1);
  inline Token TakeToken(){
    Token result=GetToken();
    Step();
    return result;
  }

  bool Good();
};

inline Tokenizer &operator >>(Tokenizer &tokenizer, Token &token){
  token=tokenizer.TakeToken();
  return tokenizer;
};

template <class T>
std::string ReplaceStr(const std::string &arg_, const std::string &what, const T &by_what){
  std::string arg=arg_;
  size_t pos=0;
  std::stringstream s;
  s<<by_what;
  while(pos<arg.size()){
    pos=arg.find(what,pos);
    if(pos==std::string::npos)break;
    arg.replace(pos, what.size(),s.str());
    pos+=s.str().size();
  }
  return arg;
};

#endif // TDFCONTAINER_H
