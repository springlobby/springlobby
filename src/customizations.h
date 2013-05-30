#ifndef SPRINGLOBBY_HEADERGUARD_CUSTOMIZATIONS_H
#define SPRINGLOBBY_HEADERGUARD_CUSTOMIZATIONS_H

#include <lslutils/globalsmanager.h>
#include <lslunitsync/optionswrapper.h>
#include <wx/string.h>
#include <wx/icon.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/iconbndl.h>

#ifdef SL_QT_MODE
	#include <QString>
	#include <QList>
#endif

//! single global instance to get all customizsations from
class Customizations {
    protected:
        Customizations();

        wxString m_modname;
        LSL::OptionsWrapper m_customs;
        wxIconBundle m_app_icons;
        wxString m_help_url;
        wxString m_archive;
        bool m_active;

		bool KeyExists( const wxString& key ) const;
        bool Provides( const wxString& key ) const;

    public:
        ~Customizations() {}

        bool Init( const wxString& );
		bool Active() const;

        const wxString& GetModname() const;
        const wxString& GetHelpUrl() const;
        const wxString& Archive() const;
        const wxIconBundle& GetAppIconBundle() const;

        const LSL::OptionsWrapper& GetCustomizations() const;

		wxString GetIntroText() const;

		//! if key is found bitmap is changed and true returned
		bool GetBitmap( const wxString& key, wxBitmap& bitmap );

		static const wxString IntroKey;// ( _T("intro_file") );
        friend class LSL::Util::GlobalObjectHolder<Customizations, LSL::Util::LineInfo<Customizations> >;

#ifdef SL_QT_MODE
        bool Init(const QString& shortname, const QString& version );

		struct DataException : public std::exception {
			const QList<QString> errors_;
			DataException( const QList<QString>& errors )
				:errors_(errors)
			{}
            const char* what() const throw() ;
			virtual ~DataException() throw() {}
		};
		QString DataBasePath();

	private:
		QString dataBasePath_;
		QString m_shortname;

	public:
		QString QmlDir();
		QString GraphicsDir();
		QString SoundsDir();
        QString MusicDir();
        const QString& GetModshortname() const;
#endif
};

Customizations& SLcustomizations();

#endif // SPRINGLOBBY_HEADERGUARD_CUSTOMIZATIONS_H

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
