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

	return 0;
}
