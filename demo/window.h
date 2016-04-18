#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include "clie.hpp"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
protected:
	void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
private slots:
	void newClient();
	void openClient();
	void saveClient();
	void saveasClient();
	void selectClient();
	void selectShop();
	void selectBank();
	void buy();
	void about();
	void addToBasket();
private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createProductList();
	void createButtons();
	bool confirm(qint16 &port, std::string keypair, QByteArray &databuffer);
	QMenu *fileMenu;
	QMenu *cardsMenu;
	QMenu *helpMenu;
	QAction *newClientAct; //je ecris act pour justifier
	QAction *openClientAct;
	QAction *saveClientAct;
	QAction *saveasClientAct;
	QAction *aboutAct;
	QAction *aboutQtAct;
	QAction *selectClientAct;
	QAction *selectShopAct;
	QAction *selectBankAct;
	QToolBar *fileToolBar;
	QListWidget *basket;
	QListWidget *product_list;
	QPushButton *buyButton;
	QPushButton *cartButton;
	QString filename; 
	qint16 client_card;
	qint16	shop_card;
	qint16	bank_card;
	unsigned long int order_id; 
	client cli;
	void realsaveClient();
	bool unsaved;	
	bool newclient;
	std::string bank_key;
};
#endif 
