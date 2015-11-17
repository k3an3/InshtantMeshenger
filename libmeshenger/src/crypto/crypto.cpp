/* crypto.cpp */

#include <crypto.h>
#include <vector>
#include <cstdint>
#include <parser.h>
#include <exception>

#include <fstream>

#include <cryptopp/osrng.h>
#include <cryptopp/files.h>
#include <cryptopp/base64.h>

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

	/* Private key setting functions. Must be used before 
	 * pretty much any functionality is available, or
	 * exceptions will happen */
	void
	CryptoEngine::setPrivateKey(InvertibleRSAFunction privkey)
	{
		m_privkey = privkey;
		privkey_initialized = true;
	}

	void
	CryptoEngine::setPrivateKey(string s)
	{
		m_privkey = privkeyFromBase64(s);
		privkey_initialized = true;
	}

	void
	CryptoEngine::setPrivateKeyFromFile(string filename)
	{
		m_privkey = privkeyFromFile(filename);
		privkey_initialized = true;
	}

	/* Attempt to decrypt the message */
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

	RSA::PrivateKey
	CryptoEngine::getPrivkey()
	{
		return m_privkey;
	}

	RSA::PublicKey
	CryptoEngine::getPubkey()
	{
		RSA::PublicKey key;
		key.AssignFrom(m_privkey);
		return key;
	}

	RSA::PublicKey
	Buddy::pubkey() const
	{
		return m_pubkey;
	}

	/* Default key generator */
	RSA::PrivateKey
	genkey()
	{
		AutoSeededRandomPool rng;
		InvertibleRSAFunction params;
		params.GenerateRandomWithKeySize(rng, 3072);
		RSA::PublicKey pubkey(params);
		RSA::PrivateKey privkey(params);

		return privkey;
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

	RSA::PublicKey
	CryptoEngine::pubkeyFromBase64(string encoded)
	{
		RSA::PublicKey key;
		ByteQueue queue;
		string decoded;

		/* Use a pipeline to Base64 decode */
		StringSource ss(encoded, true,
		    new Base64Decoder(
		        new StringSink(decoded)
		    )
		);
	
		StringSource dss(decoded, true);
		dss.TransferTo(queue);
		queue.MessageEnd();
		key.Load(queue);

		return key;
	}

	RSA::PrivateKey
	CryptoEngine::privkeyFromBase64(string encoded)
	{
		RSA::PrivateKey key;
		ByteQueue queue;
		string decoded;

		/* Use a pipeline to Base64 decode */
		StringSource ss(encoded, true,
		    new Base64Decoder(
		        new StringSink(decoded)
		    )
		);
	
		StringSource dss(decoded, true);
		dss.TransferTo(queue);
		queue.MessageEnd();
		key.Load(queue);

		return key;
	}

	string
	CryptoEngine::pubkeyToBase64(RSA::PublicKey key)
	{
		ByteQueue queue;
		string decoded;
		StringSink dss(decoded);
		string encoded;

		key.Save(queue);
		queue.CopyTo(dss);
		dss.MessageEnd();

		/* Use a CryptoPP pipeline to base64 encode */
		StringSource ss(decoded, true,
		    new Base64Encoder(
		        new StringSink(encoded)
		    )
		);

		return encoded;
	}

	string
	CryptoEngine::privkeyToBase64(RSA::PrivateKey key)
	{
		ByteQueue queue;
		string decoded;
		StringSink dss(decoded);
		string encoded;

		key.Save(queue);
		queue.CopyTo(dss);
		dss.MessageEnd();

		/* Use a CryptoPP pipeline to base64 encode */
		StringSource ss(decoded, true,
		    new Base64Encoder(
		        new StringSink(encoded)
		    )
		);

		return encoded;
	}

	/* Fingerprint (static) */
	string
	CryptoEngine::fingerprint(RSA::PublicKey key)
	{
		uint8_t hash[32];
		string fingerprint;
		char buffer[3];
		string encoded = pubkeyToBase64(key);
		CryptoPP::SHA256().CalculateDigest(hash, 
				(unsigned char *) encoded.c_str(), encoded.length());
		
		for(int i = 0; i < 16; i++) {
			snprintf(buffer, 3, "%02X", (hash[i] ^ hash[16+i]));
			fingerprint += buffer;
			if (i != 15)
				fingerprint += ':';
		}
		return fingerprint;
	}

	/* Buddy functions and constructors (pretty basic) */
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

	/* base64 pubkey, name */
	Buddy::Buddy(string key, string name)
		: m_pubkey(CryptoEngine::pubkeyFromBase64(key)), m_name(name)
	{
	}

	/* base64 pubkey */
	Buddy::Buddy(string key)
		: m_pubkey(CryptoEngine::pubkeyFromBase64(key)), m_name("")
	{
	}
}
