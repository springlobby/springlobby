#include "auimanager.h"
#include "../globalsmanager.h"

AuiManagerContainer& GetAui()
{
  static GlobalObjectHolder<AuiManagerContainer> m_aui;
  return m_aui;
}
