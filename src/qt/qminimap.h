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


#ifndef QMINIMAP_H
#define QMINIMAP_H

#ifdef SL_QT_MODE

#include <QDeclarativeItem>

class Battle;
class BattleStartRect;

class QMinimap : public QDeclarativeItem
{
    Q_OBJECT
	Q_PROPERTY(int battleId READ battleId WRITE setBattleId NOTIFY battleIdChanged)
public:
    explicit QMinimap(QDeclarativeItem *parent = 0);

	int battleId() const;
	void setBattleId( int id );

signals:
	void battleIdChanged();

public slots:
	void onBattleinfoUpdated( int battleId );

protected:
//    void mousePressEvent(QGraphicsSceneMouseEvent* event);
//    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
//    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
	QRect transformStartRect( const BattleStartRect& sr ) const;
	void drawStartRects(QPainter* painter);
	void drawStartPos(QPainter* painter);

	Battle* m_battle;
	int m_battle_id;
	QImage m_minimap_image;
};

#endif // #ifdef SL_QT_MODE
#endif // QMINIMAP_H
