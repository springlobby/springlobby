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


#ifndef SIDEMODEL_H
#define SIDEMODEL_H

#ifdef SL_QT_MODE

#include <QAbstractListModel>
#include <QList>

class wxString;

class SideModel : public QAbstractListModel
{
    Q_OBJECT
public:
	explicit SideModel( const wxString& modname, QObject *parent = 0);
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
signals:

public slots:

private:
	QList<QString> m_sidenames;
};

#endif //#ifdef SL_QT_MODE
#endif // SIDEMODEL_H
