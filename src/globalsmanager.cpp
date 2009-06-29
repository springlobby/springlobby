#include "globalsmanager.h"
#include <assert.h>
#include <vector>
#include <wx/log.h>

#include "utils/conversion.h"
#include "utils/debug.h"

class GlobalsManager
{
    std::vector<IGlobalObjectHolder *> globals;
    bool de_initialized;

public:
    GlobalsManager();
    bool RegisterGlobal(IGlobalObjectHolder *obj);
    void DestroyAll();
    virtual ~GlobalsManager();

protected:

private:

};

GlobalsManager *mgr=NULL;
bool destroyed=false;

GlobalsManager *globals()
{
    if ( destroyed ) return NULL;
    if ( !mgr )
    {
        mgr = new GlobalsManager;
    }
    return mgr;
}

void DestroyGlobals()
{
    if ( mgr )
    {
        mgr->DestroyAll();
        delete mgr;
    }
    destroyed = true;
}

bool IGlobalObjectHolder::RegisterSelf()
{
    if ( !globals() ) return false;
    return globals()->RegisterGlobal( this );
}

GlobalsManager::GlobalsManager():
de_initialized( false )
{
    wxLogMessage( _T("GlobalsManager::GlobalsManager()") );
    //ctor
}

bool GlobalsManager::RegisterGlobal( IGlobalObjectHolder *obj )
{
    wxLogDebugFunc( _T("") );
    if ( de_initialized )
    {
        return false;
    }
    globals.push_back( obj );
    return true;
}

void GlobalsManager::DestroyAll()
{
    wxLogMessage( _T("GlobalsManager::DestroyAll()") );
    for ( std::vector<IGlobalObjectHolder *>::iterator i = globals.begin(); i != globals.end(); ++i )
    {
        (*i)->Nullify();
    }

    for ( std::vector<IGlobalObjectHolder *>::iterator i = globals.begin(); i != globals.end(); ++i )
    {
        try
        {
            (*i)->Destroy();
        }
        catch ( std::runtime_error e )
        {
            wxLogMessage( _T("GlobalsManager::DestroyAll(), runtume_error '%s' when destroying"), TowxString( e.what() ).c_str() );
        }
    }

    globals.clear();
    de_initialized = true;
    wxLogMessage( _T("GlobalsManager::DestroyAll() done") );
}

GlobalsManager::~GlobalsManager()
{
    assert(globals.empty());
}

