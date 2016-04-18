#ifndef CLIE_HPP
#define CLIE_HPP

#include <istream>
#include <ostream>
#include <vector>
#include <string>

struct order	{
	std::vector<std::string> produkte;
	std::vector<float> prix;
	std::vector<unsigned short int> posotita;
	long unsigned int ordid;
};

class client	{
private:
	std::vector<struct order>::iterator trouver(long unsigned int &id);
	std::vector<struct order> orliszt;
public:
	void addorder(struct order &foo);
	void delorder(long unsigned int &id);
	struct order getorder(long unsigned int &id);
	float compute_price(long unsigned int &id);
	unsigned int NoOfOrders();
	friend std::istream &operator >> (std::istream &input, client &clie);
	friend std::ostream &operator << (std::ostream &ecrire, client &clie);
};

#endif
