#include "qerrorwindow.h"

#include <QDeclarativeView>
#include <QDeclarativeError>
#include <QList>
#include <QBoxLayout>
#include <QTextEdit>

QErrorWindow::QErrorWindow(const QList<QDeclarativeError>& errors, QWidget *parent) :
	QDialog(parent)
{
	setFixedSize(800,600);
	QVBoxLayout* layout = new QVBoxLayout(this);
	QTextEdit* text = new QTextEdit(this);
	QDeclarativeError error;
	foreach( error, errors )
	{
		text->append( error.toString() );
	}

	layout->addWidget(text,1);
	setLayout(layout);
}
