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


#ifndef QERRORWINDOW_H
#define QERRORWINDOW_H

#include <QDialog>

template <class T>
class QList;
class QDeclarativeError;

class QErrorWindow : public QDialog
{
    Q_OBJECT
public:
	QErrorWindow(const QList<QDeclarativeError>& errors, QWidget *parent = 0);

signals:

public slots:

};

#endif // QERRORWINDOW_H
