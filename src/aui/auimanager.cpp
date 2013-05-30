#include "auimanager.h"
#include <lslutils/globalsmanager.h>

AuiManagerContainer& GetAui()
{
    static LSL::Util::LineInfo<AuiManagerContainer> m( AT );
    static LSL::Util::GlobalObjectHolder<AuiManagerContainer, LSL::Util::LineInfo<AuiManagerContainer> > m_aui( m );
    return m_aui;
}
