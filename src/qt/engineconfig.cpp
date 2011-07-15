/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/


#include "engineconfig.h"

#include <QDir>
#include <QDebug>

#include <customizations.h>
#include <springunitsynclib.h>
#include <settings.h>
#include "../settings++/presets.h"

/**
 * @brief POSIX file locking class
 */
class ScopedFileLock
{
public:
	ScopedFileLock(int fd, bool write);
	~ScopedFileLock();
private:
	int filedes;
};

/**
 * @brief lock fd
 *
 * Lock file descriptor fd for reading (write == false) or writing
 * (write == true).
 */
ScopedFileLock::ScopedFileLock(int fd, bool write) : filedes(fd)
{
#ifndef _WIN32
	struct flock lock;
	lock.l_type = write ? F_WRLCK : F_RDLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	if (fcntl(filedes, F_SETLKW, &lock)) {
		// not a fatal error
		//handleerror(0, "Could not lock config file", "DotfileHandler", 0);
	}
#endif
}

/**
 * @brief unlock fd
 */
ScopedFileLock::~ScopedFileLock()
{
#ifndef _WIN32
	struct flock lock;
	lock.l_type = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	if (fcntl(filedes, F_SETLKW, &lock)) {
		// not a fatal error
		//handleerror(0, "Could not unlock config file", "DotfileHandler", 0);
	}
#endif
}

EngineConfig::EngineConfig()
{
}

EngineConfig::EngineConfig( const QString& filename, const QString& id )
{
	load( filename, id );
}

bool EngineConfig::load( const QString& filename, const QString& id )
{
	data_.clear();
	filename_ = filename;
	id_ = id;
	FILE* file;

	if ((file = fopen(filename_.toStdString().c_str(), "r"))) {
		ScopedFileLock scoped_lock(fileno(file), false);
		Read(file);
	} else {
		if (!(file = fopen(filename_.toStdString().c_str(), "a")))
			throw std::runtime_error("DotfileHandler: Could not write to config file");
		return false;
	}
	fclose(file);
	return true;
}

void EngineConfig::commit() const
{
	foreach( DataContainerType::value_type entry, data_ )
	{
		susynclib().SetSpringConfigString( TowxString( entry.first ), TowxString( entry.second ) );
	}
	wxString id = TowxString( id_ );
	wxString key = _T("sasi/last_used_preset");
	sett().Set( key, id );
}

char* Strip(char* begin, char* end)
{
	while (isspace(*begin)) ++begin;
		while (end >= begin && isspace(*end)) --end;
		*(end + 1) = '\0';
		return begin;
}

void EngineConfig::Read(FILE* file)
{
	char buf[500];
		rewind(file);
		while (fgets(buf, sizeof(buf), file))
			AppendLine(buf);
}

void EngineConfig::AppendLine(char* line)
{
	char* line_stripped = Strip(line, strchr(line, '\0'));

	if (*line_stripped != '\0') {
		// line is not empty
		if (line_stripped[0] == '#') {
			// a comment line
			return;
		}

		char* eq = strchr(line_stripped, '=');
		if (eq) {
			// a "key=value" line
			char* key = Strip(line_stripped, eq - 1);
			char* value = Strip(eq + 1, strchr(eq + 1, 0) - 1);
			data_[key] = value;
		}
	}
}

PresetModel::PresetModel(QObject *parent )
	:QAbstractListModel( parent )
{
	reload( );
}

void PresetModel::reload()
{
	wxString key = _T("sasi/last_used_preset");
	QString last_id = ToQString( sett().Get( key, wxEmptyString ) );
	presets_.clear();
	QDir preset_dir( SLcustomizations().DataBasePath() + "/presets/" );
	QStringList nameFilter;
		nameFilter << "*.conf";
	QFileInfo current_file;
	foreach( current_file, preset_dir.entryInfoList( nameFilter, QDir::Files ) )
	{
		QString id = current_file.fileName().replace(".conf","");
		presets_.append( std::make_pair<EngineConfig,QString>( EngineConfig( current_file.absoluteFilePath(), id ),
															id ) );
		if ( id == last_id )
			last_sessions_index_ = presets_.size() - 1;
	}
}

int PresetModel::lastSessionIndex() const
{
	return last_sessions_index_;
}

int PresetModel::rowCount(const QModelIndex &/*parent*/ ) const
{
	return presets_.size();
}

QVariant PresetModel::data(const QModelIndex &index, int /*role*/ ) const
{
	int row =  index.row();
	if ( !index.isValid() || row >= presets_.size() )
		   return QVariant();
	return QVariant::fromValue(presets_[row].second);
}

QString PresetModel::name(int index) const
{
	if ( index < presets_.size() )
		return presets_[index].second;
	return QString();
}

void PresetModel::use(int index) const
{
	static bool first_use = true;

	if ( first_use )
	{
		first_use = false;
		return;// we don't want to commit when the model is first loaded
	}
	if ( index > -1 && index < presets_.size() )
	{
		const EngineConfig& config = presets_[index].first;
		config.commit();
	}
	else
		qDebug() << "void PresetModel::use(int index) const OOB " << index;
}

ScreenResolutionModel::ScreenResolutionModel(QObject *parent )
	:QAbstractListModel( parent )
{
	reload( );
}

void ScreenResolutionModel::reload()
{
	wxString key = _T("sasi/last_used_resolution");
	QString last_id = ToQString( sett().Get( key, wxEmptyString ) );
	resolutions_.clear();
	for ( int i = 0; i < vl_Resolution_Str_size; ++i )
	{
		const ScreenResolution res( vl_Resolution_X[i], vl_Resolution_Y[i], i > vl_Resolution_startOfDualScreenRes);
		QString id = res.toString();
		resolutions_.append( res );
		if ( id == last_id )
			last_sessions_index_ = resolutions_.size() - 1;
	}
}

int ScreenResolutionModel::lastSessionIndex() const
{
	return last_sessions_index_;
}

int ScreenResolutionModel::rowCount(const QModelIndex &/*parent*/ ) const
{
	return resolutions_.size();
}

QVariant ScreenResolutionModel::data(const QModelIndex &index, int /*role*/ ) const
{
	int row =  index.row();
	if ( !index.isValid() || row >= resolutions_.size() )
		   return QVariant();
	return QVariant::fromValue(resolutions_[row].toString());
}

QString ScreenResolutionModel::name(int index) const
{
	if ( index < resolutions_.size() )
		return resolutions_[index].toString();
	return QString();
}

QString ScreenResolutionModel::ScreenResolution::toString() const
{
	return QString( "%1 x %2").arg( width ).arg( height );
}

void ScreenResolutionModel::use(int index) const
{
	static bool first_use = true;

	if ( first_use )
	{
		first_use = false;
		return;// we don't want to commit when the model is first loaded
	}
	if ( index > 0 && index < resolutions_.size() )
	{
		const ScreenResolution& res = resolutions_[index];
		susynclib().SetSpringConfigInt( _T("XResolution"), res.width );
		susynclib().SetSpringConfigInt( _T("YResolution"), res.height );
		susynclib().SetSpringConfigInt( _T("DualScreenMode"), res.dualscreen );
		wxString id = TowxString( res.toString() );
		wxString key = _T("sasi/last_used_resolution");
		sett().Set( key, id );
	}
	else
		qDebug() << "void ScreenResolutionModel::use(int index) const OOB " << index;
}

