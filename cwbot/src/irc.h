/* irc.h provides a class to do basic irc stuff such as joining channels or
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


#ifndef IRC_H
#define IRC_H


#include <qobject.h>
#include <qsocket.h>
#include <qtextstream.h>


///the irc-class providing irc functionatity

///irc is the class responsible for the communication between
///game-modules/cwbot-class and the IRC (-channel)
class irc : public QObject
{
	Q_OBJECT

public:
	irc();
	~irc();

	///Funktion to join IRC
	void start();

	///Function to leave IRC
	void irc_closeConnection();

	void setChannel(QString newChan){chan=newChan;};
	void setNick(QString newNick){nick=newNick;};
	void setAlternativeNick(QString newAltNick){altnick=newAltNick;};
	void setHost(QString newHost){host=newHost;};
	void setPassword(QString newPassword){password=newPassword;};
	void setHostPort(Q_UINT16 newPort){port=newPort;};
	void setUsername(QString newUsername){username=newUsername;};
	void setRealname(QString newRealname){realname=newRealname;};
	void setVerbose(Q_UINT16 newVerbose){verbose=newVerbose;};
	///(re)sets the default-values for channel,nickname,irc-server,server-password,
	///server-port,username,realname,verbosity
	void setDefaults();

private:
	void msgReceived(QString who, QString to, QString what);

	QSocket *irc_socket;

	QString logMsg_buf;

	///this is the string which holds the nick!~user@host for the admin
	QString admin;

	QString host;
	Q_UINT16 port;
	QString nick;
	QString altnick;
	QString chan;
	QString username;
	QString realname;
	QString password;
	Q_UINT16 verbose;

	//strings for received msg (see irc::slotReadyRead() )

	QString who;	///who has sent something
	QString to;		///who has received it (the whole chan or just we)
	QString cmd;	///What command was send (things like '366','353',JOIN,PRIVMSG)
	QString params;	///What params does the cmd has...

private slots:
	///\todo the parsing isnt that smart...
	void slotReadyRead();
	void slotConnected();
	void slotSocketClosed();
	void slotConnectionClosed();
	void slotSocketError(int e);

public slots:
	///can be connected to a signal that wants to send a msg to irc

	///The message passed to this function is directly send into the
	///channel the bot resides in.
	void slotSendMsg(const QString&);
	///connection between the cwbot-object and the remote-irc-user

	///The string is passed to the irc-class. then the irc-class looks
	///if the user is authorized to issue a command, and if he is, actually
	///executes his command.
	void slotWriteToRemote(const QString&);

signals:
	///this is emitted when something has to be loged
	void signalLogMsg(const QString&);
	///this signal is emitted when the bot joined the channels
	void signalChannelJoined();
	///this signal is emitted when someone said something in the channel
	void signalMsgReceived(QString user,QString msg);
	///this signal is used to control the bot trough irc
	void signalCmdReceived( const QString& );
};

#endif /* IRC_H */
