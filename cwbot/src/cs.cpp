/* cs.cpp implements the cs class to provide access to Counter-Strike logs
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

#include "cs.h"

cs::cs()
{
	setVerbose(2);
	initPlayerList();
}

cs::~cs()
{
}

void cs::start(QString localIP)
{
	if(verbose>=1)
		emit logMsg("localIP: " + localIP);
		
	addr.setAddress(localIP);
	qsd = new QSocketDevice(QSocketDevice::Datagram);
	// Now BIND, not CONNECT
	if (!qsd->bind( addr, 1500))//0.0.0.0:1500
	{
		//qFatal("Could not bind socket for cs-logs!");
	}
	qsn = new QSocketNotifier(qsd->socket(),QSocketNotifier::Read );
	// Connect the Notifier to the incming data slot
	connect(qsn, SIGNAL( activated(int) ), this, SLOT( slotIncomingData(int) ) );
	//emit sendToIrc("Initiated cs-module...");

	active=true;
}

void cs::stop()
{
	delete qsd;
	delete qsn;
	emit sendToIrc("Shut down cs-module...");
	active=false;
}

void cs::initPlayerList()
{
	qsd = new QSocketDevice(QSocketDevice::Datagram);
	if (!qsd->bind( addr, 1502))//0.0.0.0:1501
	{
		qFatal("Could not bind socket for cs!");
	}
	qsn = new QSocketNotifier(qsd->socket(),QSocketNotifier::Read );
	// Connect the Notifier to the incming data slot
	connect(qsn, SIGNAL( activated(int) ), this, SLOT( slotIncomingRconData(int) ) );

	//ask for a rcon challenge
	QHostAddress tmp_host_addr;
	QString host="192.168.1.1";
	tmp_host_addr.setAddress(host);
	QString temp = "\xff\xff\xff\xff" ;
	temp += "challenge rcon\n";
	qsd->writeBlock(temp,temp.length(), tmp_host_addr, 27016);
}

void cs::slotIncomingRconData(int)
{
	QCString temp;
	QString mooh;
	temp.resize( qsd->bytesAvailable() );
	qsd->readBlock( temp.data(), qsd->bytesAvailable());
	mooh=temp;
	if(mooh.contains("challenge rcon")>=1)
	{
		QString temp = "\xff\xff\xff\xff";
		temp += "rcon ";
		temp += mooh.right(11).left(10); //challenge
		temp += " gentoorulez status";
		//send real cmd
		QHostAddress tmp_host_addr;
		QString host="192.168.1.1";
		tmp_host_addr.setAddress(host);
		qsd->writeBlock(temp,temp.length(), tmp_host_addr, 27016);
	}

	else if(mooh.contains("Bad rcon_password")>=1)
		emit logMsg("Cannot innitialise players: Bad rcon_password");

	else if(mooh.contains("name userid uniqueid frag time ping loss adr")>=1)
	{
		emit logMsg("mooh: " + mooh +"-----------------");
		//real initialisation of the list
		mooh=mooh.mid(mooh.find("name userid uniqueid frag time ping loss adr",0),mooh.length());
		mooh=mooh.mid(mooh.find('\n',1),mooh.length());
		//now cut the block in lines
		int x=0;
		while(x<5)
		{
			int tmpint1 = mooh.find('\n',1);
			QString line = mooh.mid(1,tmpint1);
			emit logMsg("line1: " + line);
			int tmpint2 = mooh.find('\n',tmpint1+2);
			line = mooh.mid(tmpint1+1,tmpint2);
			emit logMsg("line2: " + line);

			x++;
		}
		emit logMsg(mooh);
	}
	else
		emit logMsg(mooh);
}

void cs::slotIncomingData(int)
{
	if(active)
	{
		QCString temp;
		temp.resize( qsd->bytesAvailable() );
		qsd->readBlock( temp.data(), qsd->bytesAvailable());
		doOutput(parseLog(temp));
	}
}

void cs::doOutput(log_reply_data reply)
{

	if(reply.action!="")
	{
		// <- color-code (mirc,BitchX,ircii...
		if(reply.action=="kill")
		{
			if(reply.team1=="ct" && reply.team2=="terror")
			{
				emit sendToIrc("0,112,1" + reply.nick1  + "0,1 tötete 0,14,1" + reply.nick2 + "0,1 mit " + reply.weapon);
				emit logMsg(reply.nick1  + "[ct] tötete " + reply.nick2 + "[t] mit " + reply.weapon);
			}

			if(reply.team1=="terror" && reply.team2=="ct")
			{
				emit sendToIrc("0,14,1" + reply.nick1  + "0,1 tötete 0,112,1" + reply.nick2 + "0,1 mit " + reply.weapon);
				emit logMsg(reply.nick1  + "[t] tötete " + reply.nick2 + "[ct] mit " + reply.weapon);
			}

			if(reply.team1=="terror" && reply.team2=="terror")
			{
				emit sendToIrc("0,14,1" + reply.nick1  + "0,1 tötete 0,14,1" + reply.nick2 + "0,1 mit " + reply.weapon);
				emit logMsg(reply.nick1  + "[t] tötete " + reply.nick2 + "[t] mit " + reply.weapon);
			}

			if(reply.team1=="ct" && reply.team2=="ct")
			{
				emit sendToIrc("0,112,1" + reply.nick1  + "0,1 tötete 0,112,1" + reply.nick2 + "0,1 mit " + reply.weapon);
				emit logMsg(reply.nick1  + "[ct] tötete " + reply.nick2 + "[ct] mit " + reply.weapon);
			}
		}

		else if(reply.action=="hasbomb")
		{
			emit sendToIrc("0,14,1" + reply.nick1 + "0,1 hat die Bombe.");
			emit logMsg(reply.nick1 + " hat die Bombe.");
		}
		else if(reply.action=="planted")
		{
			emit sendToIrc("0,14,1" + reply.nick1 + "0,1 hat die Bombe gelegt!");
			emit logMsg(reply.nick1 + " hat die Bombe gelegt!");
		}

		else if(reply.action=="withoutkit")
		{
			emit sendToIrc("0,112,1" + reply.nick1 + "0,1 entschärft die Bombe ohne kit");
			emit logMsg(reply.nick1 + "entschärft die Bombe ohne kit");
		}

		else if(reply.action=="start")
		{
			emit sendToIrc("0,1Runde geht los!");
			emit logMsg("Runde geht los!");
		}

		else if(reply.action=="end")
		{
			emit sendToIrc("0,1Runde is zuende!");
			emit logMsg("Runde is zuende!");
		}

		else if(reply.action=="bombed")
		{
			emit sendToIrc("0,1Bombe is explodiert. 12,1[CTs:" + reply.score_ct + "] 4,1[Ts:" + reply.score_t + "]");
			emit logMsg("Bombe is explodiert.  [CTs:" + reply.score_ct + "] [Ts:" + reply.score_t + "]");
		}

		else if(reply.action=="ctwin")
		{
			emit sendToIrc("0,1CTs haben gewonnen. 12,1[CTs:" + reply.score_ct + "] 4,1[Ts:" + reply.score_t + "]");
			emit logMsg("CTs haben gewonnen.[CTs:" + reply.score_ct + "] [Ts:" + reply.score_t + "]");
		}

		else if(reply.action=="twin")
		{
			emit sendToIrc("0,1Ts haben gewonnen. 12,1[CTs:" + reply.score_ct
			               + "] 4,1[Ts:" + reply.score_t + "]");
			emit logMsg("Ts haben gewonnen.[CTs:" + reply.score_ct
			            + "] [Ts:" + reply.score_t + "]");
		}
		else if(reply.action=="cvar")
		{}
	}

}

log_reply_data cs::parseLog(QString msg)
{
	log_reply_data log_reply;
	log_reply.nick1="";
	log_reply.nick2="";
	log_reply.team1="";
	log_reply.team2="";
	log_reply.id1="";
	log_reply.id2="";
	log_reply.action="";
	log_reply.weapon="";
	log_reply.score_ct="";
	log_reply.score_t="";

	QString temp;
	int tmpint1,tmpint2;

	msg = msg.right(msg.length()-msg.find(": ",0)-2);
	//test contains now only the 'command' (not the leading crap)

	//test= "NAME <FRAGS><ID><TEAM>" killed ... with "WAFFE"
	//emit logMsg("msg: " + msg);
	if(msg.contains(" killed"))
	{
		log_reply.action="kill";

		//first player
		//temp = msg.section( '"', 1, 1 );
		temp=msg.mid(1,msg.find('"',1)-1);
		//<- section is only qt>=3 :( so, no windows/qt2.3

		if(temp.contains("<CT>")>=1)
			log_reply.team1="ct";

		if(temp.contains("<TERRORIST>")>=1)
			log_reply.team1="terror";

		log_reply.nick1=temp.mid(0,temp.find('<',1));

		//second player
		tmpint1=msg.find('"',1);
		tmpint1=msg.find('"',tmpint1+1);

		tmpint2=msg.find('<',1);
		tmpint2=msg.find('<',tmpint2+1);
		tmpint2=msg.find('<',tmpint2+1);
		tmpint2=msg.find('<',tmpint2+1);

		//emit logMsg("temp: " + msg.right(tmpint1));
		temp = msg.right(tmpint1);

		if(temp.contains("<CT>")>=1)
			log_reply.team2="ct";

		if(temp.contains("<TERRORIST>")>=1)
			log_reply.team2="terror";

		log_reply.nick2=msg.mid(tmpint1+1,tmpint2-tmpint1-1);

		//weapon
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint2=msg.find('"',tmpint1+1);
		log_reply.weapon=msg.mid(tmpint1+1,tmpint2-tmpint1-1);

	} // killed

	else if(msg.contains("\"Spawned_With_The_Bomb\"")>=1)
	{
		log_reply.action="hasbomb";
		log_reply.team1="terror";
		temp=msg.mid(1,msg.find('"',1)-1);
		log_reply.nick1=temp.mid(0,temp.find('<',1));
	}

	else if(msg.contains("\"Round_End\"")>=1)
	{
		log_reply.action="end";
	}

	else if(msg.contains("\"Round_Start\"")>=1)
	{
		log_reply.action="start";
	}

	else if(msg.contains("\"Planted_The_Bomb\"")>=1)
	{
		log_reply.action="planted";
		log_reply.team1="terror";
		temp=msg.mid(1,msg.find('"',1)-1);
		log_reply.nick1=temp.mid(0,temp.find('<',1));
	}

	else if(msg.contains("\"Dropped_The_Bomb\"")>=1)
	{
		log_reply.action="dropped";
		log_reply.team1="terror";
		temp=msg.mid(1,msg.find('"',1)-1);
		log_reply.nick1=temp.mid(0,temp.find('<',1));
	}

	else if(msg.contains("\"Got_The_Bomb\"")>=1)
	{
		log_reply.action="gotbomb";
		log_reply.team1="terror";
		temp=msg.mid(1,msg.find('"',1)-1);
		log_reply.nick1=temp.mid(0,temp.find('<',1));
	}

	else if(msg.contains("\"Target_Bombed\"")>=1)
	{
		log_reply.action="bombed";
		tmpint1=msg.find('"',1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint2=msg.find('"',tmpint1+1);
		log_reply.score_ct=msg.mid(tmpint1+1,tmpint2-tmpint1-1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint2=msg.find('"',tmpint1+1);
		log_reply.score_t=msg.mid(tmpint1+1,tmpint2-tmpint1-1);
	}

	else if(msg.contains("\"Begin_Bomb_Defuse_Without_Kit\"")>=1)
	{
		log_reply.action="withoutkit";
		log_reply.team1="ct";
		temp=msg.mid(1,msg.find('"',1)-1);
		log_reply.nick1=temp.mid(0,temp.find('<',1));
	}

	else if(msg.contains("\"Touched_A_Hostage\"")>=1)
	{
		log_reply.action="touchedtkit";
		/// \todo if "Touched_A_Hostage" the team is not right (see FIXME)
		log_reply.team1="df"; //FIXME
		temp=msg.mid(1,msg.find('"',1)-1);
		log_reply.nick1=temp.mid(0,temp.find('<',1));
	}

	else if(msg.contains("\"Defused_The_Bomb\"")>=1)
	{
		log_reply.action="defused";
	}

	else if(msg.contains("\"CTs_Win\"")>=1)
	{
		log_reply.action="ctwin";
		tmpint1=msg.find('"',1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint2=msg.find('"',tmpint1+1);
		log_reply.score_ct=msg.mid(tmpint1+1,tmpint2-tmpint1-1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint2=msg.find('"',tmpint1+1);
		log_reply.score_t=msg.mid(tmpint1+1,tmpint2-tmpint1-1);
	}

	else if(msg.contains("\"Terrorists_Win\"")>=1)
	{
		log_reply.action="twin";
		tmpint1=msg.find('"',1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint2=msg.find('"',tmpint1+1);
		log_reply.score_ct=msg.mid(tmpint1+1,tmpint2-tmpint1-1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint2=msg.find('"',tmpint1+1);
		log_reply.score_t=msg.mid(tmpint1+1,tmpint2-tmpint1-1);
	}

	else if(msg.contains("\"Round_Draw\"")>=1)
	{
		log_reply.action="draw";
		tmpint1=msg.find('"',1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint2=msg.find('"',tmpint1+1);
		log_reply.score_ct=msg.mid(tmpint1+1,tmpint2-tmpint1-1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint1=msg.find('"',tmpint1+1);
		tmpint2=msg.find('"',tmpint1+1);
		log_reply.score_t=msg.mid(tmpint1+1,tmpint2-tmpint1-1);
	}

	else if(msg.contains("Server cvar")>=1)
	{
		log_reply.action="cvar";
	}

	return log_reply;
}

