#include <random>
#include <cstring>

#include "smartcard.hpp"
#include "card_exceptions.hpp"
#include "../ed25519/ed25519.h"
#include "../base64/base64.h"

using namespace std;

card::card()
{
	//Set default pin
	set_pin("0000");

	//Create a map to store the public-private key pairs
	keys = new unordered_map<string, keypair>;

	//Generate the main key pair
	gen_key("main");
}

card::card(bool NO_MAIN_KEY)
{
	//Set default pin
	set_pin("0000");

	//Create a map to store the public-private key pairs
	keys = new unordered_map<string, keypair>;
}

string card::gen_key(string kname)
{
	unsigned char *seed;
	short i;

	if (keys->find(kname) != keys->end())
		throw KeyExists();

	//Generate a random 32byte seed
	seed = get_random(32);

	//Generate the key pair
	ed25519_create_keypair((*keys)[kname].public_key, (*keys)[kname].private_key, seed);

	//Clear the used seed
	for (i=0;i<32;i++)
		seed[i] = 0;
	delete seed;

	//Return the public key
	return base64_encode((*keys)[kname].public_key, 32);
}

string card::gen_trusted_key(string kname)
{
	string pub;

	//Generate the key pair
	gen_key(kname);

	//Return the signed public key
	return sign((*keys)[kname].public_key, 32, "main");
}

string card::sign(const unsigned char *msg, unsigned int length, string kname)
{
	unsigned char signature[64];

	//Verify that the key pair exists
	if (keys->find(kname) == keys->end())
		throw KeyNotExists();

	//Generate the signature of the message
	ed25519_sign(signature, msg, length, (*keys)[kname].public_key, (*keys)[kname].private_key);

	//Return the final message
	return "-----BEGIN SIGNED MESSAGE-----" + base64_encode(msg, length) + "-----BEGIN SIGNATURE-----" + base64_encode(signature, 64) + "-----END SIGNATURE-----";
}

int card::del_key(string kname)
{
	if (keys->find(kname) == keys->end())
		throw KeyNotExists();
	if (kname != "main")
		return ! keys->erase(kname);
	return 1;
}

void card::set_pin(string new_pin)
{
	pin = new_pin;
}

unsigned char* card::get_random(unsigned int n)
{
	unsigned int i;
	random_device rng;
	unsigned char *true_random;

	true_random = new unsigned char [n];

	//Generate n random bytes
	for (i=0;i<n;i++) {
		while (rng.entropy() < 8) {};
		true_random[i] = rng();
	}

	return true_random;
}

string card::get_identity()
{
	return base64_encode((*keys)["main"].public_key, 32);
}

bool card::validate_pin(string pin)
{
	return this->pin == pin;
}

card::~card()
{
	short i;
	keypair zero;
	unordered_map<string, keypair>::iterator it;

	for (i=0;i<64;i++)
		zero.private_key[i] = 0;
	for (i=0;i<32;i++)
		zero.public_key[i] = 0;

	//Clear sensitive data from the memory
	for (it=keys->begin(); it!=keys->end(); it++)
		it->second = zero;
}

istream &operator>>(istream &in, card &c)
{
	unsigned int entries, i;
	string tag, pub, priv;

	in >> c.pin;
	in >> entries;
	for (i=0; i<entries; i++) {
		in >> tag >> priv >> pub;
		memcpy((*c.keys)[tag].private_key, base64_decode(priv).c_str(), 64);
		memcpy((*c.keys)[tag].public_key, base64_decode(pub).c_str(), 32);
	}
	return in;
}

ostream &operator<<(ostream &out, const card &c)
{
	unordered_map<string, keypair>::iterator i;

	out << c.pin << endl;
	out << c.keys->size() << endl;
	for (i=c.keys->begin(); i!=c.keys->end(); i++)
		out << i->first << " " << base64_encode(i->second.private_key, 64) << " " << base64_encode(i->second.public_key, 32) << endl;
	return out;
}

