#ifndef SMARTCARD_HPP
#define SMARTCARD_HPP

#include <string>
#include <unordered_map>
#include <istream>
#include <ostream>

struct keypair {
	unsigned char private_key[64], public_key[32];
};

class card {
private:
	std::string pin;
	std::unordered_map<std::string, keypair> *keys;
public:
	card();											//Creates the keystore and the "main" key pair;
	card(bool NO_MAIN_KEY);									//Creates only the keystore ***Required for the emulator***
	std::string gen_key(std::string kname);							//Generates a new key pair, saves it as kname to the key store and returns the base64 encoded public part
	std::string gen_trusted_key(std::string kname);						//Same as the gen_key method but the returned public key is signed by the "main" key pair
	std::string sign(const unsigned char *msg, unsigned int length, std::string kname);	//Signes the message using the kname key pair
	int del_key(std::string kname);								//Deletes the specified key pair
	void set_pin(std::string new_pin);							//Sets the cards pin;
	unsigned char *get_random(unsigned int n);						//Returns a byte array with n random bytes
	std::string get_identity();								//Returns the base64 encoded public part of the "main" key pair ***Required for the emulator***
	bool validate_pin(std::string pin);							//Checks if the pin is correct ***Required for the emulator***
	~card();										//Purges the keystore safely
	friend std::istream &operator>>(std::istream &in, card &c);				//Imports card data ***Required for the emulator***
	friend std::ostream &operator<<(std::ostream &out, const card &c);			//Exports card data ***Required for the emulator***
};

#endif

