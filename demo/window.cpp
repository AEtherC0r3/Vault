#include <fstream>
#include <cstdlib>
#include <ctime>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QAction>
#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QTcpSocket>
#include <QHostAddress>
#include "window.h"
#include "../ed25519/ed25519.h"
#include "../base64/base64.h"

using namespace std;

#define BANK_KEY "LvRprXVc6hjQuPCPLmi0LBXw7Q8Brff9Vw+3YgH0gNA="

MainWindow::MainWindow()
{
	basket = new QListWidget;
	setCentralWidget(basket);
	
	createActions();
	createMenus();
	createToolBars();
	createProductList();
	createButtons();

	unsaved = false;
	srand(time(NULL));
	bank_key = base64_decode(BANK_KEY);
}

void MainWindow::closeEvent(QCloseEvent *event)
{	
	if (unsaved)	{
		int ok = QMessageBox(QMessageBox::Question, "Unsaved changes?",  "Would you like to save unsaved changes?", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel).exec();
		switch (ok)	{
			case QMessageBox::Save:
				saveClient();
			case QMessageBox::Discard:
				event->accept();
				break;
			case QMessageBox::Cancel:
				event->ignore();
		}
	}
	else
		event->accept();
}
void MainWindow::newClient()	{
	if(unsaved)	{
		int ok = QMessageBox(QMessageBox::Question, "Unsaved changes?",  "Would you like to save unsaved changes?", QMessageBox::Save | QMessageBox::Discard).exec();
		if(ok == QMessageBox::Save)
			saveClient();
	}	
	
	client lala;
	struct order empty_order;
	empty_order.ordid = rand();
	order_id = empty_order.ordid;
	cli = lala;
	cli.addorder(empty_order);
	
	newclient = true;
	unsaved = false;
	
	cartButton->setEnabled(true);
}
void MainWindow::openClient()
{
	QFileDialog dialog(this);
	
	dialog.setDefaultSuffix("clie");

	if (dialog.exec()) {
		QByteArray file;
		ifstream fp;
		client lala;

		filename = dialog.selectedFiles().at(0);
		file = filename.toLatin1();

		fp.open(file.data());
		if (!fp.is_open()) {
			QMessageBox(QMessageBox::Critical, "Read failed", "Failed to open " + filename + " for reading.").exec();
			return;
		}

		fp >> lala;
		if (fp.fail()) {
			QMessageBox(QMessageBox::Critical, "Read failed", "Failed to read the client's data").exec();
			fp.close();
			return;
		}

		fp.close();
		
		newclient = false;
		
		struct order empty_order;
		empty_order.ordid = rand();
		order_id = empty_order.ordid;
		cli = lala;
		cli.addorder(empty_order);
		
		unsaved = false;
		
		cartButton->setEnabled(true);
	}
}

void MainWindow::saveClient()
{
	if(newclient)
		saveasClient();
	else
		realsaveClient();	
}

void MainWindow::saveasClient()
{
	QFileDialog dialog(this);
	
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setDefaultSuffix("clie");
	
	if(dialog.exec())	{
		filename = dialog.selectedFiles().at(0);
		realsaveClient();
		newclient = false;
	}
}

void MainWindow::selectClient()
{
	client_card = QInputDialog::getInt(this, "Server Port", "Insert client's card port:", 1024, 1, 65535, 1);
}

void MainWindow::selectShop()
{
	shop_card = QInputDialog::getInt(this, "Server Port", "Insert shop order processing port:", 1024, 1, 65535, 1);
}

void MainWindow::selectBank()
{
	bank_card = QInputDialog::getInt(this, "Server Port", "Insert bank's validator port:", 1024, 1, 65535, 1);
}

