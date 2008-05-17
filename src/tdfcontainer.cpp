#include "tdfcontainer.h"



TDFWriter::TDFWriter(wxString &s):
m_stream(s),
m_depth(0)
{

}

TDFWriter::~TDFWriter(){
  Close();
}
void TDFWriter::EnterSection(const wxString &name){
  Indent();
  m_stream<<_T("[")<<name<<_T("]\n");
  Indent();
  m_stream<<_T("{\n");
  m_depth++;
}
void TDFWriter::LeaveSection(){
  m_depth--;
  Indent();
  m_stream<<_T("}\n");
}
void TDFWriter::Indent(){
  for(int i=0;i<m_depth;++i)m_stream<<_T("\t");
}
    //wxString GetCurrentPath();
void TDFWriter::Append(const wxString &name, wxString value){
  Indent();
  m_stream<<name<<_T("=")<<value<<_T(";\n");
}

void TDFWriter::Close(){
  while(m_depth>0)LeaveSection();
  if(m_depth<0)wxLogWarning(_T("error in TDFWriter: more LeaveSection() calls than EnterSection()"));
}

void TDFWriter::AppendLineBreak(){
  m_stream<<_T("\n");
}



#include "parsing.h"
#include "sstream"
#include "fstream"
#include <iomanip>
#include <algorithm>


const mystring FileLoader_token_names[10]={
      _T("token_none"),
      _T("token_string"),
      _T("token_identifier"),
      _T("token_symbol"),
      _T("token_int"),
      _T("token_double"),
      _T("token_enter_section"),
      _T("token_leave_section"),
      _T("token_eof"),
      _T("token_semicolon")
    };

void Token::Dump(std::ostream &os){
  os<<" ("<<pos_string<<") : token_type="<<FileLoader_token_names[type];
  os<<" , string_v='"<<value_s;
  os<<"' , int_v="<<value_i;
  os<<" , double_v="<<value_d;
  os<<std::endl;
}

//#define ReportError(a) {std::cerr<<"error "<<(a);}
#define ReportError(a) {wxLogMessage(_T("error %s"),TowxString(a).c_str());}

void Tokenizer::EnterStream(std::istream &stream_, mystring name){
  skip_eol=false;
  include_stack.push_back(IncludeCacheEntry(&stream_, false));
  include_stack.back().name=name;
  include_stack.back().line=1;
  include_stack.back().column=1;
};
void Tokenizer::UnwindStack(){
  while((!include_stack.empty()) && include_stack.back().stream && (!include_stack.back().stream->good())){
    include_stack.pop_back();
  }
};

char Tokenizer::PeekNextChar(){
  UnwindStack();
  //std::string tmp;
  char tmp=0;
  if(!include_stack.empty())tmp=(*include_stack.back().stream).peek();
  return tmp;
}

char Tokenizer::GetNextChar(){
  start:
  UnwindStack();
  //std::string tmp;
  if(include_stack.empty())return 0;

  char c=(*include_stack.back().stream).get();

  if((!skip_eol) && (c==10 || c==13)){/// end of line
      include_stack.back().line+=1;
      include_stack.back().column=1;

      /// if next is different we'll skip it.
      //std::istream &stream=(*include_stack.back().stream);
      char nc=(*include_stack.back().stream).peek();
      if((nc==10 || nc==13) &&(nc!=c))skip_eol=true;
  }else{
     if(!skip_eol)include_stack.back().column+=1;
     skip_eol=false;
  }

  return c;
};
bool Tokenizer::Good(){
  UnwindStack();
  return !include_stack.empty();
};

