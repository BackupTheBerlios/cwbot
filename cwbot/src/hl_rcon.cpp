/* hl_rcon.cpp implements the hl_rcon class which can be used to send
 * rcon commands to a half-life server.
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


#include "hl_rcon.h"
#ifdef WINDOWS
#include <iostream.h>
#else
#include <iostream>
using namespace std;
#endif //WINDOWS

hl_rcon::hl_rcon()
{
	password="mooh";
	host="192.168.1.1";
	port=27015;
	localPort=1501;
}

void hl_rcon::start()
{
	qsd = new QSocketDevice(QSocketDevice::Datagram);
	if (!qsd->bind( addr, localPort))//0.0.0.0:1501
	{
		//qFatal("Could not bind socket for hl_rcon!");
	}
	qsn = new QSocketNotifier(qsd->socket(),QSocketNotifier::Read );
	// Connect the Notifier to the incming data slot
	connect(qsn, SIGNAL( activated(int) ), this, SLOT( incomingData(int) ) );
}

void hl_rcon::sendCmd(QString cmd)
{
	QHostAddress tmp_host_addr;
	host="192.168.1.1";
	tmp_host_addr.setAddress(host);
	port=27016;
	command=cmd;
	QString temp = "\xff\xff\xff\xff" ;
	temp += "challenge rcon\n";
	qsd->writeBlock(temp,temp.length(), tmp_host_addr, port);
}

void hl_rcon::sendRealCmd(QString challenge)
{
	QHostAddress tmp_host_addr;
	host="192.168.1.1";
	tmp_host_addr.setAddress(host);
	port=27016;
	QString temp = "\xff\xff\xff\xff";
	temp += "rcon ";
	temp += challenge;
	temp += " ";
	temp += password;
	temp += " ";
	temp += command;
	temp += "\n";
	emit logMsg(temp);
	
	qsd->writeBlock(temp,temp.length(), tmp_host_addr, port);
}

void hl_rcon::incomingData(int)
{
	QCString temp;
	QString mooh;
	temp.resize( qsd->bytesAvailable() );
	qsd->readBlock( temp.data(), qsd->bytesAvailable());
	mooh=temp;
	if(mooh.contains("challenge rcon")>=1)
		sendRealCmd(mooh.right(11).left(10));
	if(mooh.contains("Bad rcon_password")>=1)
		emit logMsg("Bad rcon_password");
}

void hl_rcon::setPassword(QString newPassword)
{
	password=newPassword;
}

void hl_rcon::setHost(QString newHost)
{
	host=newHost;
}

void hl_rcon::setPort(int newPort)
{
	port=newPort;
}

void hl_rcon::setLocalPort(int newLocalPort)
{
	localPort=newLocalPort;
}
