#include "auimanager.h"
#include "../globalsmanager.h"

AuiManagerContainer& GetAui()
{
    static LineInfo<AuiManagerContainer> m( AT );
    static GlobalObjectHolder<AuiManagerContainer, LineInfo<AuiManagerContainer> > m_aui( m );
    return m_aui;
}
