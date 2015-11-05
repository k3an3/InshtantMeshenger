#include <iostream>
#include <fstream>
#include <string>

#include <crypto.h>
#include <cryptopp/osrng.h>

using namespace std;
using namespace libmeshenger;
using namespace CryptoPP;

int
main(int argc, char ** argv)
{
	if (argc != 2) {
		cout << "Usage: keygen <outputname>" << endl;
		return -1;
	}
	AutoSeededRandomPool rng;
	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 3072);
	RSA::PublicKey pubkey(params);
	RSA::PrivateKey privkey(params);

	Integer m = params.GetModulus();
	Integer p = params.GetModulus();
	Integer q = params.GetModulus();
	Integer priv = params.GetPrivateExponent();
	Integer pub = params.GetPublicExponent();

	string privname = string(argv[1]).append(".priv");
	string pubname = string(argv[1]).append(".pub");

	CryptoEngine::pubkeyToFile(pubkey, pubname);
	CryptoEngine::privkeyToFile(privkey, privname);

	cout << "Loading and verifying..." << endl;

	RSA::PrivateKey newpriv = CryptoEngine::privkeyFromFile(privname);
	RSA::PublicKey newpub = CryptoEngine::pubkeyFromFile(pubname);

	cout << (m == newpriv.GetModulus() ? "TRUE" : "FALSE") << endl;
	cout << (priv == newpriv.GetPrivateExponent() ? "TRUE" : "FALSE") << endl;
	cout << (pub == newpriv.GetPublicExponent() ? "TRUE" : "FALSE") << endl;

	cout << (m == newpub.GetModulus() ? "TRUE" : "FALSE") << endl;
	cout << (pub == newpub.GetPublicExponent() ? "TRUE" : "FALSE") << endl;

	return 0;
}
