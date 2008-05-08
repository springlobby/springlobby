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

class TDFContainer
{
};

#endif // TDFCONTAINER_H
