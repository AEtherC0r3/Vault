#include <QInputDialog>
#include <QPushButton>
#include <QHostAddress>
#include <QMessageBox>

#include "dialog.h"

using namespace std;

commandDialog::commandDialog()
{
	bool ok;
	serverPort = QInputDialog::getInt(this, "Server Port", "Insert port:", 1024, 1, 65535, 1, &ok);
	if (!ok)
		return;

	layout = new QGridLayout(this);
	createButtons();
	data = new QByteArray;
	socket = new QTcpSocket;
	connect(socket, SIGNAL(connected()), this, SLOT(connected()));
	connect(socket, SIGNAL(readyRead()), this, SLOT(read_response()));
}

void commandDialog::gen_key()
{
	string kname = get_kname();

	if (kname.empty()) {
		failure("Empty key name");
		return;
	}

	data->append("gen_key\n");
	data->append(kname.c_str());

	command_exec();
}

void commandDialog::gen_trusted_key()
{
	string kname = get_kname();

	if (kname.empty()) {
		failure("Empty key name");
		return;
	}

	data->append("gen_trusted_key\n");
	data->append(kname.c_str());

	command_exec();
}

void commandDialog::sign()
{
	string kname = get_kname();
	string msg;

	if (kname.empty()) {
		failure("Empty key name");
		return;
	}

	data->append("sign\n");
	data->append(kname.c_str());
	data->append('\n');

	msg = QInputDialog::getText(this, "Sign message", "Insert a base64 encoded message for signing:").toStdString();
	if (msg.empty()) {
		failure("Empty message");
		return;
	}
	data->append(msg.c_str());

	command_exec();
}

void commandDialog::del_key()
{
	string kname = get_kname();

	if (kname.empty()) {
		failure("Empty key name");
		return;
	}

	data->append("del_key\n");
	data->append(kname.c_str());

	command_exec();
}

void commandDialog::set_pin()
{
	string pin = QInputDialog::getText(this, "Change pin", "Insert new device pin:").toStdString();

	if (pin.empty()) {
		failure("Empty pin");
		return;
	}

	data->append("set_pin\n");
	data->append(pin.c_str());

	command_exec();
}

void commandDialog::get_random()
{
	bool ok;
	unsigned int n = QInputDialog::getInt(this, "Random Bytes", "Insert the number of random bytes you want", 0, 1, 2147483647, 1, &ok);

	data->append("get_random\n");
	data->append(n);

	if (ok)
		command_exec();
}

void commandDialog::get_logs()
{
	data->append("get_logs");
	command_exec();
}

void commandDialog::get_version()
{
	data->append("get_version");
	command_exec();
}

void commandDialog::connected()
{
	socket->write(*data);
}

void commandDialog::read_response()
{
	bool status = false;

	socket->getChar((char *)&status);
	*data = socket->readAll();
	if (status)
		failure(*data);
	else
		success(*data);
}

void commandDialog::createButtons()
{
	QPushButton *button;

	button = new QPushButton("Generate new key pair");
	layout->addWidget(button, 0, 0);
	connect(button, SIGNAL(clicked()), this, SLOT(gen_key()));

	button = new QPushButton("Generate authenticated key pair");
	layout->addWidget(button, 0, 1);
	connect(button, SIGNAL(clicked()), this, SLOT(gen_trusted_key()));

	button = new QPushButton("Sign message");
	layout->addWidget(button, 1, 0);
	connect(button, SIGNAL(clicked()), this, SLOT(sign()));

	button = new QPushButton("Delete key pair");
	layout->addWidget(button, 0, 2);
	connect(button, SIGNAL(clicked()), this, SLOT(del_key()));

	button = new QPushButton("Change pin");
	layout->addWidget(button, 2, 0);
	connect(button, SIGNAL(clicked()), this, SLOT(set_pin()));

	button = new QPushButton("Generate random bytes");
	layout->addWidget(button, 3, 0);
	connect(button, SIGNAL(clicked()), this, SLOT(get_random()));

	button = new QPushButton("Show logs");
	layout->addWidget(button, 4, 0);
	connect(button, SIGNAL(clicked()), this, SLOT(get_logs()));

	button = new QPushButton("Show API version");
	layout->addWidget(button, 4, 1);
	connect(button, SIGNAL(clicked()), this, SLOT(get_version()));
}

std::string commandDialog::get_kname()
{
	return QInputDialog::getText(this, "Key pair name", "Insert the key's identification string:").toStdString();
}

void commandDialog::command_exec()
{
	socket->connectToHost(QHostAddress::LocalHost, serverPort);
}

void commandDialog::failure(QString response)
{
	QMessageBox(QMessageBox::Critical, "Failed", "Command failed to execute.").exec();
	if (!response.isEmpty())
		QMessageBox(QMessageBox::Information, "Error log", response).exec();
	data->clear();
}

void commandDialog::success(QString response)
{
	QMessageBox(QMessageBox::Information, "Succeeded", "Command execution succeeded").exec();
	if (!response.isEmpty())
		QMessageBox(QMessageBox::Information, "Result", response).exec();
	data->clear();
}

