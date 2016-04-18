#include "clie.hpp"
using namespace std;
istream &operator >> (istream &input, client &clie) {
	unsigned int orders, i;

	input >> orders;
	
	for (i = 0; i < orders; i++) {
		struct order curr_ord;

		unsigned int j, items;
		
		input >> items;
		input >> curr_ord.ordid;

		for (j = 0; j < items; j++) {
			string prod;
			float price;
			unsigned int quantity;

			input >> prod >> price >> quantity;
			curr_ord.produkte.push_back(prod);
			curr_ord.prix.push_back(price);
			curr_ord.posotita.push_back(quantity);
		}

		clie.orliszt.push_back(curr_ord);
	}
	return input;
}
ostream &operator << (ostream &ecrire, client &clie)
{
	vector<struct order>::iterator it1;
	ecrire << clie.orliszt.size() << endl;
	for (it1 = clie.orliszt.begin(); it1 != clie.orliszt.end(); it1++)
	{
		ecrire << it1->produkte.size() << endl;
		ecrire << it1->ordid << endl;
		vector<string>::iterator it2 = it1 ->produkte.begin();
		vector<float>::iterator it3 = it1 ->prix.begin();
		vector<unsigned short int>::iterator it4 = it1->posotita.begin();
		while (it2!=it1 -> produkte.end() && it3!=it1 -> prix.end() && it4 != it1 -> posotita.end())
		{
			ecrire << *it2 << " " << *it3 << " " << *it4 << endl;
			it2++; it3++; it4++;
		}
	}
	return ecrire;
}

void client::addorder(struct order &foo)
{
	orliszt.push_back(foo);
}
struct order client::getorder(long unsigned int &id)
{
	return *trouver(id);
}
float client::compute_price(long unsigned int &id)
{
	vector<struct order>::iterator it1;
	it1 = trouver(id);
	vector<float>::iterator it2 = it1->prix.begin();
	vector<unsigned short int>::iterator it3 = it1->posotita.begin();
	float sum = 0;
	while (it2 != it1->prix.end() && it3 != it1->posotita.end())
	{
		sum += (*it2)*(*it3); 
		it2++; it3++;
	}
	return sum;
}
vector<struct order>::iterator client::trouver(long unsigned int &id)
{
	vector<struct order>::iterator it;
	for (it = orliszt.begin(); it != orliszt.end(); it++)
	{
		if (it->ordid == id)
		{
			return it;
		}
	}
	return orliszt.end();
}
void client::delorder(long unsigned int &id)
{
	orliszt.erase(trouver(id));
}

unsigned int client::NoOfOrders() {
	return orliszt.size();
}
