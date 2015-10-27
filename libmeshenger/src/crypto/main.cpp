/* Tests for crypto */

#include <crypto.h>
#include <cryptopp/osrng.h>
#include <iostream>
#include <string>

using namespace std;
using namespace libmeshenger;
using namespace CryptoPP;

int main()
{
	AutoSeededRandomPool rng;

	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 3072);

	RSA::PublicKey pubkey(params);
	RSA::PrivateKey privkey(params);

	string original = "This is a test message that will be encrypted";

	CryptoEngine engine(privkey);

	EncryptedMessage em_orig(original);

	cout << "Original: " << original << endl;
	cout << "EM decrypted body: " << string((const char *) em_orig.decryptedBody().data()) << endl;

	cout << "EM is " << (em_orig.decrypted() ? "decrypted" : "NOT decrypted") << endl;
	cout << "EM is " << (em_orig.encrypted() ? "encrypted" : "NOT encrypted") << endl;

	engine.encryptMessage(em_orig, pubkey);
	cout << "Encrypted message!" << endl;

	cout << "EM is " << (em_orig.decrypted() ? "decrypted" : "NOT decrypted") << endl;
	cout << "EM is " << (em_orig.encrypted() ? "encrypted" : "NOT encrypted") << endl;
	cout << "EM ciphertext length: " << em_orig.encryptedBody().size() << endl;

	Packet p(em_orig);

	cout << "Packet ID string: " << p.idString() << endl;
	cout << "Packet body length: " << p.body().size() << endl;

	EncryptedMessage em_new(p);

	cout << "Parsed packet into new EM" << endl;

	cout << "EM is " << (em_new.decrypted() ? "decrypted" : "NOT decrypted") << endl;
	cout << "EM is " << (em_new.encrypted() ? "encrypted" : "NOT encrypted") << endl;
	cout << "EM ciphertext length: " << em_new.encryptedBody().size() << endl;

	cout << "Decrypting message" << endl;
	cout << (engine.tryDecrypt(em_new) ? "SUCCESS" : "FAIL") << endl;

	cout << "EM body: " << (const char *) em_new.decryptedBody().data() << endl;

	return 0;
}
