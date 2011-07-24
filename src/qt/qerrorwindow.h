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


#ifndef SPRINGLOBBY_HEADERGUARD_QERRORWINDOW_H
#define SPRINGLOBBY_HEADERGUARD_QERRORWINDOW_H

#include <QDialog>
#include <QTextEdit>
#include <QList>
#include <QBoxLayout>

class QErrorWindow : public QDialog
{
	Q_OBJECT
public:
	template <class T>
	explicit QErrorWindow(const QList<T>& errors, QWidget *parent = 0);
};

namespace {
	template <class T>
	void appendErrorToTextEdit( const T& error, QTextEdit* text )
	{
		text->append( error.toString() );
	}

	template <>
	void appendErrorToTextEdit( const QString& error, QTextEdit* text )
	{
		QString f = error;
		text->append( f );
	}
}

template <class T>
QErrorWindow::QErrorWindow(const QList<T>& errors, QWidget *parent) :
	QDialog(parent)
{
	setFixedSize(800,600);
	QVBoxLayout* layout = new QVBoxLayout(this);
	QTextEdit* text = new QTextEdit(this);
	T error;
	foreach( error, errors )
	{
		appendErrorToTextEdit( error, text );
	}
	layout->addWidget(text,1);
	setLayout(layout);
}

#endif // SPRINGLOBBY_HEADERGUARD_QERRORWINDOW_H
