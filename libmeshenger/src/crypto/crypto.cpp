/* crypto.cpp */

#include <crypto.h>
#include <vector>
#include <cstdint>
#include <parser.h>

#include <cryptopp/osrng.h>

using namespace std;
using namespace CryptoPP;

namespace libmeshenger
{
	CryptoEngine::CryptoEngine(RSA::PrivateKey privkey)
		: m_privkey(privkey)
	{
	}

	bool
	CryptoEngine::tryDecrypt(EncryptedMessage& em)
	{
		if (em.decrypted())
			throw PacketStateException("Message is already decrypted!");

		if (!em.encrypted())
			throw PacketStateException("Message is not encrypted!");

		vector<uint8_t> plaintext;
		vector<uint8_t> ciphertext = em.encryptedBody();
		DecodingResult res;
		AutoSeededRandomPool rng;

		/* Create decryptor */
		RSAES_OAEP_SHA_Decryptor d(m_privkey);

		/* Resize to allow for max possible size */
		plaintext.resize(d.MaxPlaintextLength(em.m_body_enc.size()));

		res = d.Decrypt(rng, ciphertext.data(), ciphertext.size(), plaintext.data());

		if (!res.isValidCoding)
			return false;

		/* Resize down to actual size */
		plaintext.resize(res.messageLength);

		em.m_body_dec = plaintext;

	}

	void
	CryptoEngine::encryptMessage(EncryptedMessage &em, RSA::PublicKey pubkey)
	{
		if (!em.encrypted())
			throw PacketStateException("Message already encrypted!");

		if (em.decrypted())
			throw PacketStateException("Message not decrypted!");

		AutoSeededRandomPool rng;
		vector<uint8_t> ciphertext;
		vector<uint8_t> plaintext;

		RSAES_OAEP_SHA_Encryptor e(pubkey);

		plaintext = em.decryptedBody();
		ciphertext.resize(e.CiphertextLength(plaintext.size()));

		e.Encrypt(rng, plaintext.data(), plaintext.size(), ciphertext.data());

		em.m_body_enc = ciphertext;
	}
}
