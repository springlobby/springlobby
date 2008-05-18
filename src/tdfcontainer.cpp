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
