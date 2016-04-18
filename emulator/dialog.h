#ifndef DIALOG_H
#define DIALOG_H

#include <string>

#include <QWidget>
#include <QGridLayout>
#include <QTcpSocket>

class commandDialog : public QWidget {
	Q_OBJECT
public:
	commandDialog();

private slots:
	void gen_key();
	void gen_trusted_key();
	void sign();
	void del_key();
	void set_pin();
	void get_random();
	void get_logs();
	void get_version();
	void connected();			//Delivers the request when the connection has been established
	void read_response();			//Parses the response

private:
	void createButtons();
	std::string get_kname();		//Requests from the user to enter the identifier of the key pair
	void command_exec();			//Tries to establish a connection with the server
	void failure(QString response);		//Shows the error message
	void success(QString response);		//Shows the response
	QGridLayout *layout;
	qint16 serverPort;
	QByteArray *data;
	QTcpSocket *socket;
};

#endif