void Tokenizer::ReadToken(Token &token){
  start:

  SkipSpaces();

  //stream>>std::ws;

  token.value_s.clear();
  token.value_i=0;
  token.value_d=0.0;

  //token_start_line=line;
  //token_start_column=column;

  if(!Good()){
    token.type=Token::type_eof;
    token.pos_string="EOF";
    return;
  }
  std::ostringstream tmp;
  if(!include_stack.back().name.empty())tmp<<include_stack.back().name<<" , ";
  tmp<<"line "<<include_stack.back().line<<" , column "<<include_stack.back().column;
  token.pos_string=tmp.str();

  bool negate=false;


  char c=GetNextChar();

  token.value_s+=c;
  /// first find what token is it, and handle all except numbers
  switch(c){
  case '.':
    if(IsNum(PeekNextChar())){
      goto decimal;
    }else{
      token.type=Token::type_symbol;
      return;
    }
    break;
  case '+':
    if(IsNum(PeekNextChar())){/// it is a number
      token.type=Token::type_int;
      c=GetNextChar();
      token.value_s+=c;

    }else{// just lonely +
      token.type=Token::type_symbol;
      return;
    }
    break;
  case '-':
    if(IsNum(PeekNextChar())){ /// it is a number
      token.type=Token::type_int;/// or double, figure out later
      negate=true;

      c=GetNextChar();
      token.value_s+=c;

    }else{ /// just lonely -
      token.type=Token::type_symbol;
      return;
    }
    break;
  case '"':{
      token.type=Token::type_string;
      token.value_s.clear();
      bool skip_next_eol_char=false;
      while(Good()){
        c=GetNextChar();
        /// wxString has problem with zero characters, replace by space.
        if(c==0){
          c=' ';
        }

        if(c=='\\'){
          if(Good()){
            token.value_s+=GetNextChar();
          }else{
            ReportError(wxT("Quotes not closed before end of file"));
            return;
          }
        }else if(c=='"'){
          return;
        }else
        {
          token.value_s+=c;
        }
        /// handle end of line
        if(skip_next_eol_char){
          skip_next_eol_char=false;
        }else if(c==10 || c==13){
          //++line;
          //column=1;
          if((PeekNextChar() == 10 || PeekNextChar() == 13) && (PeekNextChar()!=c))skip_next_eol_char=true;
        }
      }
      ReportError(wxT("Quotes not closed before end of file"));
    }
  case '{':
    //value_s=_T("{");
    token.type=Token::type_enter_section;
    return;
  case '}':
    //value_s=_T("}");
    token.type=Token::type_leave_section;
    return;
  case ';':
    token.type=Token::type_semicolon;
    return;
  case '/':/// handle comments
  if(PeekNextChar()=='/'){
    //SkipToEOL();
    if(!include_stack.empty()){
      std::string tmp;
      std::getline((*include_stack.back().stream), tmp);
      include_stack.back().line+=1;
      include_stack.back().column=1;
    }

    goto start;
  }
  else if(PeekNextChar()=='*'){// multi-line comment
    while(Good()){
      char c1=GetNextChar();
      if((c1=='*') && (PeekNextChar()=='/')){
        GetNextChar();
        break;
      }
    }
    goto start;
  }
  default:
    if(IsNum(c)){
      token.type=Token::type_int;
    }else if(IsLetter(c)){
      while(Good() && IsAlphaNum(PeekNextChar())){
        unsigned char c=GetNextChar();
        token.value_s+=c;
      }
      token.type=Token::type_identifier;
      return;
    }else{
      token.type=Token::type_symbol;
      return;
    }
  }

  /// handle numbers

  token.value_i=c-'0';
  token.value_d=token.value_i;
  //char nc;
  while(true){
    if(!Good())return ;
    c=PeekNextChar();
    if(!IsNum(c))break;
    token.value_i*=10;
    token.value_d*=10.0;
    token.value_i+=c-'0';
    token.value_d+=c-'0';

    token.value_s+=GetNextChar();
  }
  if(c=='.'){
decimal:
    token.type=Token::type_double;
    double digit_scale=0.1;
    token.value_s+=GetNextChar();
    int iterations=0;
    while(Good()){
      c=PeekNextChar();
      if(!IsNum(c))break;
      token.value_d+=(c-'0')*digit_scale;
      digit_scale*=0.1;
      token.value_s+=GetNextChar();
      iterations++;
    }
    if(iterations==0){
      //wxString error_message;
      //error_message<<_T( "File loading error at (" )<<token_start_line<<_T( "," )<<token_start_column<<_T( ") Quotes not closed before end of file" );
      //wxLogMessage(error_message);
      //error_message.Printf(_T("File loading error at (%i,%i): Decimal value expected in floating point constant"),line,column);
      //wxLogMessage(error_message);
      ReportError(_T("Decimal value expected in floating point constant"));
    }
  }
  if(c=='E' || c== 'e'){
    int exponent=0;
    bool negate_exponent=false;
    token.type=Token::type_double;
    token.value_s+=GetNextChar();
    if(PeekNextChar()=='+'){
      token.value_s+=GetNextChar();
      //column++;
    }else if(PeekNextChar()=='-')
    {
      negate_exponent=true;
      token.value_s+=GetNextChar();
    }
    int iterations=0;
    while(Good()){
      c=PeekNextChar();
      if(!IsNum(c))break;
      exponent*=10;
      exponent+=(c-'0');

      token.value_s+=GetNextChar();
      iterations++;
    }
    if(iterations==0){
      //wxString error_message;
      //error_message<<_T( "File loading error at (" )<<token_start_line<<_T( "," )<<token_start_column<<_T( ") Quotes not closed before end of file" );
      //wxLogMessage(error_message);
      //error_message.Printf(_T("File loading error at (%i,%i): Exponent value expected in floating point constant"),line,column);
      //wxLogMessage(error_message);
      ReportError(_T("Exponent value expected in floating point constant"));
    }
    if(negate_exponent)exponent=-exponent;
    token.value_d*=exp(log(10.0)*exponent);
    token.value_i=token.value_d;

  }
  if(negate){
    token.value_i=-token.value_i;
    token.value_d=-token.value_d;
  }
};

