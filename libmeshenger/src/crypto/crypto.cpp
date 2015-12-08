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
#include <cryptopp/pssr.h>
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>

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

    std::vector<Buddy>
    CryptoEngine::buddies()
    {
        return std::vector<Buddy>(m_buddies.begin(), m_buddies.end());
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

		AutoSeededRandomPool rng;

		/* Step 1: Dissect message */
		uint16_t klength = em.m_body_enc[0] * 256 + em.m_body_enc[1];
		vector<uint8_t> key_cipher(em.m_body_enc.begin() + 2, em.m_body_enc.begin() + 2 + klength);
		vector<uint8_t> iv(em.m_body_enc.begin() + 2 + klength, em.m_body_enc.begin() + 18 + klength);
		vector<uint8_t> ciphertext(em.m_body_enc.begin() + 18 + klength, em.m_body_enc.end());

		/* Step 2: Obtain AES key. Return immediately if that doesn't work */
		DecodingResult res;
		SecByteBlock key(AES::DEFAULT_KEYLENGTH);
		RSAES_OAEP_SHA_Decryptor rsa(m_privkey);
        try {
            res = rsa.Decrypt(rng, key_cipher.data(), key_cipher.size(), key.data());
        } catch (CryptoPP::Exception) {
            return false;
        }

		/* Resize down to actual size */
		key.resize(16);

		string plain;
		/* Step 3: Use AES key to decrypt ciphertext */
		try {
			CBC_Mode<AES>::Decryption aes;
			aes.SetKeyWithIV(key, key.size(), iv.data());
			string cipher((char *) ciphertext.data(), ciphertext.size());
			StringSource decryptPipeline(cipher, true,
				new StreamTransformationFilter(aes,
					new StringSink(plain)
				)
			);
		} catch (const CryptoPP::Exception& e) {
			return false;
		}

		vector<uint8_t> plaintext(plain.c_str(), plain.c_str() + plain.length());

		/* Check magic flag. Not needed once signing is implemented */
		uint8_t * magic = (uint8_t *) "DECRYPTION GOOD!";
		for (int i = 0; i < 16; i++) {
			if (plaintext[i] != magic[i])
				return false;
		}

		uint16_t mlength = (plaintext[16] * 256) + plaintext[17];
		uint16_t slength = (plaintext[18] * 256) + plaintext[19];

		/* Possible check: Verify that mlength + slength + 20 ==
		 * plaintext.length()
		 */

		em.m_body_dec = vector<uint8_t>(plaintext.begin() + 20, plaintext.begin() + 20 + mlength);
		vector<uint8_t> signature(plaintext.begin() + 20 + mlength, plaintext.end());

		/* Verify signature */
		em.m_trusted = false;
		for (int i = 0; i < m_buddies.size(); i++) {
			RSASS<PSS, SHA1>::Verifier verifier(m_buddies[i].pubkey());
			if (verifier.VerifyMessage(em.m_body_dec.data(), mlength, signature.data(), slength)) {
				em.m_sender = i;
				em.m_trusted = true;
			}
		}


		em.m_decrypted = true;
		return true;
	}

	void
	CryptoEngine::addBuddy(const Buddy& b)
	{
		cout << "Adding buddy " << b.name() << endl;
		m_buddies.push_back(b);
	}

	Buddy
	CryptoEngine::buddy(uint16_t i)
	{
		return m_buddies[i];
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
		vector<uint8_t> plaintext;

		/* Generate one-time use AES key */
		SecByteBlock key(AES::DEFAULT_KEYLENGTH);
		vector<uint8_t> iv;
		iv.resize(AES::BLOCKSIZE);

		rng.GenerateBlock(key, key.size());
		rng.GenerateBlock(iv.data(), iv.size());

		/* Encrypt the one-time use AES key */
		vector<uint8_t> key_cipher;
		RSAES_OAEP_SHA_Encryptor rsa(pubkey);

		key_cipher.resize(rsa.CiphertextLength(plaintext.size()));
		rsa.Encrypt(rng, key.data(), key.size(), key_cipher.data());
		/* key_cipher now exists */

		/* Generate signature */
		RSASS<PSS, SHA1>::Signer signer(m_privkey);
	 	size_t sig_size = signer.MaxSignatureLength();
	 	SecByteBlock signature(sig_size);

	 	sig_size = signer.SignMessage(rng, em.m_body_dec.data(), em.m_body_dec.size(), signature);
		signature.resize(sig_size);

		/* Build plaintext */
		uint8_t * magic_flag = (uint8_t *) "DECRYPTION GOOD!";
		plaintext = vector<uint8_t>(magic_flag, magic_flag + 16);

		/* Append MLength and SLength to the plaintext */
		uint8_t length[2];

		/* MLength */
		length[0] = em.m_body_dec.size() / 256;
		length[1] = em.m_body_dec.size() % 256;
		plaintext.insert(plaintext.end(), length, length + 2);

		/* SLength */
		length[0] = signature.size() / 256;
		length[1] = signature.size() % 256;
		plaintext.insert(plaintext.end(), length, length + 2);

		/* Append decrypted body to plaintext */
		plaintext.insert(plaintext.end(), em.m_body_dec.begin(), em.m_body_dec.end());

		/* Append signature to plaintext */
		plaintext.insert(plaintext.end(), signature.begin(), signature.end());

		/* Plaintext is now built */

		/* Encrypt body with AES */
		CBC_Mode<AES>::Encryption aes;
		aes.SetKeyWithIV(key, key.size(), iv.data());

		/* Create pipeline and encrypt message */
		string plain((char *) plaintext.data(), plaintext.size());
		string cipher;
		StringSource ss(plain, true,
			new StreamTransformationFilter(aes,
				new StringSink(cipher)
			)
		);

		/* Build encrypted message body */
		vector<uint8_t> body;
		body.resize(2);
		body[0] = key_cipher.size() / 256;
		body[1] = key_cipher.size() % 256;
		body.insert(body.end(), key_cipher.begin(), key_cipher.end());
		body.insert(body.end(), iv.begin(), iv.end());
		body.insert(body.end(), cipher.c_str(), cipher.c_str() + cipher.length());

		em.m_body_enc = body;
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
			if (m_buddies[i].name() == name) {
				encryptMessage(em, m_buddies[i].pubkey());
				return;
			}
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

	Buddy::Buddy(RSA::PublicKey pubkey, string& n)
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