void MainWindow::buy()
{
	float total_price = cli.compute_price(order_id);
	int ok = QMessageBox(QMessageBox::Information, "Confirm order?", "Your order costs " + QString::number(total_price) + ".", QMessageBox::Ok | QMessageBox::Cancel, this).exec();
	if (ok == QMessageBox::Ok)
	{
		QByteArray purchase;
		QString transfer(QString::number(total_price) + ' ' + QString::number(time(NULL)));
		string stdtransfer = transfer.toStdString();
		stdtransfer = base64_encode(reinterpret_cast<const unsigned char *>(stdtransfer.c_str()), stdtransfer.length());
		purchase = stdtransfer.c_str();
		confirm(shop_card, "outgoing_order", purchase);
		purchase = purchase.toBase64();
		confirm(client_card, "pisteos", purchase);
		purchase = purchase.toBase64();
		confirm(bank_card, "transaction_confirm", purchase);
		transfer = purchase;
		string final_msg = base64_decode(transfer.section("-----", 2, 2).toStdString());
		string bank_signature = base64_decode(transfer.section("-----", -3, -3).toStdString());
		if (ed25519_verify(reinterpret_cast<const unsigned char *>(bank_signature.c_str()), reinterpret_cast<const unsigned char *>(final_msg.c_str()), final_msg.length(), reinterpret_cast<const unsigned char *>(bank_key.c_str())))	{
			QMessageBox(QMessageBox::Information, "Transaction Status", "Success!").exec();
        
			struct order empty_order;
			empty_order.ordid = rand();
			order_id = empty_order.ordid;
			cli.addorder(empty_order);
			basket->clear();
			buyButton->setEnabled(false);
		}
		else	{
			QMessageBox(QMessageBox::Critical, "Transaction Status", "Transaction failed!").exec();
		}
	}
}

void MainWindow::about()
{
	QMessageBox::about(this, "About App",
	"<p> Demo Shopping App </p><br> \
	<table>					\
		<tr>					\
			<th>Über 1337 Team</th>		\
		</tr>					\
		<tr>					\
			<th>ΑΕΜ</th>			\
			<th>Ονοματεπώνυμο</th>		\
		</tr>					\
		<tr>					\
			<td>8398</td>			\
			<td>Αντώνιος Ελευθεριάδης</td>	\
		</tr>					\
		<tr>					\
			<td>8428</td>			\
			<td>Φωτεινή Δερβίση</td>	\
		</tr>					\
		<tr>					\
			<td>8443</td>			\
			<td>Απόστολος Βαβούρης</td>	\
		</tr>					\
	</table>");
}

void MainWindow::addToBasket()
{
	struct order curr_ord = cli.getorder(order_id);
	QString ord = product_list->currentItem()->text();
	QString price;
	bool ok;
	
	price = ord.section(' ', -1);

	ord.remove(' ' + price);

	unsigned short int quantity = QInputDialog::getInt(this, "Quantity", "Please insert quantity for " + ord + ":", 1, 1, 65535, 1, &ok);
	
	if(ok)	{
		curr_ord.produkte.push_back(ord.toStdString());
		curr_ord.prix.push_back(price.toFloat());
		curr_ord.posotita.push_back(quantity);

		basket->addItem(QString::number(quantity) + 'x' + ord + ' ' + QString::number(quantity*price.toFloat()));
		cli.delorder(order_id);
		cli.addorder(curr_ord);

		unsaved = true;
		
		buyButton->setEnabled(true);
	}
}