void Tokenizer::SkipSpaces(){
  while(Good() && IsWhitespace(PeekNextChar())){
    char c=GetNextChar();
  }
}

Token Tokenizer::GetToken(int i){
  int p=buffer_pos+i;
  if(p<0)return Token();
  while(token_buffer.size()<p+1){
    Token t;
    ReadToken(t);
    if(t.IsEOF())return t;
    token_buffer.push_back(t);
  }

  //if(p>=token_buffer.size())return Token();

  //token_buffer[p].Dump(std::cout);
  return token_buffer[p];
};

void Tokenizer::Step(int i){
  buffer_pos+=i;
};



Node::~Node(){
  //if(parent)parent->Remove(name);
}

DataList* Node::Parent() const { // parent list
  return parent;
}

bool Node::IsChildOf(DataList *what) const{
  DataList *current=Parent();
  while(current){
    if(current==what)return true;
    current=current->Parent();
  }
  return false;
}

void Node::ListRemove(){
  //if(parent->list_first==this)parent->list_first=next;
  //if(parent->list_last==this)parent->list_last=prev;
  if(list_prev){list_prev->list_next=this->list_next;}
  if(list_next){list_next->list_prev=this->list_prev;}
  list_prev=NULL;
  list_next=NULL;
}

void Node::ListInsertAfter(Node *what){
  ListRemove();
  if(!what)return;
  this->list_next=what->list_next;
  if(what->list_next)what->list_next->list_prev=this;
  what->list_next=this;
  this->list_prev=what;
}

mystring Node::Name(){
  return name;
}
bool Node::SetName(const mystring &name_){
  if(parent){
    return parent->Rename(name, name_);
  }
  name=name_;
  return true;
}

/*
mystring Node::ClassName(){
    if(factory.Ok()){
        return factory->Name();
    }else{
        return wxString();
    }
  };*/


