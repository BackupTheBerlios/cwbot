/* cwbot.cpp implements the main class for cwbot, which "connects"
 * module-classes such as irc or cs.
 * This is the class which is the only one used by the frontend.
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

#include <qsocket.h>
#include <qtextstream.h>
#include <qobject.h>

#include <qxml.h>
#include <qdom.h>
#include <qmessagebox.h>
#include <qfiledialog.h>

#ifdef WINDOWS
#include <iostream.h>
#else
#include <iostream>
using namespace std;
#endif //WINDOWS

#include "cwbot.h"

cwbot::cwbot()
{
	//shall our output be verbose?
	verbose=config().getValue("general","verbose").toInt();
	
	//create objects and connections
	irc_object = new irc;
	
	cs_object = new cs;
	hl_rcon_object = new hl_rcon();
	warplaner_object = new warplaner();

	//If the irc-object has something to log, it is redirected to slotLogMsg()
	connect( irc_object, SIGNAL(signalLogMsg(const QString&)), this, SLOT(slotLogMsg(const QString&)) );
	//If the bot joined the channel successfully, the cs-module is started by slotStartCsModule
	connect( irc_object, SIGNAL(signalChannelJoined()), this, SLOT(slotStartCsModule()) );
	connect( irc_object, SIGNAL(signalCmdReceived(const QString&)),this, SLOT(slotCmdReceived(const QString&)));

	//If the cs-object wants to send a message, its redirected to irc
	//This can also be used to send messages from the cs-modules to i.e. a remote control
	connect( cs_object, SIGNAL(sendToIrc(const QString&)),irc_object, SLOT(slotSendMsg(const QString&)));
	connect( cs_object, SIGNAL(logMsg(const QString&)),this, SLOT(slotLogMsg(const QString&)));

	connect( hl_rcon_object, SIGNAL(logMsg(const QString&)),this, SLOT(slotLogMsg(const QString&)));

	//Since irc is our default remote-control, messages to the remote are send to/through irc
	connect(this,SIGNAL(signalWriteToRemote(const QString&)),irc_object, SLOT(slotWriteToRemote(const QString&)));

	connect(warplaner_object,SIGNAL(signalWriteToIrc(const QString&)),irc_object,SLOT(slotWriteToRemote(const QString& )));
	is_running=false;
	start();

	
	//war *moooooooh = warplaner_object->getClanwar(warplaner_object->getNextClanwarId());

	//prints the third name and state....
	//playerlist::iterator it = moooooooh->list.at(3);
	//cout << (*it).name().latin1() << ", " << (*it).state().latin1() << endl;
	//print all players...
	//for ( it = moooooooh->list.begin(); it != moooooooh->list.end(); ++it )
	//{
	//	cout << (*it).name().latin1() << ", " << (*it).state().latin1() << endl;
	//}
}

cwbot::~cwbot()
{}

void cwbot::start()
{

	//eventually adjust the options for the irc-object
	//irc_object->setChannel(config().getValue("irc","channel"));
	//irc_object->setNick(config().getValue("irc","nick"));
	//irc_object->setAlternativeNick(config().getValue("irc","altNick"));
	irc_object->setHost(config().getValue("irc","server"));
	//irc_object->setPassword(config().getValue("irc","password"));
	//irc_object->setHostPort(config().getValue("irc","port").toInt());
	//irc_object->setUsername(config().getValue("irc","username"));
	//irc_object->setRealname(config().getValue("irc","realname"));
	irc_object->setVerbose(config().getValue("general","verbose").toInt());
	irc_object->start();
	
	//eventually adjust the options for the hl_rcon-object
	//hl_rcon_object->setLocalPort(1501);
	//hl_rcon_object->setHost(config().getValue("csServer","ip"));
	//hl_rcon_object->setPort(config().getValue("csServer","port").toInt());
	//hl_rcon_object->setPassword(config().getValue("csServer","rconPassword"));

	hl_rcon_object->start();
}

void cwbot::stop()
{
	//exiting...
	irc_object->irc_closeConnection();
	cs_object->stop();
}

void cwbot::slotCmdReceived(const QString& cmd)
{
	emit signalLogMsg(cmd);
	if(cmd=="stop")
	{
		emit signalWriteToRemote("Bot is stoping...\n");
		stop();
	}
	else if(cmd=="nextcw")
	{
		war *moooooooh = warplaner_object->getClanwar(warplaner_object->getNextClanwarId());
		playerlist::iterator it;
		emit signalWriteToRemote("The next clanwar:\n");
		emit signalWriteToRemote("Players are:\n");
		for ( it = moooooooh->list.begin(); it != moooooooh->list.end(); ++it )
		{
			emit signalWriteToRemote((*it).name() + "[" + (*it).state() + "]\n");
			cout << (*it).name().latin1() << "(" << (*it).state().latin1() << ")" << endl;
		}
	}
	else if(cmd.left(9)=="addplayer")
	{
		emit signalWriteToRemote("addplayer is not implemented yet...\n");
	}
	else if(cmd=="savewars")
	{
		emit signalWriteToRemote("Saving wars to file...\n");
		emit signalWriteToRemote(warplaner_object->saveWars("./wars.xml") + "\n");
	}
	else
	{
		emit signalWriteToRemote("Sorry, no command '" + cmd + "' found!\n");
		emit signalWriteToRemote("Possible options are:\n");
		emit signalWriteToRemote("	'stop'  -- stops the bot\n");
		emit signalWriteToRemote("	'nextcw'  -- shows the next cw\n");
		emit signalWriteToRemote("	'addplayer <warID>'  -- adds a player to a specific war.\n");
		emit signalWriteToRemote("	'savewars'  -- saves the cws.\n");
	}
}

void cwbot::slotNewRemoteClient()
{
	if(verbose>=1)
		emit signalLogMsg("newRemoteClient()");
	emit signalWriteToRemote("You have now control over this bot!");
}

///emits cwbot::signalLogMsg(const QString& msg)
void cwbot::slotLogMsg(const QString& msg)
{
	emit signalLogMsg(msg);
}

void cwbot::slotLogMsgToConsole(const QString& msg)
{
	cout << msg << endl;
}

void cwbot::slotStartCsModule()
{
	cs_object->start(config().getValue("csServer","localIP"));
}

