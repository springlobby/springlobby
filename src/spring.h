#ifndef SPRINGLOBBY_HEADERGUARD_SPRING_H
#define SPRINGLOBBY_HEADERGUARD_SPRING_H

#include <wx/event.h>
#ifdef SL_QT_MODE
	#include <QObject>
	#include <QProcess>
#else
	#define Q_OBJECT
#endif

class wxCommandEvent;
class IBattle;
class SinglePlayerBattle;
class NoGuiSinglePlayerBattle;
class OfflineBattle;
class Battle;
class SpringProcess;
class wxSpringProcess;
class wxString;


class Spring:
		#ifdef SL_QT_MODE
		 public  QObject,
		#endif
		public wxEvtHandler
{
	Q_OBJECT
  public:
	explicit Spring();
    ~Spring();

    bool IsRunning() const;
    bool Run( Battle& battle );
    bool Run( SinglePlayerBattle& battle );
    bool Run( NoGuiSinglePlayerBattle& battle );
    bool Run( OfflineBattle& battle );

    //! executes spring with replay as parameter
    /*!
     * \param filename the full path for the replayfile
     */
    bool RunReplay ( const wxString& filename );

    wxString WriteScriptTxt( IBattle& battle ) const;
    void OnTerminated( wxCommandEvent& event );

#ifdef SL_QT_MODE
public slots:
	void OnStopped( int exitCode, QProcess::ExitStatus exitStatus );
	void OnStarted();
signals:
	void springStarted();
	void springStopped();
private:
	QProcess* qt_process_;
#endif

  protected:
		bool LaunchSpring( const wxString& params );

    SpringProcess* m_process;
    wxSpringProcess* m_wx_process;
    bool m_running;

    DECLARE_EVENT_TABLE()
};

Spring& spring();

#endif // SPRINGLOBBY_HEADERGUARD_SPRING_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

