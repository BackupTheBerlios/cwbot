/* cs.h is a module-class for cwbot to provide access to Counter-Strike logs
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
 
 
#ifndef CS_H
#define CS_H


#include <qobject.h>
#include <qhostaddress.h> 
#include <qsocketdevice.h>
#include <qhostaddress.h>
#include <qsocketnotifier.h> 
#include <qvaluelist.h>

/// log_reply_data contains the log-data in a usable format
struct log_reply_data
{
	///nick first player
	QString nick1;
	///nick second player
	QString nick2;
	///how many frags (nick1)?
	QString frags1;
	///how many frags (nick2)?
	QString frags2;
	///team of nick1
	QString team1;
	///team of nick2
	QString team2;
	///userid of nick1
	QString id1;
	///userid of nick2
	QString id2;
	///action of what happened (like "round_start"/"target_bombed"/"killed")
	QString action;
	///weapon used to kill s.b.
	QString weapon;
	///team-/round-score of the t(errorist)-team
	QString score_t;
	///team-/round-score of the c(ounter)t(errorist)-team
	QString score_ct;
};

///This struct is ment to be used in a qvaluelist to keep track of the players
/*struct player
{
	QString id;
	QString name;
	QString frags;
	QString deaths;
	QString time;
	QString team;
	bool 	hasomb;
};*/

///\todo the cs-class _MUST_ be refactorized if not even rewritten!!

///class to provide the cs-log functionality

///this class converts half-life/counterstrike logs into a usable
///format (struct log_reply_data) and sends some usable strings,
///explaining whats going on at the server, through send_to_irc(const QString&) and
///log_msg(const QString&) to connected slots
class cs : public QObject
{

Q_OBJECT

public:
	cs();
	~cs();
	///function to start the bot

	///this function creates and binds the listening socket.
	void start(QString localIP);
	///function to stop the bot
	void stop();

	void setVerbose(Q_UINT16 newVerbose){verbose=newVerbose;};


private:
	log_reply_data parseLog(QString msg);
	//function to create (and actually send) the output.
	void doOutput(log_reply_data reply);
	void initPlayerList();
	bool active;
	Q_UINT16 verbose;
	
	QHostAddress addr;
	QSocketDevice *qsd;
	QSocketNotifier *qsn;
	
	//QValueList<player> playerList;
	
private slots:
	///This slot is called when the socket receives data
	void slotIncomingData(int);
	///used during initialisation of the player-list
	void slotIncomingRconData(int);

signals:
	///This signal is emitted when some text has to be send to IRC
	void sendToIrc(const QString&);
	///This signal is emitted when some text has to be logged (this is usually the same text that goes to irc but without color-codes)
	void logMsg(const QString&);
};

#endif // CS_H
