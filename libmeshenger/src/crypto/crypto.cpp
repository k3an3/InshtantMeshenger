/* crypto.cpp */

#include <crypto.h>
#include <vector>
#include <cstdint>
#include <parser.h>
#include <exception>

#include <fstream>

#include <cryptopp/osrng.h>
#include <cryptopp/files.h>

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

		/* Check magic flag. Not needed once signing is implemented */
		uint8_t * magic = (uint8_t *) "DECRYPTION GOOD!";
		for (int i = 0; i < 16; i++) {
			if (plaintext[i] != magic[i])
				return false;
		}

		em.m_body_dec = vector<uint8_t>(plaintext.begin() + 16, plaintext.end());
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

		/* Known plaintext. Might be bad, who knows? 
		 * This flag is checked to see if the decrypt was successful
		 *
		 * Will not be necessary once signatures are implemented */
		uint8_t * magic_flag = (uint8_t *) "DECRYPTION GOOD!";

		/* Appen decrypted body to magic */
		plaintext = vector<uint8_t>(magic_flag, magic_flag + 16);
		plaintext.insert(plaintext.end(), em.m_body_dec.begin(), em.m_body_dec.end());

		/* Resize ciphertext vector */
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
		ByteQueue queue;
		FileSink f(filename.c_str());

		key.Save(queue);
		queue.CopyTo(f);
		f.MessageEnd();
	}

	void
	CryptoEngine::privkeyToFile(RSA::PrivateKey key, string filename)
	{
		ByteQueue queue;
		FileSink f(filename.c_str());

		key.Save(queue);
		queue.CopyTo(f);
		f.MessageEnd();
	}

	RSA::PublicKey 
	CryptoEngine::pubkeyFromFile(string filename)
	{
		RSA::PublicKey key;
		FileSource f(filename.c_str(), true);
		ByteQueue queue;

		f.TransferTo(queue);
		queue.MessageEnd();
		key.Load(queue);

		return key;
	}

	RSA::PrivateKey 
	CryptoEngine::privkeyFromFile(string filename)
	{
		RSA::PrivateKey key;
		FileSource f(filename.c_str(), true);
		ByteQueue queue;

		f.TransferTo(queue);
		queue.MessageEnd();
		key.Load(queue);

		return key;
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
