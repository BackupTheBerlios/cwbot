/* cwbot.h contains the main class for cwbot, which "connects"
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,USA.
 */

//#ifndef CWBOT_H
//#define CWBOT_H

#include <qobject.h>

#include "irc.h"
#include "cs.h"
#include "config.h"
#include "hl_rcon.h"
#include "warplaner.h"

///main class

///this is the main class for cwbot
///which is the only class that must be used by the front-endl
///application. It provides all things nessesary to run a complete
///clanwar-irc-bot.
class cwbot : public QObject
{
	Q_OBJECT

public:
	cwbot();
	~cwbot();

	///starts the bot

	///cwbot::start() creates the modul-objects (such as irc, cs, hl_rcon),
	///connects the slots and signals used for communication,
	///and finaly starts the created modules.
	void start();

	///stops cwbot

	///cwbot::stop() actually stops the modules which were created
	///in cwbot::start()
	void stop();

	///boolean to check if the bot is/should run

	///This is used to determine if we can spawn a bot.
	///This flag is used in context with the start/stop buttons/menu
	bool is_running;
	int verbose;

private:

	irc *irc_object;
	cs *cs_object;
	hl_rcon *hl_rcon_object;
	warplaner *warplaner_object;
public slots:
	///logs the emitted qstring to console

	///This is useful when running with
	///the --nogui cmdline option, since then the console is our
	///"output-widget" (not a textedit i.e.)
	void slotLogMsgToConsole(const QString&);

private slots:

	void ircSocketError( int e )
	{
		qFatal("Error with the irc socket occured! Error-code: " + e);
	}

	void slotLogMsg(const QString&);
	void slotStartCsModule();
	void slotCmdReceived(const QString&);
	void slotNewRemoteClient();


signals:
	///Signal to log messages

	///This signal is emitted when something
	///(log/debug/status-messages) has to be logged.
	///This signal should be connected with i.e. a textedit or stdout
	///in the main-programm
	void signalLogMsg(const QString&);

	void signalWriteToRemote(const QString&);
};

//#endif
