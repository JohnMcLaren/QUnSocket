/******************************************************************
 * This is a test program for the project - QUnSocket
 * universal Qt/C++ wrapper class for Qt sockets
 *
 * src: https://github.com/JohnMcLaren/QUnSocket
 * license: GPLv3 2007 - https://www.gnu.org/licenses/gpl-3.0.html
 * 27.02.2023 🄯 JML
******************************************************************/
#include <QCoreApplication>
#include "../qunsocket.h"

#define TIMEOUT 3000
#define txt QString

int
main(int argc, char *argv[])
{
QCoreApplication app(argc, argv);
QUnSocket socket;
QUrl url;

	/*** Connect to socket signals for asynchronous receiving of data/reports ***/

	// CONNECT / DISCONNECT slots
	app.connect(&socket, &QUnSocket::connected, &app, [&] () {
		qDebug() << "socket connected, protocol:" << socket.protocol();
	});

	app.connect(&socket, &QUnSocket::encrypted, &app, [&] () {
		qDebug() << "socket encrypted.";
	});

	app.connect(&socket, &QUnSocket::disconnected, &app, [&] () {
		qDebug() << "socket disconnected.";
		//exit(0);
	});
	// ERRORs slots
	app.connect(&socket, &QUnSocket::errorOccurred, &app, [&] (const QAbstractSocket::SocketError error) {
		qDebug() << "socket error:" << error;
	});

	app.connect(&socket, &QUnSocket::sslErrors, &app, [&] (const QList<QSslError> &errors) {
		qDebug() << "socket SSL errors:" << errors;
	});
	// SEND slots
	app.connect(&socket, &QUnSocket::bytesWritten, &app, [&] (const qint64 bytes) {
		qDebug(">>> sended [%lld] bytes", bytes);
	});
	// RECEIVE slots
	// buffered
/* Uncomment this for asynchronous buffered receiving. *
	app.connect(&socket, &QUnSocket::readyRead, &app, [&] () {

		if(socket.protocol() < QUnSocket::WS || socket.canReadLine()) // All exclude QUnWebSocket OR QUnWebSocket received binary
		{
			qDebug("<<< Asynchronous buffered received [%lld] bytes:", socket.bytesAvailable());
			qDebug("%s", socket.readAll().data());
		}
		else // QUnWebSocket text only
		if(socket.canReadLineText())
		{
			qDebug("<<< Asynchronous buffered TEXT received [%lld] bytes:", socket.bytesAvailableText());
			qDebug("%ls", socket.readAllText().utf16());
		}
	});
//*/
	// non-buffered
	app.connect(&socket, &QUnSocket::received, &app, [&] (const QByteArray &data) {
		qDebug("<<< Asynchronous non-buffered received [%d] bytes", data.size());
		qDebug() << data;
	});
	// non-buffered (WebSockets only)
	app.connect(&socket, &QUnSocket::receivedText, &app, [&] (const QString &text) {
		qDebug("<<< Asynchronous non-buffered TEXT received [%d] bytes", text.size());
		qDebug("%ls", text.utf16());
	});

	/*** Test protocols ***/
	try
	{
	int proto =0;

		while(++proto <= QUnSocket::WSS)
		{
		QString request;
		bool bMultiPartReceive =false;

			switch(proto)
			{
			case QUnSocket::UNIX:
				qDebug("---------- Test UNIX-socket ----------");
				url ="unix:/tmp/test.sock"; /*** RUN UNIX-socket server: $ nc -U /tmp/test.sock -l ***/
				request ="hellou UNIX!!\n";
				break;
			case QUnSocket::HTTP:
				qDebug("---------- Test HTTP-socket ----------");
				url ="http://tcpbin.org/";
				bMultiPartReceive =true;
				request =txt("GET %1 HTTP/1.1\r\n").arg(url.path())
						+ txt("Host: %1\r\nConnection: close\r\n\r\n").arg(url.host());
				break;
			case QUnSocket::HTTPS: // secure TCP (TLS/TCP)
				qDebug("---------- Test HTTPS-socket ----------");
				url ="https://tcpbin.com/";
				request =txt("GET %1 HTTP/1.1\r\n").arg(url.path())
						+ txt("Host: %1\r\nConnection: close\r\n\r\n").arg(url.host());
				break;
			case QUnSocket::TCP:
				qDebug("---------- Test raw TCP-socket ----------");
				url ="tcp://tcpbin.com:4242";
				request ="hellou TCP!!\n"; // end with '\n' mandatory
				break;
			case QUnSocket::UDP:
				qDebug("---------- Test raw UDP-socket ----------");
				url ="udp://localhost:40000"; /*** RUN UDP-socket server: $ nc -u -p 40000 -l ***/
				request ="hellou UDP!!\n";
				break;
			case QUnSocket::WS:
				qDebug("---------- Test WS-socket ----------");
				url ="ws://achex.ca:4010";
				request ="{\"SID\": 68888, \"ID\": \"Bob\"}";
				break;
			case QUnSocket::WSS:
				qDebug("---------- Test WSS-socket ----------");
				url ="wss://polkadot.webapi.subscan.io/socket";
				break;
			}

		//... Do coonect and request ...

			if(!socket.connectToServer(url, TIMEOUT))
				throw(txt("Connect to %1").arg(url.toString()));
			// Send if there is data to request
			if(!request.isEmpty())
				socket.write(request);
			// Waiting for a reply and reading it
			do
			{
				if(!socket.waitForReadyRead(TIMEOUT)) // if timeout OR disconnect while waiting for ReadyRead()
				{
					if(socket.isConnected())
						qDebug("ReadyRead TIMEOUT");

					break; // disconnect: it is normal behavior for multi-parts receiving.
				}

				if(socket.protocol() < QUnSocket::WS) // All sockets exclude WebSockets
				{
					qDebug("<<< Synchronous buffered received [%lld] bytes:", socket.bytesAvailable());
					qDebug() << socket.readAll();
				}
				else // WebSockets only
				{
					qDebug("<<< Synchronous buffered TEXT received [%lld] bytes:", socket.bytesAvailableText());
					qDebug() << socket.readAllText();
				}

			} while(bMultiPartReceive); // If the reply consists of several parts, then repeat the reading procedure
			//break; // DEBUG
		}

		qDebug() << "========== OK.EXIT ==========";
	}
	catch(const QString &error)
	{
		qDebug("FAIL on: %ls", error.utf16());
		exit(0);
	}

//return app.exec();
}

