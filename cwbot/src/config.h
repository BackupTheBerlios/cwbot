/* config.h contains the a class for parsing a config file
 * in simple xml.
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

#ifndef CONFIG_H
#define CONFIG_H

#include <qobject.h>
#include <qdom.h>
#include <qstring.h>

///\todo config.cpp has to be documented (Implementation not definition!)!

///Singelton-Class wich is responsible for configuration options

///This class reads the configuration data from a xml-file,
///stores its content in a QDomDocument and returns
///the value for an option requestet by Configuration::getValue()
///
///Since its a singelton-class the constructor is private
class Configuration : public QObject
{

	Q_OBJECT
public:
	///Function to be used by other parts/classes etc. of the program
	///to retrieve configuration options
	QString getValue(QString section, QString option);
	bool setValue(QString section, QString option, QString newValue);

	bool saveOptions(QString filename = "./config.xml");
private:
	Configuration();
	Configuration(const Configuration&);
	///Function which returns a pointer to a/the instance of the Configuration object.
	///If the object does not yet exist, this function creates one.
	friend Configuration& config();
	QDomNode node;
	QDomNode node2;
	QDomElement element;
	QDomDocument domTree;
};
///Function which returns a pointer to a/the instance of the Configuration object.
///If the object does not yet exist, this function creates one.
Configuration& config();
#endif
