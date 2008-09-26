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
/// project: render_control_new/utils/parsing.h and cpp , database.h and cpp
///

#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <map>

#include <autopointers.h>

class Node;
typedef RefcountedPointer<Node> PNode;
class DataList;
typedef RefcountedPointer<DataList> PDataList;
class DataLeaf;
typedef RefcountedPointer<DataLeaf> PDataLeaf;

class Tokenizer;

class Node: public RefcountedContainer {
  friend class DataList;
protected:
	DataList *parent;
	Node *list_prev;
	Node *list_next;
	void ListRemove();
	void ListInsertAfter(Node *what);
	wxString name;
public:

    wxString Name();

    // Sets the name, and updates parent if present
    bool SetName(const wxString &name_);
    Node(): parent(NULL), list_prev(NULL), list_next(NULL){}
    virtual ~Node();
    DataList* Parent() const;// parent list
    //void SetParent(DataList *parent_);
    bool IsChildOf(DataList *what) const;

    virtual void Save(TDFWriter &f);
    virtual void Load(Tokenizer &f);
};

class DataList: public Node {
  protected:
    std::map<wxString,PNode> nodes;
    Node list_loop;// next is first, prev is last in the list
    typedef std::map<wxString,PNode>::iterator nodes_iterator;
  public:
    DataList();
    ~DataList();

    bool Insert(PNode node);// return false if such entry already exists.
    bool InsertAt(PNode node, PNode where);// return false if such entry already exists. Inserts right before given

   // rename if such entry already exists. Names is like name!1 or name!2 etc
    void InsertRename(PNode node);
    void InsertRenameAt(PNode node,PNode where);
    bool Remove(const wxString &str);
    bool Remove(PNode node);
    bool Rename(const wxString &old_name, const wxString &new_name);
    PNode Find(const wxString &str);// find by name

    wxString Path();

    PNode FindByPath(const wxString &str);

    PNode Next(PNode what);
    PNode Prev(PNode what);
    PNode End();
    PNode First();
    PNode Last();

    virtual void Save(TDFWriter &f);
    virtual void Load(Tokenizer &f);
};

class DataLeaf: public Node{
  friend class DataList;
  protected:
  wxString value;
  public:
  wxString GetValue();
  void SetValue(const wxString &value);

  virtual void Save(TDFWriter &f);
  virtual void Load(Tokenizer &f);
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
inline bool IsWhitespace(char c){
	return (c==' ') || (c==10) || (c==13) || (c=='\t');
}
struct Token{
    enum TokenType{
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

    bool IsEOF(){
      return (type==type_eof);
    }
    Token():type(type_eof)
    {
    }

};

class Tokenizer{
  /// todo: clean up, move to CPP file
  struct IncludeCacheEntry{// simple reference counted pointer to stream.
    wxString name;/// used for error reporting
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

  int errors;

  public:
  Tokenizer(): buffer_pos(0), skip_eol(false), errors(0)
  {
  }

  void EnterStream(std::istream &stream_, const wxString &name=_T(""));

  Token GetToken(int i=0);
  void Step(int i=1);
  inline Token TakeToken(){
    Token result=GetToken();
    Step();
    return result;
  }

  bool Good();

  void ReportError(const Token &t, const wxString &err);

  int NumErrors(){return errors;}
};

inline Tokenizer &operator >>(Tokenizer &tokenizer, Token &token){
  token=tokenizer.TakeToken();
  return tokenizer;
};

PDataList ParseTDF(std::istream &s, int *error_count=NULL);

#endif // TDFCONTAINER_H