void MainWindow::createActions()
{
	newClientAct = new QAction(QIcon(":/images/new.png"), "New", this);
	newClientAct->setStatusTip("Creates a new client object");
	connect(newClientAct, SIGNAL(triggered()), this, SLOT(newClient()));

	openClientAct = new QAction(QIcon(":/images/open.png"), "Open", this);
	openClientAct->setStatusTip("Open existing client object");
	connect(openClientAct, SIGNAL(triggered()), this, SLOT(openClient()));

	saveClientAct = new QAction(QIcon(":/images/save.png"), "Save", this);
	saveClientAct->setStatusTip("Save existing client object");
	connect(saveClientAct, SIGNAL(triggered()), this, SLOT(saveClient()));

	saveasClientAct = new QAction(QIcon(":/images/saveas.png"), "Save As...", this);
	connect(saveasClientAct, SIGNAL(triggered()), this, SLOT(saveasClient()));

	aboutAct = new QAction("About", this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction("About Qt", this);
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	selectClientAct = new QAction("Client", this);
	connect(selectClientAct, SIGNAL(triggered()), this, SLOT(selectClient()));
	
	selectShopAct = new QAction("Shop", this);
	connect(selectShopAct, SIGNAL(triggered()), this, SLOT(selectShop()));
	
	selectBankAct = new QAction("Bank", this);
	connect(selectBankAct, SIGNAL(triggered()), this, SLOT(selectBank()));
}
void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu("File");
	fileMenu->addAction(newClientAct);
	fileMenu->addAction(openClientAct);
	fileMenu->addAction(saveClientAct);
	fileMenu->addAction(saveasClientAct);
		
	cardsMenu = menuBar()->addMenu("Cards selection");
	cardsMenu->addAction(selectClientAct);
	cardsMenu->addAction(selectShopAct);
	cardsMenu->addAction(selectBankAct);

	helpMenu = menuBar()->addMenu("Help");
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
	fileToolBar = addToolBar("File");
	fileToolBar->addAction(newClientAct);
	fileToolBar->addAction(openClientAct);
	fileToolBar->addAction(saveClientAct);
}

void MainWindow::createProductList()
{
	QDockWidget *dock = new QDockWidget("Product List", this);
	product_list = new QListWidget(dock);
	product_list->addItems(QStringList()
		<< "foteini 4294967295"
		<< "antonis 4294967295"
		<< "apostolos 4294967295"
		<< "water 12"
		<< "fire 64"
		<< "earth 9876"
		<< "air 8"
		<< "aether 1337");

	dock->setWidget(product_list);
	dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
	addDockWidget(Qt::RightDockWidgetArea, dock);
}

void MainWindow::createButtons()
{
	buyButton = new QPushButton;
	buyButton->setText("Buy!");
	buyButton->setEnabled(false);

	QDockWidget *dock = new QDockWidget;
	dock->setWidget(buyButton);
	dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
	addDockWidget(Qt::BottomDockWidgetArea, dock);
	
	connect(buyButton, SIGNAL(clicked()), this, SLOT(buy()));

	cartButton = new QPushButton;
	cartButton->setText("Add to cart");
	cartButton->setEnabled(false);

	dock = new QDockWidget;
	dock->setWidget(cartButton);
	dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
	addDockWidget(Qt::RightDockWidgetArea, dock);
	
	connect(cartButton, SIGNAL(clicked()), this, SLOT(addToBasket()));
}

bool MainWindow::confirm(qint16 &port, string keypair, QByteArray &databuffer)
{
	QTcpSocket socket;
	socket.connectToHost(QHostAddress::LocalHost, port);
	
	if (!socket.waitForConnected())
		return true;
	
	QByteArray request;
	request.append("sign\n");
	request.append(keypair.c_str());
	request.append('\n');
	request.append(databuffer);

	socket.write(request);

	socket.waitForReadyRead(-1);
	bool status = false;
	socket.getChar((char *)&status);
	databuffer = socket.readAll();

	return status;
}

void MainWindow::realsaveClient()
{
	QByteArray file;
	ofstream fp;

	file = filename.toLatin1();

	fp.open(file.data(), ofstream::trunc);
	if (!fp.is_open()) {
		QMessageBox(QMessageBox::Critical, "Write failed", "Failed to open " + filename + " for writing.").exec();
		return;
	}

	fp << cli;
	if (fp.fail())
		QMessageBox(QMessageBox::Critical, "Unfortunately writing failed", "Failed to save the clie's data").exec();

	fp.close();

	unsaved = false;
}

