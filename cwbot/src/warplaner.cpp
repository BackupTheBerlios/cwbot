/* warplaner.cpp contains classes/structs, which
 * are needed for managing the clanwar-records.
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

#include "warplaner.h"
#include <qfile.h>

#ifdef WINDOWS
#include <iostream.h>
#else
#include <iostream>
using namespace std;
#endif //WINDOWS

war::war()
{

}

warplaner::warplaner()
{
	QFile opmlFile( "./wars.xml" );
	if ( !opmlFile.open( IO_ReadOnly ) )
	{
		cout << "Critical Error!\n->Cannot open war-file<-" << endl;
		exit(0);
	}

	if ( !domTree.setContent( &opmlFile ) )
	{
		cout << "Critical Error!\n->Parsing error for war-file<-" << endl;
		opmlFile.close();
		exit(0);
	}
	opmlFile.close();
	QTimer *timer = new QTimer();
	connect( timer, SIGNAL(timeout()), this, SLOT(timerDone()) );
	timer->start(2000); // 2 seconds single-shot timer
}
warplaner::~warplaner()
{}
void warplaner::timerDone()
{
	//cout << "der nächste war ist am " << wars().getNextClanwar().time() << endl;
	//cout << "es ist ein " << getNextClanwar().type() << " gegen " << getNextClanwar().opponent() << endl;
}

QString warplaner::saveWars(QString filename)
{
	QFile file(filename);
    if ( file.open( IO_WriteOnly ) ) {
        QTextStream stream( &file );
        stream << domTree.toString() << "\n";
        file.close();
		return "Wars saved successfully.";
    }
	else
		return "Wars NOT saved successfully.";
}

QString warplaner::getNextClanwarId()
{
	QString tmpTime = "99999999";
	QString tmpClanwarId = "No Matching Clanwar Found!";
	QDomElement root = domTree.documentElement();
	node = root.firstChild();
	while ( !node.isNull() )
	{
		if ( node.isElement() && node.nodeName() == "war" )
		{
			element = node.toElement();
			node2 = element.firstChild();
			//search for the war with the lowest time
			while ( !node2.isNull() )
			{
				if ( node2.isElement() && node2.nodeName() == "time")
				{
					if (node2.firstChild().nodeValue().toInt() < tmpTime.toInt())
					{
						tmpTime = node2.firstChild().nodeValue();
						tmpNode = node;
					}
				}
				node2 = node2.nextSibling();
			}
		}
		node = node.nextSibling();
	}
	//search the id of the cw
	tmpNode = tmpNode.firstChild();
	while ( !tmpNode.isNull() )
	{
		if ( tmpNode.isElement() && tmpNode.nodeName() == "id")
		{
			tmpClanwarId = tmpNode.firstChild().nodeValue();
		}
		tmpNode = tmpNode.nextSibling();
	}
	return tmpClanwarId;
	//return "No matching war found!";
	//return "warplaner::getNextClanwar() is not implemented yet!! see warplaner.cpp!";
}

war *warplaner::getClanwar(QString warID)
{
	tmpWar = new war();
	QDomElement root = domTree.documentElement();
	node = root.firstChild();
	while ( !node.isNull() )
	{
		if ( node.isElement() && node.nodeName() == "war" )
		{
			element = node.toElement();
			node2 = element.firstChild();
			//search for the war with the lowest time
			while ( !node2.isNull() )
			{
				if ( node2.isElement() && node2.nodeName() == "id")
				{
					if (node2.firstChild().nodeValue() == warID)
					{
						tmpNode = node;
					}
				}
				node2 = node2.nextSibling();
			}
		}
		node = node.nextSibling();
	}

	tmpNode = tmpNode.firstChild();

	while ( !tmpNode.isNull() )
	{
		element = tmpNode.toElement();
		if (element.tagName() == "player")
		{
			tmpWar->list.append(player(element.attribute("name"),element.attribute("state")));
		}
		tmpNode = tmpNode.nextSibling();

	}

	return tmpWar;
}

/*QString warplaner::getPlayers(QString warID)
{
	QDomElement root = domTree.documentElement();
	node = root.firstChild();
	while ( !node.isNull() )
	{
		if ( node.isElement() && node.nodeName() == "war" )
		{
			element = node.toElement();
			node2 = element.firstChild();
			while ( !node2.isNull() )
			{
				if ( node2.isElement() && node2.nodeName() == "id")
				{
					if (node2.firstChild().nodeValue() == warID)
					{
						tmpNode = node;
					}
				}
				node2 = node2.nextSibling();
			}
		}
		node = node.nextSibling();
	}

	QString tmpQString;
	tmpNode = tmpNode.firstChild();
	while ( !tmpNode.isNull() )
	{
		if ( tmpNode.isElement() && tmpNode.nodeName() == "player")
		{
			tmpQString += tmpNode.firstChild().nodeValue() + ", ";
			tmpNode.firstChild().setNodeValue(tmpNode.firstChild().nodeValue() + "3");
		}
		tmpNode = tmpNode.nextSibling();
	}
	return tmpQString;
}*/