PNodeInfo Node::GetNodeInfo() const{
  return node_info;
}
void Node::SetNodeInfo(const PNodeInfo &node_info_){
  node_info=node_info_;
}

void Node::Save(FileWriter &f){
  /// nothing to save there.
}
void Node::Load(FileLoader &f){
  /// nothing to load there. Just skip to next.
  f.NextToken();

  if(f.token_type==FileLoader::token_leave_section)return;
  f.ReportError(_T("'}' expected"));

  do{
    if(f.token_type=FileLoader::token_eof){
      f.ReportError(_T("unexpected end of file."));
      return;
    }
    f.NextToken();
  }while(f.token_type!=FileLoader::token_leave_section);

}

void Node::UpdateTopology(){
  /// nothing to do there
}

void Node::Compile(CompilerOutput &out){
  /// nothing to compile there
}

PNode Node::Clone(){
  PNode result=NewNode(ClassName());
  result->SetName(Name());
  result->SetNodeInfo(node_info);
  return result;
}


/// ***********************************************************
///  class DataPath  (unused)
/// ***********************************************************

DataPath::DataPath():parent_depth(0) {}
/*
void DataPath::SetPathString(const mystring &str){
    //wxLogMessage();
  };
mystring DataPath::GetPathString(){
  };
*/
void DataPath::Append(const mystring &str){
  path.push_back(str);
}
void DataPath::Prepend(const mystring &str){
  path.push_front(str);
}
void DataPath::StripFront(){
  path.pop_front();
}
void DataPath::StripBack(){
  path.pop_back();
}


/// ***********************************************************
///  class DataList
/// ***********************************************************

DEFINE_NODE_CLASS(DataList)

DataList::DataList() {
  //parent = NULL;
  list_loop.list_prev=&list_loop;
  list_loop.list_next=&list_loop;
  list_loop.parent=this;
  list_loop.Reference();
}

DataList::~DataList() {// disconnect from childs
  PNode node=First();
  while(node.Ok() && node!=End()){
    //std::cout<<"printing"<<std::endl;
    PNode nextnode=Next(node);
    node->parent=NULL;
    node->ListRemove();
    node=nextnode;
  }
}

int DataList::type_id() const {
  return type_datalist;
}

bool DataList::Insert(PNode node)/// return false if such entry already exists.
{
  if(!node.Ok())return false;
  bool inserted=nodes.insert(std::pair<mystring, PNode>((*node).name, node)).second;
  if(!inserted)return false;

  node->parent=this;
  node->ListInsertAfter(list_loop.list_prev);
  node->UpdateTopology();
  return true;
}

bool DataList::InsertAt(PNode node, PNode where)/// return false if such entry already exists.
{
  if(!node.Ok())return false;
  if(!(where->list_prev))return false;
  bool inserted=nodes.insert(std::pair<mystring, PNode>((*node).name, node)).second;
  if(!inserted)return false;

  node->parent=this;
  node->ListInsertAfter(where->list_prev);
  node->UpdateTopology();
  return true;
}

#ifdef use_std_string
static const char* rename_prefix="!";
#else
static const wxChar* rename_prefix=_T("!");
#endif

void DataList::InsertRename(PNode node) {/// rename if such entry already exists. str contains new name.
  if(!node.Ok())return;

  if(!Insert(node)) {
    mystring original_name=node->Name();
    for(int n=0;n<10000;++n) {
      //mystring tmp=str+mystring(rename_prefix);
#ifdef use_std_string
      std::ostringstream os;
      os<<original_name<<rename_prefix<<n;
      node->name=os.str();
#else
      mystring tmp;
      tmp<<original_name;
      tmp<<rename_prefix;
      tmp<<n;
      node->name=tmp;
#endif
      if(Insert(node)) {
        return;
      }
    }
    std::cout<<"insertRename: iterated over 10 000 names, way too many"<<std::endl;
  }
}

