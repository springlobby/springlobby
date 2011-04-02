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


#include "qsettings.h"

#include <QDir>

#include <customizations.h>

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

QSettings::QSettings()
{
}

bool QSettings::load( const QString& filename )
{
	filename_ = filename;
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

void QSettings::commit()
{

}

char* Strip(char* begin, char* end)
{
	while (isspace(*begin)) ++begin;
		while (end >= begin && isspace(*end)) --end;
		*(end + 1) = '\0';
		return begin;
}

void QSettings::Read(FILE* file)
{
	char buf[500];
		rewind(file);
		while (fgets(buf, sizeof(buf), file))
			AppendLine(buf);
}

void QSettings::AppendLine(char* line)
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
	preset_names_.clear();
	QDir preset_dir( SLcustomizations().DataBasePath() + "/presets/" );
	QStringList nameFilter;
		nameFilter << "*.conf";
	QFileInfo current_file;
	foreach( current_file, preset_dir.entryInfoList( nameFilter, QDir::Files ) )
	{
		 preset_names_.append( current_file.fileName().replace(".conf","") );
	}
}

int PresetModel::rowCount(const QModelIndex &/*parent*/ ) const
{
	return preset_names_.size();
}

QVariant PresetModel::data(const QModelIndex &index, int /*role*/ ) const
{
	int row =  index.row();
	if ( !index.isValid() || row >= preset_names_.size() )
		   return QVariant();
	return QVariant::fromValue(preset_names_[row]);
}

QString PresetModel::name(int index) const
{
	if ( index < preset_names_.size() )
		return preset_names_[index];
	return QString();
}

void PresetModel::use(int index) const
{

}
