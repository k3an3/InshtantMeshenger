/* crypto.cpp */

#include <crypto.h>
#include <vector>
#include <cstdint>
#include <parser.h>
#include <exception>

#include <fstream>

#include <cryptopp/osrng.h>

using namespace std;
using namespace CryptoPP;

namespace libmeshenger
{
	/* Constructors */
	CryptoEngine::CryptoEngine(RSA::PrivateKey privkey)
		: m_privkey(privkey), privkey_initialized(true)
	{
	}

	CryptoEngine::CryptoEngine()
		: privkey_initialized(false)
	{
	}

	void
	CryptoEngine::setPrivateKey(InvertibleRSAFunction privkey)
	{
		m_privkey = privkey;
		privkey_initialized = true;
	}

	bool
	CryptoEngine::tryDecrypt(EncryptedMessage& em)
	{
		/* Ensure state is valid */
		if (em.decrypted())
			throw PacketStateException("Message is already decrypted!");

		if (!em.encrypted())
			throw PacketStateException("Message is not encrypted!");

		if (!privkey_initialized)
			throw runtime_error("Private key sot set");

		vector<uint8_t> plaintext;
		vector<uint8_t> ciphertext = em.encryptedBody();
		DecodingResult res;
		AutoSeededRandomPool rng;

		/* Create decryptor */
		RSAES_OAEP_SHA_Decryptor d(m_privkey);

		/* Resize to allow for max possible size */
		plaintext.resize(d.MaxPlaintextLength(em.m_body_enc.size()));

		res = d.Decrypt(rng, ciphertext.data(), ciphertext.size(), plaintext.data());

		/* Return false if it didn't decrypt. This does not validate correct
		 * key! */
		if (!res.isValidCoding)
			return false;

		/* Resize down to actual size */
		plaintext.resize(res.messageLength);

		em.m_body_dec = plaintext;
		em.m_decrypted = true;
	}

	void
	CryptoEngine::encryptMessage(EncryptedMessage &em, RSA::PublicKey pubkey)
	{
		/* Check state */
		if (em.encrypted())
			throw PacketStateException("Message already encrypted!");

		if (!em.decrypted())
			throw PacketStateException("Message not decrypted!");

		AutoSeededRandomPool rng;
		vector<uint8_t> ciphertext;
		vector<uint8_t> plaintext;

		/* Create decryptor */
		RSAES_OAEP_SHA_Encryptor e(pubkey);

		/* Set size and plaintext */
		plaintext = em.decryptedBody();
		ciphertext.resize(e.CiphertextLength(plaintext.size()));

		e.Encrypt(rng, plaintext.data(), plaintext.size(), ciphertext.data());

		em.m_body_enc = ciphertext;
		em.m_encrypted = true;
	}

	/* Other ways of getting public key */
	void
	CryptoEngine::encryptMessage(EncryptedMessage &em, const Buddy &b)
	{
		encryptMessage(em, b.pubkey());
	}

	void
	CryptoEngine::encryptMessage(EncryptedMessage &em, const string &name)
	{
		// TODO: Change the exceptions to a new class
		if (name == "")
			throw runtime_error("Name cannot be empty!");

		for(uint16_t i = 0; i < m_buddies.size(); i++){
			if (m_buddies[i].name() == name)
				encryptMessage(em, m_buddies[i].pubkey());
		}

		throw runtime_error("Buddy with name not found!");
	}

	void
	CryptoEngine::encryptMessage(EncryptedMessage &em, uint16_t i)
	{
		if (i >= m_buddies.size())
			throw runtime_error("Buddy index out of bounds!");

		encryptMessage(em, m_buddies[i]);
	}

	RSA::PublicKey
	Buddy::pubkey() const
	{
		return m_pubkey;
	}

	/* Static key serialization functions */
	void
	CryptoEngine::pubkeyToFile(RSA::PublicKey key, string filename)
	{
		/* Simple serialization format:
		 * Modulus
		 * Public Exp.
		 *
		 * Currently, the default primes are used. That's probably not good
		 * 				Johnny Treehorn presents
		 * 				****** LOGJAMMIN' *******
		 */
		ofstream f(filename);
		f << key.GetModulus() << endl;
		f << key.GetPublicExponent() << endl;
		f.close();
	}

	string
	Buddy::name() const
	{
		return m_name;
	}

	Buddy::Buddy(RSA::PublicKey pubkey, string n)
		: m_pubkey(pubkey), m_name(n)
	{
	}

	Buddy::Buddy(RSA::PublicKey pubkey)
		: m_pubkey(pubkey), m_name("")
	{
	}

}
