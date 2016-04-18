#include <unordered_map>

#include <QTcpSocket>
#include <QInputDialog>

#include "dock.h"
#include "card_exceptions.hpp"
#include "../base64/base64.h"

using namespace std;

cardDock::cardDock(QString filename, MainWindow *parent, card *sc) : QDockWidget(filename, parent), file(filename)
{
	this->sc = sc;
	this->parent = parent;
	openLog();
	startServer();

	connect(server, SIGNAL(newConnection()), this, SLOT(accept_connection()));
}

cardDock::~cardDock()
{
	delete widget();
	delete sc;
	delete server;
}

void cardDock::accept_connection()
{
	client = server->nextPendingConnection();
	connect(client, SIGNAL(disconnected()), client, SLOT(deleteLater()));
	connect(client, SIGNAL(readyRead()), this, SLOT(api_call()));
}

void cardDock::api_call()
{
	int i=0;
	char tmp;
	unordered_map<string, short> commands = {{"gen_key", 1}, {"gen_trusted_key", 1}, {"sign", 2}, {"del_key", 1}, {"set_pin", 1}, {"get_random", 1}, {"get_logs", 0}, {"get_version", 0}};
	unordered_map<string, short> command_index = {{"gen_key", 1}, {"gen_trusted_key", 2}, {"sign", 3}, {"del_key", 4}, {"set_pin", 5}, {"get_random", 6}, {"get_logs", 7}, {"get_version", 8}};
	string command;
	vector<string> args;
	QByteArray buffer;

	//Read the command to be executed
	while (client->getChar(&tmp) && tmp != '\n')
		command += tmp;

	//Check if the command is valid
	if (commands.find(command) == commands.end()) {
		buffer.append((char)true);
		buffer.append("Invalid command");
		client->write(buffer);
		log->appendPlainText("Invalid command received.");
		client->disconnectFromHost();
		return;
	}

	//Read the arguments
	for (i=0; i<commands[command]; i++) {
		string tmps;
		while (client->getChar(&tmp) && tmp != '\n')
			tmps += tmp;
		args.push_back(tmps);
	}

	//Validate inputs and request authorization from the user
	for (i=0; i<commands[command]; i++)
		if (args[i].empty()) {
			buffer.append((char)true);
			buffer.append("Invalid arguments");
			client->write(buffer);
			log_command(command, true);
			log->appendPlainText(" Invalid arguments.");
			client->disconnectFromHost();
			return;
		}

	if (command != "get_random" && command != "get_version" && !authorized(command, args)) {
		client->putChar(true);
		log->appendPlainText("Command not authorized.");
		client->disconnectFromHost();
		return;
	}

	//Execute the command
	switch (command_index[command]) {
		case 1:
		{
			string out;

			try {
				out = sc->gen_key(args[0]);
			} catch (KeyExists e) {
				buffer.append((char)true);
				buffer.append(e.what());
				log_command(command, true);
				log->appendPlainText(e.what());
				break;
			}
			buffer.append((char)false);
			buffer.append(out.c_str());
			log_command(command, false);
			parent->save(file, sc);
			break;
		}
		case 2:
		{
			string out;

			try {
				out = sc->gen_trusted_key(args[0]).c_str();
			} catch (KeyExists e) {
				buffer.append((char)true);
				buffer.append(e.what());
				log_command(command, true);
				log->appendPlainText(e.what());
				break;
			}
			buffer.append((char)false);
			buffer.append(out.c_str());
			log_command(command, false);
			parent->save(file, sc);
			break;
		}
		case 3:
		{
			string msg = base64_decode(args[1]);
			string out;

			try {
				out = sc->sign(reinterpret_cast<const unsigned char *>(msg.c_str()), msg.length(), args[0]);
			} catch (KeyNotExists e) {
				buffer.append((char)true);
				buffer.append(e.what());
				log_command(command, true);
				log->appendPlainText(e.what());
				break;
			}
			buffer.append((char)false);
			buffer.append(out.c_str());
			log_command(command, false);
			break;
		}
		case 4:
			try {
				if (sc->del_key(args[0])) {
					buffer.append((char)true);
					log_command(command, true);
				} else {
					buffer.append((char)false);
					log_command(command, false);
					parent->save(file, sc);
				}
			} catch (KeyNotExists e) {
				buffer.append((char)true);
				buffer.append(e.what());
				log_command(command, true);
				log->appendPlainText(e.what());
			}
			break;
		case 5:
			sc->set_pin(args[0]);
			buffer.append((char)false);
			parent->save(file, sc);
			break;
		case 6:
		{
			unsigned short i;
			unsigned int nrbytes=0;
			unsigned char *rand_bytes;
				for (i=0; i<sizeof(unsigned int); i++)
				nrbytes += args[0][i] << (sizeof(unsigned int) - 1 - i);
			rand_bytes = sc->get_random(nrbytes);
			buffer.append((char)false);
			buffer.append(base64_encode(rand_bytes, nrbytes).c_str());
			log_command(command, false);
			delete rand_bytes;
			break;
		}
		case 7:
			buffer.append((char)false);
			buffer.append(log->document()->toPlainText().toLatin1());
			log_command(command, false);
			break;
		case 8:
			buffer.append((char)false);
			buffer.append("0.1a");
			log_command(command, false);
	}

	client->write(buffer);
	client->disconnectFromHost();
}
	
void cardDock::openLog()
{
	log = new QPlainTextEdit;
	log->setReadOnly(true);
	log->setPlainText(QString("Identity: ").append(sc->get_identity().c_str()));
	setWidget(log);
}

void cardDock::startServer()
{
	server = new QTcpServer(this);
	server->listen(QHostAddress::LocalHost);
	log->appendPlainText(QString("Listening on port ").append(QString::number(server->serverPort())));
}

bool cardDock::authorized(string command, vector<string> args)
{
	QString message(("To execute " + command + " with arguments: ").c_str());
	for (unsigned int i=0; i<args.size(); i++)
		message += (args[i] + " ").c_str();
	QString pin = QInputDialog::getText(this, "Authorization Required", message + "\n Insert the cards pin:", QLineEdit::Password);
	return sc->validate_pin(pin.toStdString());
}

void cardDock::log_command(string command, bool status)
{
	QString textlog("Command ");
	textlog += command.c_str();
	if (status)
		textlog += " failed.";
	else
		textlog += " succeeded.";
	log->appendPlainText(textlog);
}

