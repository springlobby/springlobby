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

#include "qminimap.h"

#include <QPainter>
#include <battle.h>
#include <server.h>
#include <converters.h>

QMinimap::QMinimap(QDeclarativeItem *parent) :
	QDeclarativeItem(parent),
	m_battle(0),
	m_battle_id(-1)
{
	setFlag(QGraphicsItem::ItemHasNoContents, false);
}

void QMinimap::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/ )
{
	painter->setRenderHints(QPainter::Antialiasing, true);

	const QRectF bounds = boundingRect();
	painter->drawImage( bounds ,m_minimap_image );

	if (!m_battle)
		return;
	const IBattle::StartType position_type =
			IBattle::StartType(s2l( CustomBattleOptions().getSingleValue( _T("startpostype"), OptionsWrapper::EngineOption ) ));
	switch ( position_type ) {
		case IBattle::ST_Fixed:
		case IBattle::ST_Pick:
			drawStartRects( painter );
			break;
		case IBattle::ST_Choose:
		default:
			drawStartPos( painter );
	}
}
void QMinimap::drawStartRects(QPainter* painter)
{
	for ( int i = 0; i <= int(m_battle->GetLastRectIdx()); ++i )
	{
		QRect sr = transformStartRect( m_battle->GetStartRect(i) );
		if ( sr.isEmpty() )
			continue;
		QColor col;
		if ( i == m_battle->GetMe().BattleStatus().ally )
			col.setRgb( 0, 200, 0 );
		else
			col.setRgb( 200, 0, 0 );

		painter->setPen(QPen (col, 2));
		painter->setBrush(QBrush(col, Qt::BDiagPattern));
		painter->drawRect( sr );
		painter->drawText( sr.center(), QString( "Ally: %1" ).arg( i ) );
	}
}

void QMinimap::drawStartPos(QPainter* painter)
{
	if (!m_battle)
		return;
	const QRectF mr = boundingRect();

}

void QMinimap::setBattleId( int id )
{
	if ( id == m_battle_id )
		return;
	m_battle_id = id;
	try {
		m_battle = &serverSelector().GetServer().GetBattle( id );
	}
	catch ( ... ) {}
	if (!m_battle) {
		emit battleIdChanged();
		return;
	}
	const wxString mapname = m_battle->GetHostMapName();
	m_map = m_battle->LoadMap();
	const QRectF bounds = boundingRect();
	const int width = bounds.width();
	const int height = bounds.height();

	const wxImage h = usync().GetMinimap( mapname, width, height );
	const QImage q = wxQtConvertImage( h );
	assert( !q.isNull() );
	m_minimap_image = q.scaled( width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );

	update();
	emit battleIdChanged();
}

int QMinimap::battleId() const
{
	return m_battle_id;
}

void QMinimap::onBattleinfoUpdated( int id )
{
	if ( id == m_battle_id )
		return;
	update();
}

QRect QMinimap::transformStartRect( const BattleStartRect& sr ) const
{
	int x1,y1,x2,y2;
	if ( !sr.exist || sr.todelete ) return QRect();
	QRectF mr = boundingRect();

	x1 = int( (mr.x() + sr.left * mr.width() / 200.0) + 0.5 );
	y1 = int( (mr.y() + sr.top * mr.height() / 200.0) + 0.5 );
	x2 = int( (mr.x() + sr.right * mr.width() / 200.0) + 0.5 );
	y2 = int( (mr.y() + sr.bottom * mr.height() / 200.0) + 0.5 );

	return QRect( x1, y1, x2-x1, y2-y1 );
}
