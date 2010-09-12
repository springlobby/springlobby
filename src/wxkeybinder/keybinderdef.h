/////////////////////////////////////////////////////////////////////////////
// Name:        keybinder.h
// Purpose:     shared build defines
// Author:      Francesco Montorsi
// Created:     2005/07/10
// RCS-ID:      $Id: keybinderdef.h 437 2007-03-01 11:38:54Z frm $
// Copyright:   (c) Aleksandras Gluchovas and (c) Francesco Montorsi
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_KEYBINDER_DEFS_H_
#define _WX_KEYBINDER_DEFS_H_

// Defines for component version.
// The following symbols should be updated for each new component release
// since some kind of tests, like those of AM_WXCODE_CHECKFOR_COMPONENT_VERSION()
// for "configure" scripts under unix, use them.
#define wxKEYBINDER_MAJOR          2
#define wxKEYBINDER_MINOR          2
#define wxKEYBINDER_RELEASE        0

// For non-Unix systems (i.e. when building without a configure script),
// users of this component can use the following macro to check if the
// current version is at least major.minor.release
#define wxCHECK_KEYBINDER_VERSION(major,minor,release) \
    (wxKEYBINDER_MAJOR > (major) || \
    (wxKEYBINDER_MAJOR == (major) && wxKEYBINDER_MINOR > (minor)) || \
    (wxKEYBINDER_MAJOR == (major) && wxKEYBINDER_MINOR == (minor) && wxKEYBINDER_RELEASE >= (release)))



// for shared builds
#ifdef WXMAKINGDLL_KEYBINDER
    #define WXDLLIMPEXP_KEYBINDER                  WXEXPORT
    #define WXDLLIMPEXP_DATA_KEYBINDER(type)       WXEXPORT type
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_KEYBINDER                  WXIMPORT
    #define WXDLLIMPEXP_DATA_KEYBINDER(type)       WXIMPORT type
#else // not making nor using DLL
    #define WXDLLIMPEXP_KEYBINDER
    #define WXDLLIMPEXP_DATA_KEYBINDER(type)	    type
#endif

#endif // _WX_KEYBINDER_DEFS_H_

