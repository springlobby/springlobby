#include "aui/auimanager.h"

AuiManagerContainer& GetAui()
{
  static AuiManagerContainer m_aui;
  return m_aui;
}