void DataList::InsertRenameAt(PNode node, PNode where) {// rename if such entry already exists. str contains new name.
  if(!node.Ok())return;
  if(!where->list_prev)return;

  if(!InsertAt(node,where)) {
    for(int n=0;n<10000;++n) {

#ifdef use_std_string
      std::ostringstream os;
      os<<node->Name()<<rename_prefix<<n;
      node->name=os.str();
#else
      mystring tmp;
      tmp<<(node->Name());
      tmp<<rename_prefix;
      tmp<<n;
      node->name=tmp;
#endif
      if(InsertAt(node,where)) {
        return;
      }
    }
    std::cout<<"insertRename: iterated over 10 000 names, way too many"<<std::endl;
  }
}

bool DataList::Remove( const mystring &str ) {
  //PNode node=nodes.find(str)->last;
  PNode node = Find(str);
  if(!node.Ok())return false;
  if(nodes.erase(str)<=0) return false;

  node->parent=NULL;
  node->ListRemove();
  node->UpdateTopology();
  return true;
}

bool DataList::Remove( PNode node ) {
  if(!node.Ok())return false;
  if(nodes.erase(node->Name())<=0) return false;

  node->parent=NULL;
  node->ListRemove();
  node->UpdateTopology();
  return true;
}

bool DataList::Rename(const mystring &old_name, const mystring &new_name){
  /// check that new name is not used up.
  if(nodes.find(new_name)!=nodes.end())return false;
  nodes_iterator i=nodes.find(old_name);
  if(i==nodes.end())return false;
  PNode node=i->second;
  my_assert(node.Ok());
  my_assert(node->Name()==old_name);
  node->name=new_name;
  nodes.erase(i);
  bool inserted=nodes.insert(std::pair<mystring, PNode>((*node).name, node)).second;
  my_assert(inserted);
  return inserted;
}

/// find by name. unused.
PNode DataList::Find( const mystring &str ) {
  if(str==_T(".."))return Parent();
  if(str==_T("."))return this;
  nodes_iterator i=nodes.find(str);
  if(i!=nodes.end()){
    my_assert(i->second->Name()==str);
    return i->second;
  }
  return NULL;
}

mystring DataList::Path(){
  mystring result;
  PDataList tmp(this);
  while(tmp.Ok()) {
    result=mystring(_T("/"))+tmp->Name();
    tmp=tmp->Parent();
  }
  return name;
}

PNode DataList::FindByPath( const mystring &str ) {
  if(str.empty())return this;
  int i=0;
  mystring buff;
  PDataList current_dir(this);
  if(str[i]=='/') {/// go to root
    PDataList tmp=Parent();
    while(tmp.Ok()) {
      current_dir=tmp;
      tmp=tmp->Parent();
    }
  }
  else {
    buff+=str[0];

  }
  i=1;
  while(i<str.size()) {
    if(str[i]=='/') {
      if(buff==_T("..")) {
        current_dir=current_dir->Parent();
        if(!current_dir.Ok())
          return NULL;
      }
      else if(buff!=_T(".")&&!buff.empty()) {//
        PNode node=current_dir->Find(buff);
        if(!node.Ok())
          return NULL;
        if(node->type_id()==type_datalist) {
          current_dir=static_cast<PDataList>(node);
        }
        else
          return NULL;
      }
      buff=_T("");
    }
    else {
      buff+=str[i];
    }
    ++i;
  }
  if(current_dir.Ok()) {
    if(!buff.empty()) {
      return current_dir->Find(buff);
    }
    else
      return PNode(current_dir);
  }
  else {
    return NULL;
  }
}

