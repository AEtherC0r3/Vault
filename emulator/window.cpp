#include <fstream>

#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QApplication>
#include <QMenuBar>

#include "window.h"
#include "dialog.h"
#include "dock.h"

MainWindow::MainWindow()
{
	createActions();
	createMenus();
}

void MainWindow::save(QString filename, card *sc)
{
		QByteArray file;
		std::ofstream fp;

		file = filename.toLatin1();

		fp.open(file.data(), std::ofstream::trunc);
		if (!fp.is_open()) {
			QMessageBox(QMessageBox::Critical, "Write failed", "Failed to open " + filename + " for writing.").exec();
			return;
		}

		fp << *sc;
		if (fp.fail())
			QMessageBox(QMessageBox::Critical, "Write failed", "Failed to save the card's data").exec();

		fp.close();
}

void MainWindow::newCard()
{
	QFileDialog dialog(this);
	
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setDefaultSuffix("sc");

	if (dialog.exec()) {
		card *c;
		QString file;

		//Create a new card object and write it to a file
		c = new card;
		file = dialog.selectedFiles().at(0);
		save(file, c);
		
		//Create a dock for the new smart card
		createDockWindow(file, c);
	}
}

void MainWindow::open()
{
	QFileDialog dialog(this);
	
	dialog.setDefaultSuffix("sc");

	if (dialog.exec()) {
		card *c;
		QString file1;
		QByteArray file2;
		std::ifstream fp;

		//Create a card object and read it's data from the file
		c = new card(true);
		file1 = dialog.selectedFiles().at(0);
		file2 = file1.toLatin1();

		fp.open(file2.data());
		if (!fp.is_open()) {
			QMessageBox(QMessageBox::Critical, "Read failed", "Failed to open " + file1 + " for reading.").exec();
			return;
		}

		fp >> *c;
		if (fp.fail()) {
			QMessageBox(QMessageBox::Critical, "Read failed", "Failed to read the card's data").exec();
			fp.close();
			return;
		}

		fp.close();

		//Create a dock for the smart card
		createDockWindow(file1, c);
	}
}

void MainWindow::commands()
{
	commandDialog *cmd = new commandDialog;
	cmd->setAttribute(Qt::WA_DeleteOnClose, true);
	cmd->show();
}

void MainWindow::about()
{
	QMessageBox::about(this, "About App", 
	"<p> The emulator creates and manages virtual	\
	smartcards in order to provide the smartcard 	\
	API functionality </p> <br>			\
	<table>						\
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

void MainWindow::createActions()
{
	newCardAct = new QAction("New", this);
	newCardAct->setStatusTip("Creates a new virtual smartcard");
	connect(newCardAct, SIGNAL(triggered()), this, SLOT(newCard()));

	openAct = new QAction("Open", this);
	openAct->setStatusTip("Open existing virtual smartcard");
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	runCommand = new QAction("Execute command", this);
	connect(runCommand, SIGNAL(triggered()), this, SLOT(commands()));

	aboutAct = new QAction("About", this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction("About Qt", this);
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu("File");
	fileMenu->addAction(newCardAct);
	fileMenu->addAction(openAct);

	debugMenu = menuBar()->addMenu("Debug");
	debugMenu->addAction(runCommand);

	helpMenu = menuBar()->addMenu("Help");
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
}

void MainWindow::createDockWindow(QString filename, card *sc)
{
	cardDock *dock;

	dock = new cardDock(filename, this, sc);
	dock->setAttribute(Qt::WA_DeleteOnClose, true);
	addDockWidget(Qt::TopDockWidgetArea, dock);
}

