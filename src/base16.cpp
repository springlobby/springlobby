#include "base16.h"


wxString ToBase16(unsigned char *begin, int size){
  wxString result;
  result.reserve(size*3);
  char *digits="0123456789ABCDEF";
  for(int i=0;i<size;++i){
    result.Append(digits[begin[i]>>4]);
    result.Append(digits[begin[i]&15]);
    result.Append(' ');
  }
  return result;
}
