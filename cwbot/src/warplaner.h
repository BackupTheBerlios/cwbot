/* warplaner.h contains the a class-definition for the warplaner-class, which
 * is responsible for managing the clanwar-records.
 *
 * Copyright (C) 2003 Malte Böhme (malte@stammkranich.de)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef WARPLANER_H
#define WARPLANER_H

#include <qobject.h>
#include <qdom.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qtimer.h>

class player
{
public:
	player(): n(0) {}
	player(QString name,QString state)
            : n(name), s(state)
        {}
	~player(){}
	QString name(){return n;}
	QString state(){return s;}
	bool isSet;
	void setName(QString newName){n=newName;};
	void setState(QString newState){s=newState;};
private:
	QString n;
	QString s;

};

typedef QValueList<player> playerlist;

class war
{
public:
	war();
	~war(){};
    playerlist list;
	QString opponent;
	QString type;
	QString time;
	QStringList maps;
};

///This is used to manage the wars

///The warplaner class is used to get and set war-records
///to/from the xml-file. This class is responsible
///for keeping track of the wars, and to notify players that there is an upcoming clanwar.
class warplaner : public QObject
{
	Q_OBJECT

public:
	warplaner();
	~warplaner();
	///returns the next clanwar in a string prepared for beeing displayed on IRC
	QString getNextClanwarId();
	QString getPlayers(QString warID);
	QString saveWars(QString filename);
	war *getClanwar(QString warID);
	war *tmpWar;
private:

	QDomNode node;
	QDomNode node2;
	QDomNode tmpNode;
	QDomElement element;
	QDomDocument domTree;

private slots:
	void timerDone();

signals:
	void signalWriteToIrc(const QString&);
};

#endif /* WARPLANER_H */

