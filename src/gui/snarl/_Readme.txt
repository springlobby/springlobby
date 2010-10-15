==============================================================
| Snarl C++ interface
==============================================================
Written by Toke Noer (toke 'at' noer.it)
For bugfixes etc. fell free to contact me.

The C++ interface for Snarl is implemented in:
SnarlInterface.cpp
SnarlInterface.h

Include these two files in your project, and you should be ready to use Snarl.

Quick example:
#include "SnarlInterface.h"
Snarl::V41::SnarlInterface *snarl = new Snarl::V41::SnarlInterface();
snarl->RegisterApp(_T("AppId"), _T("CSharp Test App"), NULL, NULL, 0, 0);
snarl->EZNotify(_T("id"), _T("title"), _T("message body"), 10, _T(""), 0, _T("ack"), _T("value")));
snarl->UnregisterApp();
delete snarl;

Take a look at the test applications if you need help getting it to work. (See SVN links below)



==============================================================
| Snarl C++ interface test application
==============================================================
The application is made to test the functionality of the Snarl C++ interface. 
I take no responsibility for any data corruption, data deletion or any other harm done by this program. 
Take a look at the source and compile for your self so you know nothing is wrong with the program.



================================================================================
= Home of this file: https://snarlwin.svn.sourceforge.net/svnroot/snarlwin/trunk/hdr/C++/
= API reference: http://www.fullphat.net/dev/api.htm
= VB reference: https://snarlwin.svn.sourceforge.net/svnroot/snarlwin/trunk/hdr/VB/
================================================================================
