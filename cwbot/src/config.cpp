/* config.cpp contains the implementation of a class for parsing a
 * config file in simple xml.
 *
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


#include <qfile.h>
#include <stdlib.h>

#include "config.h"

#ifdef WINDOWS
#include <iostream.h>
#else
#include <iostream>
using namespace std;
#endif//WINDOWS

Configuration::Configuration()
{
	QFile opmlFile( "./config.xml" );
	if ( !opmlFile.open( IO_ReadOnly ) )
	{
		cout << "Critical Error!\n->Cannot open config-file<-" << endl;
		exit(0);
	}

	if ( !domTree.setContent( &opmlFile ) )
	{
		cout << "Critical Error!\n->Parsing error for config-file<-" << endl;
		opmlFile.close();
		exit(0);
	}
	opmlFile.close();



}

///\todo the solution in the case of not finding a value isnt that smart. it have to be checkable if the option was found or not!
QString Configuration::getValue(QString section, QString option)
{
	QDomElement root = domTree.documentElement();
	node = root.firstChild();
	while ( !node.isNull() )
	{
		if ( node.isElement() && node.nodeName() == section )
		{
			element = node.toElement();
			node2 = element.firstChild();
			while ( !node2.isNull() )
			{
				if ( node2.isElement() )
				{
					// case for the different header entries
					if ( node2.nodeName() == option )
					{
						return node2.firstChild().nodeValue();
					}
				}
				node2 = node2.nextSibling();
			}
		}
		if (node.nodeName() == option )
			return node.firstChild().nodeValue();

		node = node.nextSibling();
	}
	return "No value for option " + option + " found!";
}


bool Configuration::setValue(QString section, QString option, QString newValue)
{
	QDomElement root = domTree.documentElement();
	node = root.firstChild();
	while ( !node.isNull() )
	{
		if ( node.isElement() && node.nodeName() == section )
		{
			element = node.toElement();
			node2 = element.firstChild();
			while ( !node2.isNull() )
			{
				if ( node2.isElement() )
				{
					// case for the different header entries
					if ( node2.nodeName() == option )
					{
						node2.firstChild().setNodeValue(newValue);
						return TRUE;
					}
				}
				node2 = node2.nextSibling();
			}
		}
		if (node.nodeName() == option )
			return node.firstChild().nodeValue();

		node = node.nextSibling();
	}
	return FALSE;
}


bool Configuration::saveOptions(QString filename)
{
	QFile file(filename);
    if ( file.open( IO_WriteOnly ) ) {
        QTextStream stream( &file );
        stream << domTree.toString() << "\n";
        file.close();
		return TRUE;
    }
	else
		return FALSE;
}
Configuration& config()
{
	static Configuration conf;
	return conf;
}