PNode DataList::Next(PNode what){
  if(what.Ok())return what->list_next;
  return NULL;
}
PNode DataList::Prev(PNode what){
  if(what.Ok())return what->list_prev;
  return NULL;
}
PNode DataList::End(){
  return PNode(&list_loop);
}
PNode DataList::First(){
  return PNode(list_loop.list_next);
}
PNode DataList::Last(){
  return PNode(list_loop.list_prev);
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
void DataList::Save(FileWriter &f){
  PNode node=First();
  if(node==End())return;
  f.LineBreak();
  while(node.Ok() && node!=End()){
    // if class name is not set properly, continue
    //if(node->ClassName().empty())continue;
    WriteString(f.stream,node->Name());
    // Save it intelligently - whenever possible, do not write redundant stuff
    if(node->ClassName()==_T("Node")){
      // write nothing
    }
    else if(node->ClassName()==_T("VariantLeaf")){
        // write variant
        f.stream<<"=";
        node->Save(f);
    }else
    {
        if(node->ClassName()!=_T("DataList"))f.stream<<" "<<node->ClassName().mb_str();
        f.stream<<"={";
        f.EnterSection();
        node->Save(f);
        f.LeaveSection();
        f.LineBreak();
        f.stream<<"}";
    }

    f.stream<<";";
    node=Next(node);
    if(node!=End())f.LineBreak();
  }
}

PNode LoadImplicit(FileLoader &f){// called with current token set to first token of implicit node
  //f.ReportError(_T("loading of implicit types is not supported yet.(you need to specify type explicitly)"));
  switch(f.token_type){
  case FileLoader::token_string:{
      RefcountedPointer<VariantLeaf> new_node(new VariantLeaf);
      new_node->v=wxVariant(f.value_s);
      f.NextToken();
      return PNode(new_node);
    }
  case FileLoader::token_int:{
      RefcountedPointer<VariantLeaf> new_node(new VariantLeaf);
      new_node->v=wxVariant((long int)f.value_i);
      f.NextToken();
      return PNode(new_node);
    }
  case FileLoader::token_double:{
      RefcountedPointer<VariantLeaf> new_node(new VariantLeaf);
      //new_node->v=wxVariant(f.value_d);
      //f.NextToken();
      new_node->Load(f);
      return PNode(new_node);
    }
  case FileLoader::token_enter_section:{
      PDataList new_node(new DataList);
      new_node->Load(f);
      if(f.token_type!=FileLoader::token_leave_section){
        f.ReportError(_T("'}' expected"));
        f.SkipToSemicolon();
        return PNode(new_node);
      }
      f.NextToken();
      return PNode(new_node);
    }
  case FileLoader::token_semicolon:{
      PNode new_node(new Node);
      return new_node;
    }
  default:
    f.ReportError(_T("Invalid implicit node"));
  }
  return PNode(NULL);
}

void DataList::Load(FileLoader &f){
  //wxLogMessage(_T("DataList::Load"));
  while(f.token_type!=FileLoader::token_eof){
    f.NextToken();
    //wxLogMessage(_T("next token done"));

    switch(f.token_type){
    case FileLoader::token_identifier:
    case FileLoader::token_string:{
        mystring node_name=f.value_s;
        f.NextToken();
        PNode new_node;
        // type may be explicitly specified or there might be = right away:
        if(f.token_type==FileLoader::token_identifier){
          // read explicit identifier
          /*
          PNode tmp(GlobFactoryList()->Find(f.value_s));
          if(!tmp.Ok()){
              f.ReportError(wxString(_T("Type '"))+f.value_s+_T("' not found."));
              f.SkipToSemicolon();
              goto end_of_switch;
           }
          PAbstractFactoryLeaf factory(tmp);
          // if no such factory found
          if(!factory.Ok()){
              f.ReportError(_T("Can not cast."));
              f.SkipToSemicolon();
              goto end_of_switch;
           }
           new_node=factory->GenericNewObject();
           */
          new_node=NewNode(f.value_s);
          if(!new_node.Ok()){
            //f.ReportError(_T("Node not found"));
            f.ReportError(wxString(_T("Type '"))+f.value_s+_T("' not found or failed to create(bad metadata?)."));
            f.SkipToSemicolon();
            goto end_of_switch;
          }

          f.NextToken();

          // there it may be just = ; or have something inside
          // check if we have nothing to load
          if(f.token_type==FileLoader::token_semicolon){
            goto node_load_end;
          }
          // if it is not =
          if(f.value_s!=wxString(_T("="))){
            f.ReportError(_T("'=' expected"));
            f.SkipToSemicolon();
            goto end_of_switch;
          }
          f.NextToken();
          //new_node=factory->GenericNewObject();
          if(f.token_type!=FileLoader::token_enter_section){
            f.ReportError(_T("'{' expected"));
            f.SkipToSemicolon();
            goto end_of_switch;
          }
          //f.NextToken();
          new_node->Load(f);
          if(f.token_type!=FileLoader::token_leave_section){
            f.ReportError(_T("'}' expected"));
            f.SkipToSemicolon();
            goto end_of_switch;
          }
          f.NextToken();
        }else
        {
          // implicit identifier
          //wxLogMessage(_T("Loading implicit"));
          if(f.token_type==FileLoader::token_semicolon){// Nothing useful to load there. Just empty PNode
            new_node=PNode(new Node);
            goto node_load_end;
          }
          if(f.value_s!=wxString(_T("="))){
            f.ReportError(_T("identifier or '=' expected"));
            f.SkipToSemicolon();
            goto end_of_switch;
          }
          f.NextToken();
          new_node=LoadImplicit(f);
        }
node_load_end:
        if(!new_node.Ok()){
          f.ReportError(_T("Failed to load node"));
          f.SkipToSemicolon();
          goto end_of_switch;
        }
        new_node->SetName(node_name);
        PNode samename_node=Find(node_name);

        //node with this name already exist. Try to move child nodes from old to new
        if(samename_node.Ok()){
          PDataList new_list(new_node);
          PDataList old_list(samename_node);
          if(new_list.Ok() && old_list.Ok()){
            wxLogMessage(_T("Got node with same name as some already existing node. Will move all childs from old into new and replace old by new"));
            PNode old_child=old_list->First();
            while(old_child!=old_list->End()){
              PNode next_old_child=old_list->Next(old_child);
              old_list->Remove(old_child);
              new_list->Insert(old_child);
              old_child=next_old_child;
            }
            Remove(samename_node);
            Insert(new_node);
          }else{
            f.ReportError(_T("Failed to load node because node with same name ")+node_name+_T(" already exists"));
          }

        }else{
          Insert(new_node);
        }
      }// end load node
      break;
    case FileLoader::token_leave_section:
    case FileLoader::token_eof:
      //return;
      goto end_of_load;
    default:
      f.ReportError(_T("string or '}' or end of file expected"));
    }
end_of_switch:
    if(f.token_type==FileLoader::token_eof || f.token_type==FileLoader::token_leave_section)goto end_of_load;
    if(f.token_type!=FileLoader::token_semicolon){
      f.ReportError(_T(" '}', ';' or end of file expected"));
      f.SkipToSemicolon();
    }
  }
end_of_load:;

//wxLogMessage(_T("DataList::Load end"));
//UpdateTopology();
}

void DataList::UpdateTopology(){
  PNode node;
  for(node=First(); node!=End(); node=Next(node)){
    node->UpdateTopology();
  }
}

void DataList::Compile(CompilerOutput &out){
  PNode node;
  for(node=First(); node!=End(); node=Next(node)){
    node->Compile(out);
  }
}

PNode DataList::Clone(){
  PNode result=Node::Clone();
  PDataList r(result);
  if(!r.Ok())return PNode(NULL);
  PNode node;
  for(node=First(); node!=End(); node=Next(node)){
    PNode cloned=node->Clone();
    if(cloned.Ok())r->Insert(cloned);
  }
  return result;
}


