/* hl_rcon.h provides the hl_rcon class which can be used to send
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
 
 
#ifndef HL_RCON_H
#define HL_RCON_H
 

#include <qobject.h>
#include <qsocketdevice.h>
#include <qsocketnotifier.h>
#include <qhostaddress.h>
///\todo source needs to be refactorized/rewritten

///hl_rcon provides access to the half-life rcon protocol
 
///This can be used to issue remote commands to a Half-Life server
class hl_rcon : public QObject
{
	Q_OBJECT
	
public:
	void start();
	///creates the socket
	hl_rcon();
	///is used to send a rcon-command to the server

	///Sending commands with this function is the same as typing them on
	///the servers console. If we have a hl_rcon object called hl_rcon_object
	///then making the server say "mooh" a simple hl_rcon_object->sendCmd("say mooh")
	///does the job. You dont have to worry about rcon-challenges etc.
	///But be aware of using false passwords too often. the server might block
	///you ip for rcon-requests for a certain amount of time.
	void sendCmd(QString cmd);

	void setPassword(QString newPassword);
	void setHost(QString newHost);
	void setPort(int newPort);
	void setLocalPort(int newLocalPort);
private:
	///this functionn is called from incomingData(int) when a response
	///from the server contains "rcon challenge"
	void sendRealCmd(QString challenge);

	///this is the rcon-password
	QString password;
	///the host (ip-address of the server)
	QString host;
	///port the server runs on
	int	port;
	///the local port the bot should listen on
	int	localPort;

	QSocketDevice *qsd;
	QSocketNotifier *qsn;
	QHostAddress addr;

	QString command;
	
signals:
	///signal to log text
	void logMsg(const QString&);

private slots:
	void incomingData(int);
};


#endif
