#ifndef DOCK_H
#define DOCK_H

#include <string>
#include <vector>

#include <QDockWidget>
#include <QPlainTextEdit>
#include <QTcpServer>

#include "window.h"
#include "smartcard.hpp"

class cardDock : public QDockWidget {
	Q_OBJECT
public:
	cardDock(QString filename, MainWindow *parent, card *sc);
	~cardDock();

private slots:
	void accept_connection();						//Accepts incoming connection
	void api_call();							//Processes the api calls

private:
	void openLog();
	void startServer();
	bool authorized(std::string command, std::vector<std::string> args);	//Requests authorization from the user
	void log_command(std::string command, bool status);			//Logs if the command succeded or failed
	MainWindow *parent;
	QString file;
	card *sc;
	QPlainTextEdit *log;
	QTcpServer *server;
	QTcpSocket *client;
};

#endif

