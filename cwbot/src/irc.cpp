/* irc.cpp implements the irc class to do basic irc stuff such as joining channels or
 * sending messages to channels.
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

#include "irc.h"
#include <iostream>
using namespace std;

irc::irc()
{
	//initialisation of default values
	setDefaults();
}

irc::~irc()
{
	irc_closeConnection();
}

void irc::start()
{
	altnick=nick.left(nick.length()-2) + "23";

	//create new socket and connect signals/slots
	irc_socket = new QSocket( this );
	connect(irc_socket, SIGNAL(connected()),SLOT(slotConnected()) );
	connect(irc_socket, SIGNAL(connectionClosed()),SLOT(slotConnectionClosed()) );
	connect(irc_socket, SIGNAL(readyRead()),SLOT(slotReadyRead()) );
	connect(irc_socket, SIGNAL(error(int)),	SLOT(slotSocketError(int)) );

	irc_socket->connectToHost(host,port);
}


void irc::slotSendMsg(const QString& msg)
{
	// write to the server
	QTextStream os(irc_socket);
	os << "PRIVMSG " << chan << " :" << msg << "\n";
}

void irc::slotWriteToRemote(const QString& msg)
{
	// write to the server
	QTextStream os(irc_socket);
	if(admin)
		os << "PRIVMSG " << admin.section('!',0,0) << " :" << msg << "\n";
}

//private slots anfang
void irc::slotReadyRead()
{
	QString temp,tmp;
	QTextStream os(irc_socket);

	// read from the server
	while (irc_socket->canReadLine() )
	{
		who = cmd = params = "";
		//eine zeile vom server holen
		temp = irc_socket->readLine();
		//letzte zeichen abschneiden (\n)
		temp = temp.left(temp.length()-2);
		if (verbose>=1)
			emit signalLogMsg("irc::slotReadyRead() IRC:" + temp);

		tmp=temp.left(temp.find(" ",1));

		if (tmp=="PING")
		{
			tmp=temp.mid(6,temp.length()-6);
			os << "PONG " << tmp << "\n";
			;
			if(verbose)
				emit signalLogMsg("PING? - PONG!");
		}
		else if(tmp=="NOTICE")
		{}

		//tmp is not PING or NOTICE
		else
		{
			//who has sent a message...
			who=tmp.remove(0,1);
			//the command that was send
			tmp=temp.right(temp.length()-temp.find(" ",0)-1);
			cmd=tmp.left(tmp.find(" ",0));
			//what was send...
			params=tmp.right(tmp.length()-tmp.find(":",0)-1);

			if(verbose>=1)
				emit signalLogMsg("host: >" + host + "<\ncmd: >" + cmd
				                  + "<\nparams: >" + params + "<\n" );
		}

		if(cmd=="376" | cmd=="422")
		{
			emit signalLogMsg("Joining channel..");
			os << "JOIN " << chan << "\n";
		}

		if(cmd=="433")
		{
			emit signalLogMsg("Nickname " + nick + "allready in use!");
			emit signalLogMsg("Changing nick to " + altnick);
			os << "NICK " << altnick << "\n";
		}

		if(cmd=="366")
		{
			//os << "PRIVMSG " << chan << " :" << "Hi folks!" << "\n";
			emit signalLogMsg("Channel joined... start logging...");
			emit signalChannelJoined();
		}

		if(cmd=="403")
		{
			//os << "PRIVMSG " << chan << " :" << "Hi folks!" << "\n";
			emit signalLogMsg("Channel " + chan + " does not exist!");
		}

		if(cmd=="PRIVMSG")
		{
			tmp=temp.right(temp.length()-temp.find(" ",0)-1);
			tmp=tmp.left(tmp.find(" ",8));//PRIVMSG blah
			to = tmp.right(tmp.length()-tmp.find(" ",0)-1);//blah
			msgReceived(who,to,params);
		}
	}
}

void irc::msgReceived(QString who, QString to, QString what)
{
	//We have received a message...
	//Initialise the TextStream to write s.th. to the IRC

	QTextStream os(irc_socket);

	//Parse possible options
	//per '/msg cwbot login wurst' einloggen
	if(to==nick && what=="login wurst")
	{
		os << "PRIVMSG " << who.section('!',0,0) << " :" << who << " is now authed...\n";
		admin = who;
	}
	//if the authed admin says something (privmsg)
	else if(to==nick && admin==who && what!="login wurst")
	{
		emit signalCmdReceived(what);
	}

	else if (to==nick && admin!=who)
	{
		os << "PRIVMSG " << who.section('!',0,0) << " :Sorry," << who.section('!',0,0) << ": You are not logged in...\n";
		os << "PRIVMSG " << who.section('!',0,0) << " :Use \"Login 'password'\" to login\n";
	}

	/*else if (to==chan && admin!=who)
	{
		os << "PRIVMSG " << who.section('!',0,0) << " :Sorry," << who.section('!',0,0) << ": You are not logged in...\n";
		os << "PRIVMSG " << who.section('!',0,0) << " :Use \"Login 'password'\" to login\n";
	}*/

}

void irc::slotConnected()
{
	//wenn der socket connected ist, dann password
	//nick und username senden, damit er uns rein lässt
	//dann nur noch ein ping-pong und wir sind drinnen und
	//können einem channel joinen
	emit signalLogMsg("Connected to IRC-Server. Logging in...");

	QTextStream os(irc_socket);
	os << "PASS " << password  << "\n";
	os << "NICK " << nick << "\n";
	os << "USER " << username << " * 0 :" << realname << "\n";

}

void irc::slotConnectionClosed()
{
	if(verbose>=1)
		emit signalLogMsg("irc::slotConnectionClosed()");
}

void irc::slotSocketClosed()
{
	if(verbose>=1)
		emit signalLogMsg("irc::slotSocketClosed()") ;
}

void irc::slotSocketError( int e )
{
	if(e==QSocket::ErrConnectionRefused)
		QTextOStream( &logMsg_buf) << "irc::slotSocketError(int) : Error numer " << e << " occured! (Conenction Refused)";
	emit signalLogMsg(logMsg_buf);
}
//private slots ende

void irc::irc_closeConnection()
{
	if(irc_socket->state() == QSocket::Connection)
	{
		if(verbose>=1)
			emit signalLogMsg("irc::irc_closeConnection(): Leaving IRC...");

		QTextStream os(irc_socket);
		//os << "PRIVMSG " << chan << " :Shutting down bot..." << "\n";
		//os << "PRIVMSG " << chan << " :Please visit http://cwbot.berlios.de/ !" << "\n";
		//os << "QUIT schuess...\n";
		os << "QUIT Please visit http://cwbot.berlios.de/\n";
		//wir lassen uns vom server disconnecten (QUIT)
		//irc_socket->close();
	}

	if(irc_socket->state() == QSocket::Closing )
	{
		// We have a delayed close.
		connect(irc_socket, SIGNAL(delayedCloseFinished()),SLOT(slotSocketClosed()) );
	}

	else
	{
		// The socket is closed.
		slotSocketClosed();
	}
}

void irc::setDefaults()
{
	//initialisation of default values
	verbose=0;
	host="192.168.1.1";
	nick="cwbot";
	altnick="23cwbot23";
	port=6667;
	chan="#cwbot";
	username="cwbot";
	realname="cwbot";
	password="";
}

