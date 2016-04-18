#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

#include "smartcard.hpp"

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow();
	void save(QString filename, card *sc);			//Saves the virtual smart card to a file

private slots:
	void newCard();						//Creates a new virtual smart card
	void open();						//Reads the data of an existing virtual smart card from a file
	void about();
	void commands();					//Opens dialog with the available commands

private:
	void createActions();
	void createMenus();
	void createDockWindow(QString filename, card *sc);
	QMenu *fileMenu;
	QMenu *debugMenu;
	QMenu *helpMenu;
	QAction *newCardAct;
	QAction *openAct;
	QAction *runCommand;
	QAction *aboutAct;
	QAction *aboutQtAct;
};

#endif

